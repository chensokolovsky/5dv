//
//  FiveDVEncDec.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 8/4/16.
//  Copyright © 2016 chen. All rights reserved.
//

/*
 LICENSE
 
 5dv - Five Dimensional Vision - File/Stream protocol, Encoder & Decoder
 @copywrite Chen Sokolovsky 2012-2017
 
 This software is not open source. You may not use it commenrisally, distribute or sublicense it. You may not upload it to any public or private server or cloud service. You may not send it via email or LAN.
 
 You may download it, read the code, modify it and run it for personal use only, on a local machine. If you wish to share it with others you may share a link to this repository
 
 The software is provided without warrenty of any kind. In no event shall the
 authors or copyright holders be liable for any claim, damages or other
 liability.
 
 */

#import "FiveDVEncDec.h"



// This method is expecting an input file, input header and output header already created
// The 3rd argument is the one we are writing into, by compressing one frame at a time
// This method should read a frame from the input file, and write a compressed chroma buffer to the output file
// Return 1 success. -1 to error
// In this version the compression is TVFramesPerZip frames at a time

int compress(FILE** uFile,struct fiveDVUFileHeader inHeader,FILE** cFile, struct FDVFileHeader outHeader) {
    
    
    /// Create a tvData for the different phases
    // ----------------------------------------
    struct EncDecData tvData;
    float width = tvData.tvsize.width = inHeader.tvsize.width;
    float height = tvData.tvsize.height = inHeader.tvsize.height;
    float depth = tvData.tvsize.depth = inHeader.tvsize.depth;
    unsigned int totalFrames = tvData.totalFrames = inHeader.totalFrames;
    
    
    if (debugMain) printf("compressFile: received an uncompressed file with TV size:  %.0f: %.0f: %.0f and totalFrames: %f\n", tvData.tvsize.width,tvData.tvsize.depth,tvData.tvsize.height, tvData.totalFrames);
    
    // The current model does note include i or p frames. This is just a regular linearencoding, frame by frame
    
    /// Now read the input file frame by frame.
    unsigned int pixelsInATVPlate = width * depth;
    unsigned int pixelsInAFrame = pixelsInATVPlate * height;
    unsigned int totalBytesInAnUncompressedTVFrame = pixelsInAFrame * sizeof(zPixel);
    zPixel* uncompressedFrameBuffer = (zPixel*)calloc(totalBytesInAnUncompressedTVFrame,1);
    
    printf("compressing frames:");
    
    int totalZipGroups = totalFrames / TVFramesPerZip;
    totalZipGroups++;
    int currentFrameIndexInTV = 0;
    
    unsigned char* tempWritingBuffer = (unsigned char*)calloc(totalBytesInAnUncompressedTVFrame * TVFramesPerZip,1);
    
    // for each group, grab the compressed image, zip the buffer, and add it to the destination file
    for (int zipGroupIndex = 0; zipGroupIndex < totalZipGroups; zipGroupIndex++ ) {
    
        
        unsigned char* currentWritingPtr = tempWritingBuffer;
        
        // For each frame in the group
        for (int currentFrameInZipGroup = 0; currentFrameInZipGroup< TVFramesPerZip; currentFrameInZipGroup++) {
            
            if (currentFrameIndexInTV +1 > totalFrames) break;
            
            // read the full frame
            fread(uncompressedFrameBuffer, 1, totalBytesInAnUncompressedTVFrame, *uFile);
            
            unsigned char* compressedFrameBuffer;
            unsigned int compressedFrameLength;
            
            compressTVFrame(&uncompressedFrameBuffer,tvData,&compressedFrameBuffer,&compressedFrameLength);
            
            // we add a little "header" to each frame. for now lets do "newTVFrame" and size
            const char* newFrameStr = "...newTVFrame...";
            memcpy(currentWritingPtr,newFrameStr,16);
            currentWritingPtr += 16;
            unsigned int* intPtr = (unsigned int*)currentWritingPtr;
            *intPtr = compressedFrameLength;
            currentWritingPtr += 4;
            
            memcpy(currentWritingPtr,compressedFrameBuffer,compressedFrameLength);
            currentWritingPtr += compressedFrameLength;
            
            free (compressedFrameBuffer);
            compressedFrameBuffer = 0;
            
            currentFrameIndexInTV++;
            
            printf(".");

        }
        
        // do zip here...
        unsigned char* compressedGroupOfTVFrames;
        unsigned int compressedGroupLengthInBytes;
        char randomNameOut[randomNameLength+1];
        randomNameOut[randomNameLength] = '\0';
        
        size_t totalBytes = currentWritingPtr - tempWritingBuffer;

        compressZip(tempWritingBuffer, (unsigned int)totalBytes, &compressedGroupOfTVFrames, &compressedGroupLengthInBytes, randomNameOut);
        
        if (debugZips) printf("zipped group with random name:%s\n",randomNameOut);
        
        // add a chunk header of the size
        const char* newGroupStr = "...newZipGroup...";
        size_t wrote = fwrite(newGroupStr, 1, 17, *cFile);
        if (wrote != 17) {printf("Error writing newZipGroup header string!\n"); return 0;}
        wrote = fwrite(randomNameOut, 1, randomNameLength, *cFile);
        if (wrote != randomNameLength) {printf("Error writing newZipGroup header random name!\n"); return 0;}
        wrote = fwrite(&compressedGroupLengthInBytes, 1, 4, *cFile);
        if (wrote != 4) {printf("Error writing newZipGroup header size!\n"); return 0;}

        // write the buffer
        wrote = fwrite(compressedGroupOfTVFrames, 1, compressedGroupLengthInBytes, *cFile);
        if (wrote != compressedGroupLengthInBytes) {
            printf("Error writing newZipGroup data!\n");
            return 0;
        }

    }
    
    
    free (uncompressedFrameBuffer);
    free (tempWritingBuffer);
    
    printf("\n");
    return 1;
}

// This method is expecting an input file, input header and output header already created
// The 3rd argument is the one we are writing into, by readinf the chroma buffers of one frame at a time, converting them to uncompressed frames, and adding them to the destination file
// Return 1 success, -1 error
// This method is written at ver 7, which only allocated buffer for a single frame. The rest are stored in the files

int decompress(FILE** cFile,struct FDVFileHeader inHeader,FILE** uFile, struct fiveDVUFileHeader outHeader) {
 
    
    
    /// Create a tvData for the different phases
    // ----------------------------------------
    struct EncDecData tvData;
    float width = tvData.tvsize.width = inHeader.tvsize.width;
    float height = tvData.tvsize.height = inHeader.tvsize.height;
    float depth = tvData.tvsize.depth = inHeader.tvsize.depth;
    unsigned int totalFrames = tvData.totalFrames = inHeader.totalFrames;
    
    if (debugMain) printf("compressFile: received an uncompressed file with TV size:  %.0f: %.0f: %.0f and totalFrames: %f\n", tvData.tvsize.width,tvData.tvsize.depth,tvData.tvsize.height, tvData.totalFrames);
    
    // The current model does note include i or p frames. This is just a regular linearencoding, frame by frame
    
    /// Now read the input file frame by frame.
    unsigned int pixelsInATVPlate = width * depth;
    unsigned int pixelsInAFrame = pixelsInATVPlate * height;
    unsigned int totalBytesInAnUncompressedTVFrame = pixelsInAFrame * sizeof(zPixel);
    zPixel* uncompressedFrameBuffer = (zPixel*)calloc(totalBytesInAnUncompressedTVFrame,1);
    
    printf("decompressing frames:");
    
    int totalZipGroups = totalFrames / TVFramesPerZip;
    totalZipGroups++;
    int currentFrameIndexInTV = 0;
    
    // we allocate a big enough buffer for the full size, but we won't use all of it
    // We read the buffer from the file, and expand frame by frame and when a block of frame is done we are writing it to the file
    unsigned char* tempReadingBuffer = (unsigned char*)calloc(totalBytesInAnUncompressedTVFrame * TVFramesPerZip,1);
    
    size_t totalBytesPerGroup = (16 + 4 + totalBytesInAnUncompressedTVFrame) * TVFramesPerZip;
    
    // for each group, grab the compressed image, zip the buffer, and add it to the destination file
    for (int zipGroupIndex = 0; zipGroupIndex < totalZipGroups; zipGroupIndex++ ) {
        
      
        // read a zip group header of the size
        char headerStrOfGroup[18];
        size_t ret = fread(headerStrOfGroup,1,17,*cFile);
        if (ret != 17) {printf("Error reading file for zip group header\n"); return 0;}
        char should[] = "...newZipGroup...";
        char* shouldBe = should;
        headerStrOfGroup[17] = '\0';
        if (strcmp(headerStrOfGroup,shouldBe) != 0) {
            printf("Error! zip group should start with ...newZipGroup... string\n");
            return 0;
        }
        
        char randomName[randomNameLength+1];
        ret = fread(randomName,1,randomNameLength,*cFile);
        if (ret != randomNameLength) {printf("Error reading file for zip group random name\n"); return 0;}
        randomName[randomNameLength] = '\0';

        // read the group header size
        unsigned int zipGroupLengthInBytes = 0;
        ret = fread(&zipGroupLengthInBytes,1,4,*cFile);
        if (ret != 4) {printf("Error reading file for zip group size\n"); return 0;}
        
        // read the zipped data
        ret = fread(tempReadingBuffer,1,zipGroupLengthInBytes,*cFile);
        if (ret != zipGroupLengthInBytes) {printf("Error reading file for zip group data\n"); return 0;}

        //now we can unzip the data
        unsigned char* unzipedGroup;
        unsigned int unzipedGroupSizeInBytes;
        if (debugZips) printf("random name in is:%s\n",randomName);
        
        expandZip(tempReadingBuffer, zipGroupLengthInBytes,randomName ,&unzipedGroup, &unzipedGroupSizeInBytes);
        
        
        unsigned char* currentReadingPointer = unzipedGroup;
        
        // For each frame in the group
        for (int currentFrameInZipGroup = 0; currentFrameInZipGroup< TVFramesPerZip; currentFrameInZipGroup++) {

             if (currentFrameIndexInTV +1 > totalFrames) break;
            
            
            // read the little header
            // Verify file starts with 5DVU string
            char headerStr[17];
            memcpy(headerStr,currentReadingPointer,16);
            char should[] = "...newTVFrame...";
            char* shouldBe = should;
            headerStr[16] = '\0';
            if (strcmp(headerStr,shouldBe) != 0) {
                printf("Error! frame should start with ...newTVFrame... string\n");
                return -1;
            }
            
            currentReadingPointer += 16;
            
            unsigned int* intPrt = (unsigned int*)currentReadingPointer;
            
            unsigned int frameCompressedLength = intPrt[0];
            currentReadingPointer += 4;

            // allocate buffer for compressed frame
            unsigned char* compressedFrameBuffer = (unsigned char*)calloc(frameCompressedLength,1);
            
            // decompress the frame
            decompressTVFrame(&currentReadingPointer, frameCompressedLength, tvData, &uncompressedFrameBuffer);
            
            currentReadingPointer += frameCompressedLength;
            
            // add it to the uncompressed file
            fwrite(uncompressedFrameBuffer, 1, totalBytesInAnUncompressedTVFrame, *uFile);
                
            currentFrameIndexInTV++;

            
            // clean up
            free (compressedFrameBuffer);
            
            printf(".");
        }
    
    }
    
    // clean up
    free (uncompressedFrameBuffer);
    
    printf("\n");
    return 1;

}







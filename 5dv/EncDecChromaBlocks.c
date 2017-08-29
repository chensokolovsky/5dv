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

#import "EncDecChromaBlocks.h"

// taken from wikipedia https://en.wikipedia.org/wiki/YCbCr
void convertRGBColorToYCrCb(struct zPixel rgbColor, struct yCbCr* yCbCrColor) {
    
    yCbCrColor->y = (0.299 * rgbColor.R) + (0.587 * rgbColor.G) + (0.114 * rgbColor.B);
    yCbCrColor->Cb = 128 - (0.168736 * rgbColor.R) - (0.331264 * rgbColor.G) + (0.5 * rgbColor.B);
    yCbCrColor->Cr = 128 + (0.5 * rgbColor.R) - (0.418688 * rgbColor.G) - (0.081312 * rgbColor.B);
 
}

void convertYCrCbToRGBColor(struct yCbCr yCbCrColor, struct zPixel* rgbColor) {
    
    rgbColor->R = yCbCrColor.y + 1.402 * (yCbCrColor.Cr - 128);
    rgbColor->G = yCbCrColor.y - 0.344136 * (yCbCrColor.Cb - 128) - 0.714136 * (yCbCrColor.Cr - 128);
    rgbColor->B = yCbCrColor.y + 1.772 * (yCbCrColor.Cb - 128);
}


// destination should be allocated
void convertRGBPixelsToYCrCb(struct zPixel* pixels, int totalPixels, struct yCbCrGroup* destination) {
    
    // we habe totalPixels worth of zPixel structures
    // we want to keep the y and alpha, and avarage the Cr and Cb
    int CrSum = 0;
    int CbSum = 0;
    
    unsigned char* writingPointer = (unsigned char*)destination->YandAlpha;
    
    for (int i=0; i<totalPixels; i++) {
        zPixel pixel = pixels[i];
        struct yCbCr dest;
        convertRGBColorToYCrCb(pixel, &dest);
        CrSum += dest.Cr;
        CbSum += dest.Cb;
        *writingPointer = dest.y;
        writingPointer++;
        *writingPointer = pixel.alpha;
        writingPointer++;
    }
    
    destination->Cb = CbSum / totalPixels;
    destination->Cr = CbSum / totalPixels;
    
}

// destination should be already allocated
void convertYCrCbGroupToPixels(struct yCbCrGroup* yCbCrGroup, int totalPixels,struct zPixel* destination) {
    
    // we have a common Cr and Cb for all pixels.
    // convert to RGB from the data
    unsigned char* readingPointer = (unsigned char*) yCbCrGroup->YandAlpha;
    for (int i=0; i< totalPixels; i++) {
        
        zPixel* pixel = &(destination[i]);
        struct yCbCr ycrcbPixel;
        ycrcbPixel.Cb = yCbCrGroup->Cb;
        ycrcbPixel.Cr = yCbCrGroup->Cr;
        ycrcbPixel.y = *readingPointer;
        readingPointer++;
        
        convertYCrCbToRGBColor(ycrcbPixel, pixel);
        
        pixel->alpha = *readingPointer;
        readingPointer++;
        
    }
    

}



// This method is expecting an input buffer, tv data and output buffer
// The 3rd argument is the one we are writing into
// This method should read a frame from the input buffer, and write a compressed chroma buffer to the output buffer
// Return 1 success. -1 to error
// The outBuffer is allocated in this method, and should be deallocated by one of the callers

int performPhaseEncodeChromaBlocks(unsigned char** inBuffer,struct EncDecData tvData,unsigned char** outBuffer) {

    /// Allocate memory for the compressed buffer
    // ----------------------------------------
    float width = tvData.tvsize.width;
    float height = tvData.tvsize.height;
    float deapth = tvData.tvsize.deapth;
    int totalFrames = tvData.totalFrames;
    
    if (debugMain) printf("compressFile: received an uncompressed file with TV size:  %.0f: %.0f: %.0f and totalFrames: %d\n", width,deapth,height, totalFrames);
    
    unsigned int pixelsInAChromaBlock = chromaBlockWidth * chromaBlockHeight * chromaBlockDeapth;
    // 2 is cb and cr, total pixels times y + alpha
    unsigned int bytesInAChromaStreamPerChromaBlock = 2 + pixelsInAChromaBlock * 2;
    
    unsigned int pixelsInATVPlate = width * deapth;
    
    int totalChromaPlates = height / chromaBlockHeight;
    int totalRowsInPlate = width / chromaBlockWidth;
    int totalColumnsInPlate = deapth / chromaBlockDeapth;
    unsigned int totalChromaBlocksInPlate = totalRowsInPlate * totalColumnsInPlate;
    unsigned int totalChromaBlocksInTV = totalChromaPlates * totalChromaBlocksInPlate;
    
    if (debugMain) printf("Bytes in a chroma block: %d\nChroma blocks in TV: %d\n",bytesInAChromaStreamPerChromaBlock,totalChromaBlocksInTV);
    
    
    unsigned int totalBytesForAllChromaBlocksPerFrame = bytesInAChromaStreamPerChromaBlock * totalChromaBlocksInTV;
    
    // Now, we can allocate the destination buffer (chroma)
    unsigned char* chromaBufferOfFrame = (unsigned char*)calloc(totalBytesForAllChromaBlocksPerFrame, 1);
    
    if (chromaBufferOfFrame == NULL || !chromaBufferOfFrame) {
        printf("Error allocating buffer size %u bytes", totalBytesForAllChromaBlocksPerFrame);
        return -1;
    }
    else {
        if (debugMain) printf("Allocated %u bytes for compressed buffer\n",totalBytesForAllChromaBlocksPerFrame);
    }
    

    /// Now read the input file frame by frame.
    zPixel* uncompressedFrameBuffer = (zPixel*)*inBuffer;
    
    
    unsigned char* writingBuffer = chromaBufferOfFrame;
    
    // Start converting pixels to chroma blocks
    // -----------------------------------------
    for (int heightIndex = 0; heightIndex < totalChromaPlates; heightIndex++) {
        for (int deapthIndex = 0; deapthIndex < totalColumnsInPlate; deapthIndex++) {
            for (int widthIndex = 0; widthIndex < totalRowsInPlate; widthIndex++) {
                
                // for each chroma block, we want to convert from RGBa to a block
                // first we need to gather the indexes of the group (the block) from the full rgb buffer;
                
                // Create the input:
                //------------------
                // how many pixels in a group?
                int totalPixelsInGroup = chromaBlockHeight * chromaBlockWidth * chromaBlockDeapth;
                zPixel* inputBuffer = (zPixel*)malloc(totalPixelsInGroup * sizeof(zPixel));
                int totalPixelsOnAPlate = width*deapth;
                
                // get the first index. This is the height so far
                unsigned long long int indexOfFirstPixel = totalPixelsOnAPlate * heightIndex * chromaBlockHeight;
                // plus the deapth so far
                indexOfFirstPixel += deapthIndex * chromaBlockWidth * width;
                // plus the width so far
                indexOfFirstPixel += widthIndex * chromaBlockWidth;
                
                
                // now, place this widths on the input buffer
                // we need three loops here as well, one for block heights,
                // one for blocks deapth, and one for widths, filling the entire block with pixels from the right indexes
                
                int indexInDestinationBuffer = 0;
                for (int blocksInnerHeight=0; blocksInnerHeight< chromaBlockHeight; blocksInnerHeight++) {
                    for (int blocksInnerDeapth = 0; blocksInnerDeapth< chromaBlockDeapth;blocksInnerDeapth++) {
                        for (int blocksInnerWidth = 0; blocksInnerWidth< chromaBlockWidth;blocksInnerWidth++) {
                            
                            unsigned long long int indexOnTV = indexOfFirstPixel + blocksInnerWidth + blocksInnerDeapth * width + blocksInnerHeight * pixelsInATVPlate;
                            
                            zPixel* destinationChroma = &(inputBuffer[indexInDestinationBuffer]);
                            *destinationChroma = uncompressedFrameBuffer[indexOnTV];
                            
                            indexInDestinationBuffer++;
                        }
                    }
                }
                
                
                // Create the output;
                struct yCbCrGroup resultGroup;
                resultGroup.YandAlpha = (unsigned char*)malloc(totalPixelsInGroup * 2);
                
                // convert
                convertRGBPixelsToYCrCb(inputBuffer,totalPixelsInGroup,&resultGroup);
                
                // add to the buffer
                writingBuffer[0] = resultGroup.Cb;
                writingBuffer[1] = resultGroup.Cr;
                memcpy(&(writingBuffer[3]), resultGroup.YandAlpha, totalPixelsInGroup*2);
                writingBuffer += totalPixelsInGroup * 2 + 2;
                
                free(resultGroup.YandAlpha);
                free (inputBuffer);
            }
        }
    }
    
    // now, set the destination pointer to point to the chroma buffer
    *outBuffer = (unsigned char*)chromaBufferOfFrame;

    
    return 1;
}

// This method is expecting an input buffer, tv data and output buffer
// The 3rd argument is the one we are writing into, by reading the chroma buffer, converting them to uncompressed zPixels, and adding them to the destination buffer
// Return 1 success, -1 error
// Third argument is allocated within this method and should be freed by the caller

int performPhaseDecodeChromaBlocks(unsigned char** inBuffer,struct EncDecData tvData,unsigned char** outBuffer) {

    // The input buffer (the first parameter)  is a buffer of Cb Cr Y and Alpha groups. These are currently 2*2*2, meaning 2 + 16 bytes.
    
    int height = tvData.tvsize.height;
    int width =  tvData.tvsize.width;
    int deapth =  tvData.tvsize.deapth;
    
    
    unsigned int totalPixelsInTheTV = height * width * deapth;
    unsigned int totalBytesPerTVFrame = totalPixelsInTheTV * sizeof(zPixel);
    

    
    // allocate memory for the uncompressed frame
    zPixel* uncompressedFrameBuffer = (zPixel*)malloc(totalBytesPerTVFrame);
    
    //start reading to the file buffer
    
    unsigned int pixelsInATVPlate = width * deapth;
    
    int totalPlates = height / chromaBlockHeight;
    int totalRowsInPlate = width / chromaBlockWidth;
    int totalColumnsInPlate = deapth / chromaBlockDeapth;
    
    // allocate mempry for reading the croma buffer per frame
    unsigned char* chromaBufferPerFrame = *inBuffer;
    
    
    // read a chroma buffer from file
    unsigned char* readingPointer = chromaBufferPerFrame;

    
    // Start converting chroma blocks to pixels
    // -----------------------------------------
    for (int heightIndex = 0; heightIndex < totalPlates; heightIndex++) {
        for (int deapthIndex = 0; deapthIndex < totalColumnsInPlate; deapthIndex++) {
            for (int widthIndex = 0; widthIndex < totalRowsInPlate; widthIndex++) {
                
                // for each chroma block, we want to convert from RGBa to a block
                // first we need to gather the indexes of the group (the block) from the full rgb buffer;
                
                // Create the input:
                //------------------
                // how many pixels in a group?
                int totalPixelsInGroup = chromaBlockHeight * chromaBlockWidth * chromaBlockDeapth;
                
                // allocate the destination buffer of zPixels
                zPixel* resultDecompressedPixelBuffer = (zPixel*)malloc(totalPixelsInGroup * sizeof(zPixel));
                
                // read from the buffer
                struct yCbCrGroup group;
                group.Cb = readingPointer[0];
                group.Cr = readingPointer[1];
                group.YandAlpha = (unsigned char*)calloc(1, totalPixelsInGroup*2);
                memcpy(group.YandAlpha, &(readingPointer[2]), totalPixelsInGroup*2);
                readingPointer += totalPixelsInGroup * 2 + 2;
                
                
                // convert to zPixels
                convertYCrCbGroupToPixels(&group, totalPixelsInGroup ,resultDecompressedPixelBuffer);
                
                // now place the result on the uncompressed tv buffer
                
                int totalPixelsOnAPlate = width*deapth;
                
                // get the first index. This is the height so far
                unsigned long long int indexOfFirstPixel = totalPixelsOnAPlate * heightIndex * chromaBlockHeight;
                // plus the deapth so far
                indexOfFirstPixel += deapthIndex * chromaBlockDeapth * width;
                // plus the width so far
                indexOfFirstPixel += widthIndex * chromaBlockWidth;
                
                
                // for each pixel
                int indexInDestinationBuffer = 0;
                for (int blocksInnerHeight=0; blocksInnerHeight< chromaBlockHeight; blocksInnerHeight++) {
                    for (int blocksInnerDeapth = 0; blocksInnerDeapth< chromaBlockDeapth;blocksInnerDeapth++) {
                        for (int blocksInnerWidth = 0; blocksInnerWidth< chromaBlockWidth;blocksInnerWidth++) {
                            
                            unsigned long long int indexOnTV =  indexOfFirstPixel + blocksInnerWidth + blocksInnerDeapth * width + blocksInnerHeight * pixelsInATVPlate;
                            
                            uncompressedFrameBuffer[indexOnTV] = resultDecompressedPixelBuffer[indexInDestinationBuffer];
                            
                            if (debugMain) {
                                if (heightIndex == 0 && widthIndex == 0 && deapthIndex == 0 && blocksInnerHeight == 0 &&  blocksInnerDeapth == 0 && blocksInnerWidth == 0) {
                                    printf("filling buffer from %llu\n", indexOnTV);
                                }
                            }
                            
                            indexInDestinationBuffer++;
                        }
                    }
                }
                
                free (group.YandAlpha);
            }
        }
    }
    
  
    
    /// now, set the out buffer to point to the uncompressed buffer
    *outBuffer = (unsigned char*)uncompressedFrameBuffer;

    
    return 1;
}

// returns 0 if all zeros but looks ok
// return 1 if non zero value exists
int verifyBuffer(void* buffer, size_t length) {
    int foundNonZero = 0;
    for (size_t k=0; k<length;k++) {
        unsigned char val = ((unsigned char*)buffer)[k];
        if (val != 0) return 1;
    }
    if (!foundNonZero) return 0;
    return -1;
}




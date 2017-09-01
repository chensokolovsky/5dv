//
//  FDVFile.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 8/18/16.
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

#import "FDVFile.h"

//ver 7. 1st parameter should exist on disk. second is created. 3rd is 1 to compress 0 to expand (decompress)
int convert(char* inputPath, char* outputPath, int shouldCompress) {
    
    if (shouldCompress) {
        
        // read the 5dvU file
        FILE* inf;
        struct fiveDVUFileHeader inHeader;
        openFiveDVUFile(inputPath, &inHeader, &inf);

        // create a new 5dv file
        struct FDVFileHeader outHeader;
        outHeader.framesPerSecond = inHeader.framesPerSecond;
        outHeader.totalFrames = inHeader.totalFrames;
        outHeader.tvsize = inHeader.tvsize;
        struct TVSize chromaBlockSize;
        chromaBlockSize.width = chromaBlockWidth;
        chromaBlockSize.height = chromaBlockHeight;
        chromaBlockSize.depth = chromaBlockDeapth;
        outHeader.chromaBlockSize = chromaBlockSize;
        
        // Single frame
        //outHeader.totalFrames = 1;
        
        FILE* outf;
        createFDVFile(outputPath, outHeader, &outf);
        
        // convert the buffers
        compress(&inf, inHeader, &outf, outHeader);
        
        fclose(outf);

        
        return 1;
    }
    
    else {
        
        // read the compressed file
        FILE* inf;
        struct FDVFileHeader inHeader;
        openFDVFile(inputPath, &inHeader, &inf);
        
        // create a new fiveDVU file
        fiveDVUFileHeader outHeader;
        outHeader.framesPerSecond = inHeader.framesPerSecond;
        outHeader.totalFrames = inHeader.totalFrames;
        outHeader.tvsize = inHeader.tvsize;
        
        FILE* outf;
        createFiveDVUFile(outputPath, outHeader, &outf);
        
        // convert
        decompress(&inf, inHeader, &outf, outHeader);
        
        fclose(outf);
        
        
        return 1;
    }
    
    
    return -1;
    
}




int createFDVFile(char* name, struct FDVFileHeader header, FILE** fp) {
    
    if (debugMain) printf("saving file...\n");
    
    *fp = fopen(name, "w+");
    const char* headerStr = "5DV1";
    fwrite(headerStr, 4, 1, *fp);
    unsigned int tvwidth = header.tvsize.width;
    fwrite(&tvwidth,4,1,*fp);
    unsigned int tvheight = header.tvsize.height;
    fwrite(&tvheight,4,1,*fp);
    unsigned int tvDeapth = header.tvsize.depth;
    fwrite(&tvDeapth,4,1,*fp);
    unsigned int totalFrames = header.totalFrames;
    fwrite(&totalFrames,4,1,*fp);
    unsigned int framesPerSecond = header.framesPerSecond;
    fwrite(&framesPerSecond,4,1,*fp);
    unsigned char chromaWidth = header.chromaBlockSize.width;
    unsigned char chromaDeapth = header.chromaBlockSize.depth;
    unsigned char chromaHeight = header.chromaBlockSize.height;
    unsigned char zero = 0;
    fwrite(&chromaWidth, 1, 1, *fp);
    fwrite(&chromaDeapth, 1, 1, *fp);
    fwrite(&chromaHeight, 1, 1, *fp);
    fwrite(&zero, 1, 1, *fp);
    
    //unsigned int totalDataBufferSize = header.dataSizeInBytes;
    //fwrite(&totalDataBufferSize, 4,1,fp);
    
    return 1;
    
}
int openFDVFile(char* name, struct FDVFileHeader* header, FILE** fp) {
    
    *fp = fopen(name, "rb");
    
    // Verify file starts with 5DVU string
    char headerStr[4];
    fread(headerStr,1,3,*fp);
    char should[] = "5DV";
    char* shouldBe = should;
    headerStr[3] = '\0';
    if (strcmp(headerStr,shouldBe) == 0) {
        if (debugMain) printf("header starts with 5DV\n");
    }
    else {
        printf("Error. header should start with 5DV");
        return 0;
    }
    
    char version[2];
    fread(version,1,1,*fp);
    char versionOne[] = "1";
    char* ver = versionOne;
    version[1] = '\0';
    if (strcmp(version,ver) == 0) {
        if (debugMain) printf("This is a 5dv version 1 file\n");
    }
    else {
        printf("Error. this file is not version 1");
        return 0;
    }
    
    // read tv size into file
    unsigned int tvwidth;
    fread(&tvwidth,4,1,*fp);
    unsigned int tvheight;
    fread(&tvheight,4,1,*fp);
    unsigned int tvDeapth;
    fread(&tvDeapth,4,1,*fp);
    header->tvsize.width = tvwidth;
    header->tvsize.height = tvheight;
    header->tvsize.depth = tvDeapth;
    
    // read the total frames and frames per second
    unsigned int totalFrames;
    fread(&totalFrames,4,1,*fp);
    unsigned int framesPerSecond;
    fread(&framesPerSecond,4,1,*fp);

    
    header->totalFrames = totalFrames;
    header->framesPerSecond = framesPerSecond;
    
    
    unsigned char chromaWidth;
    unsigned char chromaDeapth;
    unsigned char chromaHeight;
    unsigned char zero;
    fread(&chromaWidth, 1, 1, *fp);
    fread(&chromaDeapth, 1, 1, *fp);
    fread(&chromaHeight, 1, 1, *fp);
    fread(&zero, 1, 1, *fp);
    
    struct TVSize chromaSize;
    chromaSize.width = chromaWidth;
    chromaSize.height = chromaHeight;
    chromaSize.depth = chromaDeapth;
    
    header->chromaBlockSize = chromaSize;
    
    return 1;
    
}














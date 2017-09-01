//
//  FiveDVUFile.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/27/16.
//  Copyright © 2016 chen. All rights reserved.
//
/*
 LICENSE
 
 5dv - Five Dimensional Vision - File/Stream protocol, Encoder & Decoder
 @copywrite Chen Sokolovsky 2012-2017
 
 This software is not open source. You may not use it commenrically, distribute
 or sublicense it without authorization. You may not upload it to any public or private server or
 cloud service without authorization. You may not send it via email or LAN without authorization.
 
 You may download it, read the code, modify it and run it for personal use only,
 on a local machine. If you wish to share it with others you may share a link
 to this repository.
 
 The software is provided without warranty of any kind. In no event shall the
 authors or copyright holders be liable for any claim, damages or other
 liability.
 
 */


#import "FiveDVUFile.h"



// create a new file and write the header
// file is created within thie method
// return 1 for suceess
int createFiveDVUFile(char* fileName, struct fiveDVUFileHeader header,FILE **fp) {
    
    if (debugMain) printf("saving file...\n");
    
    *fp = fopen(fileName, "w+");
    if (!fp) return -1;
    
    const char* headerID = "5DVU";
    size_t ret = fwrite(headerID, 4, 1, *fp);
    if (ret != 1) return -1;
    unsigned int tvwidth = header.tvsize.width;
    ret = fwrite(&tvwidth,4,1,*fp);
    if (ret != 1) return -1;
    unsigned int tvheight = header.tvsize.height;
    ret = fwrite(&tvheight,4,1,*fp);
    if (ret != 1) return -1;
    unsigned int tvDeapth = header.tvsize.depth;
    ret = fwrite(&tvDeapth,4,1,*fp);
    if (ret != 1) return -1;
    unsigned int totalFrames = header.totalFrames;
    ret = fwrite(&totalFrames,4,1,*fp);
    if (ret != 1) return -1;
    unsigned int framesPerSecond = header.framesPerSecond;
    ret = fwrite(&framesPerSecond,4,1,*fp);
    if (ret != 1) return -1;

    return 1;
    
}

// add images to the file
int writeFrameToFiveDVUFile(struct TVSize size, zPixel* buffer, FILE **fp) {
    
    unsigned int sizeOfEachFrameInPixels = size.width * size.height * size.depth;
    int sizeofpixel = sizeof(zPixel);
    unsigned int sizeOfEachFrameInBytes = sizeOfEachFrameInPixels * sizeofpixel;
   
    size_t ret = fwrite(buffer,1,sizeOfEachFrameInBytes,*fp);
    if (ret != 1) return -1;
   
    return 1;
    
}

// OR READ
// open a file
// file is loaded into fp inside this method
// header will get allocated within thie method
int openFiveDVUFile(char* fileName, fiveDVUFileHeader* header, FILE** fp) {
    
    *fp = fopen(fileName, "rb");
    
    // Verify file starts with 5DVU string
    char headerStr[5];
    size_t ret = fread(headerStr,1,4,*fp);
    if (ret != 4) return -1;
    char should[] = "5DVU";
    char* shouldBe = should;
    headerStr[4] = '\0';
    if (strcmp(headerStr,shouldBe) == 0) {
       if (debugMain) printf("header starts with 5dvu\n");
    }
    else return -1;
    
    // read tv size into file
    unsigned int tvwidth;
    ret = fread(&tvwidth,4,1,*fp);
    if (ret != 1) return -1;
    unsigned int tvheight;
    ret = fread(&tvheight,4,1,*fp);
    if (ret != 1) return -1;
    unsigned int tvDeapth;
    ret = fread(&tvDeapth,4,1,*fp);
    if (ret != 1) return -1;
    header->tvsize.width = tvwidth;
    header->tvsize.height = tvheight;
    header->tvsize.depth = tvDeapth;
    
    // read the total frames and frames per second
    unsigned int totalFrames;
    ret = fread(&totalFrames,4,1,*fp);
    if (ret != 1) return -1;
    unsigned int framesPerSecond;
    ret = fread(&framesPerSecond,4,1,*fp);
    if (ret != 1) return -1;

    header->totalFrames = totalFrames;
    header->framesPerSecond = framesPerSecond;
    
    return 1;

    
}
// read images
// buffer should be allocated already to a TV size. assuming we will keep reading to the same buffer that will get freed at the end
int readFrameFromFiveDVUFile(struct TVSize size, zPixel* buffer, unsigned int frameNumber, FILE** fp) {
    
    //malloc data of file
    unsigned int totalPixelsInAFrame = size.depth * size.width * size.height;
    unsigned int totalBytesPerFrame = totalPixelsInAFrame * sizeof(zPixel);
    
    //seek
    unsigned int offset = sizeof(fiveDVUFileHeader);
    offset += frameNumber * totalBytesPerFrame;
    int res = fseek(*fp, offset, SEEK_SET);
    if (res != 0) return -1;
    
    // read the data
    size_t ret = fread(buffer, 1,totalBytesPerFrame,*fp);
    if (ret != totalBytesPerFrame) return -1;
        
    return 1;
    
}



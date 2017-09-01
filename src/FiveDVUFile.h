//
//  FiveDVUFile.h
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

#ifndef FiveDVUFile_h
#define FiveDVUFile_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include "SimDefs.h"


typedef struct fiveDVUFileHeader {
    struct TVSize tvsize;
    float totalFrames;
    float framesPerSecond;
}fiveDVUFileHeader;

/*
struct fiveDVUFile {
    struct fiveDVUFileHeader header;
    struct zPixel* data;
};
*/

//// Starting version 7, Aug 23 2016, the file should not write/read full buffers.
/// Instead, we should create a file pointer and save/load the heaser, then read/write one frame at a time.
// This way we are dealing with a lot less memory in the buffer
// So these two methods below should get replaced.
//int readFiveDVUFile( char* fileName, struct fiveDVUFile* file);
//int writeFiveDVUFile(char* fileName, struct fiveDVUFile* file);


// The new methods should be something like
// WRITE
// create a new file and write the header
int createFiveDVUFile(char* fileName, struct fiveDVUFileHeader header,FILE **fp);
// add images to the file
int writeFrameToFiveDVUFile(struct TVSize size, zPixel* buffer, FILE **fp);

// OR READ
// open a file
int openFiveDVUFile(char* fileName,fiveDVUFileHeader* header, FILE** fp);
// read images
int readFrameFromFiveDVUFile(struct TVSize size, zPixel* buffer, unsigned int frameNumber, FILE** fp);

















#endif /* FiveDVUFile_h */

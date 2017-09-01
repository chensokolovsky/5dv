//
//  FDVFile.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 8/18/16.
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


#ifndef FDVFile_h
#define FDVFile_h


// This is the compressed form of the file
#include <stdio.h>
#include <SDL2/SDL.h>
#include  "SimDefs.h"
#include "FiveDVEncDec.h"
#include "FiveDVUFile.h"




/*
struct FDVFile {
    struct FDVFileHeader header;
    void* data;
};
*/

// These were used before ver 7.
// compress FiveDVU to FDVFile structures
//int convert(struct fiveDVUFile *uncompressedFile, struct FDVFile* compressedFile);
//int deconvert(struct FDVFile* compressedFile, struct fiveDVUFile* uncomporessedFile);

// starting v7, convert paths only
// input path should exist on disk. output will get created within the method
int convert(char* inputPath, char* outputPath, int shouldCompress);




// Same as with the uncompressed version. Starting version 7, we no longer want to save/load the enntire buffer
// We have new methods to create/read a file and its header, and methods to write/read a single frame from the buffer
/// This is dont not to allocate memory for a full movie buffer.
// These two methods won't be used anymore
//int saveFDVFile(struct FDVFile* file,char* name);
//int loadFDVFile(char* name, struct FDVFile* file);


// Instead we will use methods like these:
int createFDVFile(char* name, struct FDVFileHeader header, FILE** fp);
int openFDVFile(char* name, struct FDVFileHeader* header, FILE** fp);

// Writing and reading will just use the regular FILE write/read methods. No need for a special interface really
// The writer/reader will make sure to read/write in buffers not larger than a frame
// example:  fwrite(buffer,totalBytes,1,fp);
//  fread(buffer,totalBytes,1,fp);
//
// Also, caller should close the file
// fclose(fp)
//


#endif /* FDVFile_h */

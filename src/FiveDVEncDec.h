//
//  FiveDVEncDec.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 8/4/16.
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

#ifndef FiveDVEncDec_h
#define FiveDVEncDec_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include "FiveDVUFile.h"
#include "EncDecDefines.h"
#include "FiveDVFrameEncDec.h"




// New methods (Ver 7) usses file object and read/write one frame at a time
int compress(FILE** uFile,struct fiveDVUFileHeader inHeader,FILE** cFile, struct FDVFileHeader outHeader);
int decompress(FILE** cFile,struct FDVFileHeader inHeader,FILE** uFile, struct fiveDVUFileHeader outHeader);

#endif /* FiveDVEncDec_h */

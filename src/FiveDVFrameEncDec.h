//
//  FiveDVFrameEncDec.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 5/24/17.
//  Copyright © 2017 chen. All rights reserved.
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

#ifndef FiveDVFrameEncDec_h
#define FiveDVFrameEncDec_h

#include <stdio.h>
#include "SimDefs.h"
#include "EncDecDefines.h"
#include "EncDecChromaBlocks.h"
#include "EncDecMacroBlocks.h"
#include "EncDecDCT.h"
#include "EncDecQuantization.h"
#include "EncDecZigZag.h"
#include "zips.h"

// when compressing a frame we know the uncompressed size, but not the compressed size. the last argument will retun the size
int compressTVFrame(zPixel** uncompressedFrameBuffer,struct EncDecData tvData,unsigned char** compressedFrameBuffer,unsigned int * compressedFrameLength);

// decompressing should contain the length of the input buffer
int decompressTVFrame(unsigned char** compressedFrameBuffer, unsigned int compressedFrameLength, struct EncDecData tvData , zPixel**uncompressedFrameBuffer);


#endif /* FiveDVFrameEncDec_h */

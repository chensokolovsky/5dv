//
//  EncDecMacroBlocks.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 5/25/17.
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

#ifndef EncDecMacroBlocks_h
#define EncDecMacroBlocks_h



#include <stdio.h>
#include "FiveDVUFile.h"
#include "EncDecDefines.h"


int performPhaseEncodeMacroBlocks(unsigned char** inBuffer, struct EncDecData tvData, unsigned char** outBuffer, unsigned int* outputBufferLength);

int performPhaseDecodeMacroBlocks(unsigned char** inBuffer, struct EncDecData tvData, unsigned char** outBuffer);


#endif /* EncDecMacroBlocks_h */

//
//  ZigZag.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 6/20/17.
//  Copyright © 2017 chen. All rights reserved.
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


#ifndef ZigZag_h
#define ZigZag_h

#include <stdio.h>
#include <SDL2/SDL.h>
#include "EncDecDefines.h"


int generateZigZagMatrix(short int* resultMatrix);

#endif /* ZigZag_h */

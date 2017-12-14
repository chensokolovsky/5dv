//
//  fastDct.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 6/19/17.
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

#ifndef fastDct_h
#define fastDct_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define N 8

int initFastDCTTwo(float* c);

int initFastDCT(float* c);
int fast3DDct(float* in, float* out,float* c);
int fast3DIDct(float* in, float* out,float* c);

#endif /* fastDct_h */

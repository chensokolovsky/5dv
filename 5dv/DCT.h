//
//  DCT.h
//  DCTLab
//
//  Created by Chen Sokolovsky on 6/10/17.
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

#ifndef DCT_h
#define DCT_h

#include <stdio.h>
#include <math.h>
#include "EncDecDefines.h"
#include "fastDct.h"

//#define N macroBlockDimension


int performDCT(unsigned char* input, short int* output, int dimensions);
int performIDCT( short int* input, unsigned char* output, int dimensions);


// 1-D
int perform1DDCT(unsigned char* input, short int* output);
int perform1DIDCT(short int* input, unsigned char* output);

int performFloatDCT(float* input, float* output);
int performFloatIDCT(float* input, float* output);


// 2-D
int perform2DDCT(unsigned char* input, short int* output);
int perform2DIDCT(short int* input, unsigned char* output);

int performFloat2DDCT(float* input, float* output);
int performFloat2DIDCT(float* input, float* output);

// 3-D
int perform3DDCT(unsigned char* input, short int* output);
int perform3DIDCT(short int* input, unsigned char* output);

int performFloat3DDCT(float* input, float* output);
int performFloat3DIDCT(float* input, float* output);

// 3-D fast
int performFast3DDCT(unsigned char* input, short int* output,float* c);
int performFast3DIDCT(short int* input, unsigned char* output, float* c);

#endif /* DCT_h */

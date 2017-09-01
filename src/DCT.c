//
//  DCT.c
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

#import "DCT.h"

// Thanks to this pages for some code and math formulas:
/*
 https://unix4lyfe.org/dct-1d/
 https://unix4lyfe.org/dct/
*/

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
# pragma mark general
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int performDCT(unsigned char* input, short int* output, int dimensions) {
    
    if (dimensions == 1) {
        return perform1DDCT(input, output);
    }
    else if (dimensions == 2) {
        return perform2DDCT(input,output);
    }
    else if (dimensions == 3) {
        return perform3DDCT(input, output);
    }
    
    return 0;
}
int performIDCT( short int* input, unsigned char* output, int dimensions) {
    if (dimensions == 1) {
        return perform1DIDCT(input, output);
    }
    else if (dimensions == 2) {
        return perform2DIDCT(input,output);
    }
    else if (dimensions == 3) {
        return perform3DIDCT(input, output);
    }
    return 0;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
#pragma  mark 1-D transforms
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

int perform1DDCT(unsigned char* input, short int* output) {
    
    
    float calibratedIn[N];
    float calibratedOut[N];
    
    for (int k=0; k<N;k++) calibratedIn[k] = (input[k] - 128) / 128.0;
    
    performFloatDCT(calibratedIn,calibratedOut);
    
    for (int k=0;k<N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0);
        if (val < 0) val = 0;
        if (val > 4096) val = 4096;
        output[k] = val;
    }
    
    return 1;
}

int performFloatDCT(float* input, float* output) {
    for (int k = 0; k < N; ++k) {
        float sum = 0.;
        float factor = k==0 ? sqrt(.5) : 1.;
        for (int n = 0; n < N; ++n) {
            sum +=  input[n] * cos(M_PI * (n + .5) * k / N);
        }
        output[k] = factor * sum * sqrt(2. / N);
    }
    return 1;
}

int perform1DIDCT(short int* input, unsigned char* output) {
    
    float calibratedIn[N];
    float calibratedOut[N];
    
    for (int k=0; k<N;k++) calibratedIn[k] = input[k]  / 128.0;
    
    performFloatIDCT(calibratedIn, calibratedOut);
    
    for (int k=0;k<N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0) + 128;
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        output[k] = val;
    }
    
    
    return 1;
}

int performFloatIDCT(float* input, float* output) {
   
    for (int n = 0; n < N; ++n) {
        float sum = 0.;
        for (int k = 0; k < N; ++k) {
            float factor = k==0 ? sqrt(.5) : 1.;
            sum += factor * input[k] * cos(M_PI * (n + .5) * k / N);
        }
        output[n] = sum * sqrt(2. / N);
    }
    
    return 1;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
#pragma  mark 2-D transforms
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

// result is a -2048 - 2048 values
int perform2DDCT(unsigned char* input, short int* output) {
    
    float calibratedIn[N*N];
    float calibratedOut[N*N];
    
    for (int k=0; k<N*N;k++) calibratedIn[k] = (input[k] - 128) / 128.0;

    performFloat2DDCT(calibratedIn, calibratedOut);
    
    for (int k=0;k<N*N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0);
        output[k] = val;
    }
    
    
    return 1;
}
int perform2DIDCT(short int* input, unsigned char* output) {
    
    float calibratedIn[N*N];
    float calibratedOut[N*N];
    
    for (int k=0; k<N*N;k++) calibratedIn[k] = input[k] / 128.0;
    
    performFloat2DIDCT(calibratedIn, calibratedOut);
    
    for (int k=0;k<N*N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0) + 128;
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        output[k] = val;
    }
 
    return 1;
}

// temp float experiment
int performFloat2DDCT(float* input, float* output) {
    
    float factor = sqrt(2. / N) * sqrt(2. / N);
    
    for (int v=0;v<N;v++) {
        for (int u=0;u<N;u++){
            float sum=0;
            float cv = v==0?sqrt(.5):1;
            float cu = u==0?sqrt(.5):1;
            for (int y=0;y<N;y++){
                for (int x=0;x<N;x++){
                    float syx = input[y*N+x];
                    float cos1 = cos((2*y+1)*v*M_PI/(2*N));
                    float cos2 = cos((2*x+1)*u*M_PI/(2*N));
                    sum += syx * cos1 * cos2;
                }
            }
            output[v*N+u] = factor * cv * cu * sum;
        }
    }
    
    
    return 1;
}
int performFloat2DIDCT(float* input, float* output) {
    
     float factor = sqrt(2. / N) * sqrt(2. / N);
    
    for (int y=0;y<N;y++) {
        for (int x=0;x<N;x++){
            float sum=0;
            for (int v=0;v<N;v++){
                for (int u=0;u<N;u++){
                    float cv = v==0?sqrt(.5):1;
                    float cu = u==0?sqrt(.5):1;
                    float fuv = input[v*N+u];
                    float cos1 = cos((2*y+1)*v*M_PI/(2*N));
                    float cos2 = cos((2*x+1)*u*M_PI/(2*N));
                    sum += cv * cu * fuv * cos1 * cos2;
                }
            }
            output[y*N+x] = factor * sum;
        }
    }
    
    
    return 1;
}


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
#pragma  mark 3-D transforms
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

#define shouldUseFastDCT 1

// inpur in char (0-255) output (-2048 - 2048)
int perform3DDCT(unsigned char* input, short int* output) {
    
    
    float calibratedIn[N*N*N];
    float calibratedOut[N*N*N];
    
    for (int k=0; k<N*N*N;k++) calibratedIn[k] = (input[k] - 128) / 128.0;
    
    
    performFloat3DDCT(calibratedIn, calibratedOut);
    
    
    for (int k=0;k<N*N*N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0);
        output[k] = val;
    }
    
    
    return 1;
}
int perform3DIDCT(short int* input, unsigned char* output) {
    
    float calibratedIn[N*N*N];
    float calibratedOut[N*N*N];
    
    for (int k=0; k<N*N*N;k++) calibratedIn[k] = input[k] / 128.0;

    performFloat3DIDCT(calibratedIn, calibratedOut);
    
    for (int k=0;k<N*N*N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0) + 128;
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        output[k] = val;
    }

    
    return 1;
}

int performFast3DDCT(unsigned char* input, short int* output,float* c) {
    
    
    float calibratedIn[N*N*N];
    float calibratedOut[N*N*N];
    
    for (int k=0; k<N*N*N;k++) calibratedIn[k] = (input[k] - 128) / 128.0;
    
    fast3DDct(calibratedIn, calibratedOut, c);
    
    for (int k=0;k<N*N*N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0);
        output[k] = val;
    }
    
    
    return 1;
}
int performFast3DIDCT(short int* input, unsigned char* output, float* c) {
    
    float calibratedIn[N*N*N];
    float calibratedOut[N*N*N];
    
    for (int k=0; k<N*N*N;k++) calibratedIn[k] = input[k] / 128.0;
    
    fast3DIDct(calibratedIn, calibratedOut, c);
    
    for (int k=0;k<N*N*N;k++) {
        int val = (int)lrintf(calibratedOut[k] * 128.0) + 128;
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        output[k] = val;
    }
    
    
    return 1;
}



int performFloat3DDCT(float* input, float* output) {
    
    float factor = sqrt(2. / N) * sqrt(2. / N)  * sqrt(2. / N);

    
    for (int w=0;w<N;w++) {
        for (int v=0;v<N;v++) {
            for (int u=0;u<N;u++){
                float sum=0;
                float cv = v==0?sqrt(.5):1;
                float cu = u==0?sqrt(.5):1;
                float cw = w==0?sqrt(.5):1;
                for (int y=0;y<N;y++){
                    for (int z=0;z<N;z++){
                        for (int x=0;x<N;x++){
                            float syxz = input[y*N*N+z*N+x];
                            float cos1 = cos((2*y+1)*w*M_PI/(2*N));
                            float cos2 = cos((2*z+1)*v*M_PI/(2*N));
                            float cos3 = cos((2*x+1)*u*M_PI/(2*N));
                            sum += syxz * cos1 * cos2 * cos3;
                        }
                    }
                }
                output[w*N*N+v*N+u] = factor * cw *cv * cu * sum;
            }
        }
    }
    
    
    return 1;
}
int performFloat3DIDCT(float* input, float* output) {
    
    float factor = sqrt(2. / N) * sqrt(2. / N) * sqrt(2. / N);
    
    for (int y=0;y<N;y++) {
        for (int z=0;z<N;z++) {
            for (int x=0;x<N;x++){
                float sum=0;
                for (int w=0;w<N;w++){
                    for (int v=0;v<N;v++){
                        for (int u=0;u<N;u++){
                            float cv = v==0?sqrt(.5):1;
                            float cu = u==0?sqrt(.5):1;
                            float cw = w==0?sqrt(.5):1;
                            float fwuv = input[w*N*N+v*N+u];
                            float cos1 = cos((2*y+1)*w*M_PI/(2*N));
                            float cos2 = cos((2*z+1)*v*M_PI/(2*N));
                            float cos3 = cos((2*x+1)*u*M_PI/(2*N));
                            sum += cv * cu * cw *fwuv * cos1 * cos2 * cos3;
                        }
                    }
                }
                output[y*N*N+z*N+x] = factor * sum;
            }
        }
    }
    
    return 1;
    
}



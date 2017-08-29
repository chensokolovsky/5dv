//
//  EncDecQuantization.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 6/15/17.
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

#include "EncDecQuantization.h"

/* Utils */

int generateChromaMatrix(unsigned char* matrix) {
    
    int i = 0;
    for (int h = 0; h < macroBlockDimension; h++) {
        for (int d = 0; d < macroBlockDimension; d++) {
            for (int w = 0; w < macroBlockDimension; w++) {
                
                int highest = h > d ? h : d;
                highest = highest > w ? highest : w;
                int val = 99;
                if (highest == 0) val = 17;
                else if (highest == 1) val = 18;
                else if (highest == 2) val = 24;
                else if (highest == 3) val = 47;
                else if (highest == 4) val = 70;
                matrix[i] = val;
                i++;
            }
        }
    }
    
    return 1;
}

int divideMacroBlockByQuantizationMatrix(short int* inputBuffer,  short int* outputBuffer, unsigned char* matrix) {
  
    // input is short int values 8x8x8
    // result is Schar values 8x8x8
    // matrix is Uchar 8x8x8
    int totalElements = macroBlockDimension * macroBlockDimension * macroBlockDimension;
    for (int k=0; k<totalElements;k++) {
        outputBuffer[k] = inputBuffer[k] / matrix[k];
    }
    
    return 1;
}

int multiplyMacroBlockByQuantizationMatrix(short int* inputBuffer, short int* outputBuffer, unsigned char* matrix) {
    
    // input is SChar values 8x8x8
    // result is short int values 8x8x8
    // matrix is Uchar 8x8x8
    int totalElements = macroBlockDimension * macroBlockDimension * macroBlockDimension;
    for (int k=0; k<totalElements;k++) {
        outputBuffer[k] = inputBuffer[k] * matrix[k];
    }
    
    return 1;
}

/*
  This method receives mega blocks of short it values after the DCT
    It grabs these values and depending on their location on the matrix, divides them by the quantization factor of that location.
 The output buffer is mega blocks of quantized values in unsigned char
 */
int performPhaseEncQuantization(unsigned char** inputBuffer, struct EncDecData tvData, unsigned char** outputBuffer,unsigned int* outputBufferLength) {
    
    int debugQuantEnc = 0;
    
    int elementsInMacroBlock = macroBlockDimension * macroBlockDimension *macroBlockDimension;
    unsigned char* chromaMatrix = (unsigned char*)calloc(elementsInMacroBlock,1);
    generateChromaMatrix(chromaMatrix);
    
    // Create a long enough buffer that will have enough room as if all the mega blocks have content
    int sizeOfMacroBlockInBytes = sizeof(short int) * macroBlockDimension * macroBlockDimension * macroBlockDimension;
    int sizeOfMegaBlockInBytes = 4 + ( 2 + 2 * megaBlockDimension * megaBlockDimension * megaBlockDimension) * sizeOfMacroBlockInBytes; // 4 is the header + ( 2 which are cb and cr + 2 which are luma and alpha times 8)
    int megaBlocksInWidth = tvData.tvsize.width / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInDeapth = tvData.tvsize.deapth / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInHeight = tvData.tvsize.height / (macroBlockDimension * megaBlockDimension);
    int totalMegaBlocksOnTV = megaBlocksInWidth * megaBlocksInDeapth * megaBlocksInHeight;
    unsigned int totalSizeOfTempWritingBufferInBytes = totalMegaBlocksOnTV * sizeOfMegaBlockInBytes;
    short int* tempOutputBuffer = (short int*)calloc(totalSizeOfTempWritingBufferInBytes,1);
    short int* writingPtr = tempOutputBuffer;
    
    short int* readintPtr = (short int*)*inputBuffer;
    

    for (int k=0; k<totalMegaBlocksOnTV; k++) {
        
       // printf(".");
        
        unsigned char* byteReadingPtr = (unsigned char*)readintPtr;
        
        // read the header.
        //If no alpha components, skip the next mega block and write an empty header to the output buffer
        if (byteReadingPtr[0] != 255) {
            printf("Error. expecting a mega block header first 111111111 here."); exit(-1);}
        if (byteReadingPtr[1] != 0) {printf("Error. expecting a mega block header second 00000000 here."); exit(-1);}
        if (byteReadingPtr[2] != 255) {printf("Error. expecting a mega block header third 111111111 here."); exit(-1);}
        if (byteReadingPtr[3] == 0) {
            if (debugQuantEnc) printf("all zeros mega block\n");
            // copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            //   printf("writing offset is %d\n",(int)(writingPtr - tempOutputBuffer));
            readintPtr += 2;
            writingPtr += 2;
        }
        else {
            // still, copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            readintPtr += 2;
            writingPtr += 2;
            
            // Now start reading macro blocks
            //Cb
            divideMacroBlockByQuantizationMatrix(readintPtr,writingPtr,chromaMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;
            
            
            
            //Cr
            divideMacroBlockByQuantizationMatrix(readintPtr, writingPtr, chromaMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;

            // 8 luma + 8 alpha
            for (int j=0; j<2 ; j++) {
                for (int m=0; m<megaBlockDimension * megaBlockDimension * megaBlockDimension; m++){
                    divideMacroBlockByQuantizationMatrix(readintPtr, writingPtr, chromaMatrix);
                    writingPtr += elementsInMacroBlock;
                    readintPtr += elementsInMacroBlock;

                }
            }
        }
    }
    
    
    unsigned int totalBytesWrote = (unsigned int)(writingPtr - tempOutputBuffer) * sizeof(short int);
    *outputBuffer = (unsigned char*)calloc(totalBytesWrote, 1);
    memcpy( *outputBuffer, tempOutputBuffer, totalBytesWrote);
    free (tempOutputBuffer);
    tempOutputBuffer = 0;
    free (chromaMatrix);
    chromaMatrix = 0;
    *outputBufferLength = totalBytesWrote;
        
    
    return 1;
}

int performPhaseDecQuantization(unsigned char** inputBuffer,struct EncDecData tvData, unsigned char** outputBuffer) {
    
    int debugQuantDec = 0;
    
    int elementsInMacroBlock = macroBlockDimension * macroBlockDimension *macroBlockDimension;
    unsigned char* chromaMatrix = (unsigned char*)calloc(elementsInMacroBlock,1);
    generateChromaMatrix(chromaMatrix);
    
    // Create a long enough buffer that will have enough room as if all the mega blocks have content
    int sizeOfMacroBlockInBytes = sizeof(short int) * macroBlockDimension * macroBlockDimension * macroBlockDimension;
    int sizeOfMegaBlockInBytes = 4 + ( 2 + 2 * megaBlockDimension * megaBlockDimension * megaBlockDimension) * sizeOfMacroBlockInBytes; // 4 is the header + ( 2 which are cb and cr + 2 which are luma and alpha times 8)
    int megaBlocksInWidth = tvData.tvsize.width / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInDeapth = tvData.tvsize.deapth / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInHeight = tvData.tvsize.height / (macroBlockDimension * megaBlockDimension);
    int totalMegaBlocksOnTV = megaBlocksInWidth * megaBlocksInDeapth * megaBlocksInHeight;
    unsigned int totalSizeOfTempWritingBufferInBytes = totalMegaBlocksOnTV * sizeOfMegaBlockInBytes;
    short int* tempOutputBuffer = (short int*)calloc(totalSizeOfTempWritingBufferInBytes,1);
    short int* writingPtr = tempOutputBuffer;
    
    short int* readintPtr = (short int*)*inputBuffer;
    
    
    for (int k=0; k<totalMegaBlocksOnTV; k++) {
        
       // printf(".");
        
        unsigned char* byteReadingPtr = (unsigned char*)readintPtr;
        
        // read the header.
        //If no alpha components, skip the next mega block and write an empty header to the output buffer
        if (byteReadingPtr[0] != 255) {
            printf("Error. expecting a mega block header first 111111111 here."); exit(-1);}
        if (byteReadingPtr[1] != 0) {printf("Error. expecting a mega block header second 00000000 here."); exit(-1);}
        if (byteReadingPtr[2] != 255) {printf("Error. expecting a mega block header third 111111111 here."); exit(-1);}
        if (byteReadingPtr[3] == 0) {
            if (debugQuantDec) printf("all zeros mega block\n");
            // copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            //   printf("writing offset is %d\n",(int)(writingPtr - tempOutputBuffer));
            readintPtr += 2;
            writingPtr += 2;
        }
        else {
            // still, copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            readintPtr += 2;
            writingPtr += 2;
            
            // Now start reading macro blocks
            //Cb
            multiplyMacroBlockByQuantizationMatrix(readintPtr,writingPtr,chromaMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;
            
            //Cr
            multiplyMacroBlockByQuantizationMatrix(readintPtr, writingPtr, chromaMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;
            
            // 8 luma + 8 alpha
            for (int j=0; j<2 ; j++) {
                
                for (int m=0; m<megaBlockDimension * megaBlockDimension * megaBlockDimension; m++){
                    multiplyMacroBlockByQuantizationMatrix(readintPtr, writingPtr, chromaMatrix);
                    writingPtr += elementsInMacroBlock;
                    readintPtr += elementsInMacroBlock;
                    
                }
            }
            
        }
        
    }
    
    unsigned int totalBytesWrote = (unsigned int)(writingPtr - tempOutputBuffer) * sizeof(short int);
    *outputBuffer = (unsigned char*)calloc(totalBytesWrote, 1);
    memcpy(*outputBuffer, tempOutputBuffer, totalBytesWrote);
    free (tempOutputBuffer);
    tempOutputBuffer = 0;
    free (chromaMatrix);
    chromaMatrix = 0;

    
    
    return 1;
}

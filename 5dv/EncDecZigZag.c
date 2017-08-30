//
//  EncDecZigZag.c
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


#import "EncDecZigZag.h"

//Utils
int zigZagBuffer(short int* input, short int* output, short int* matrix) {
    
    int size = macroBlockDimension * macroBlockDimension * macroBlockDimension;
    for (int k=0;k<size;k++) {
        output[k] = input[matrix[k]];
    }
 
    return 1;
}

int unZigZagBuffer(short int* input, short int* output, short int* matrix) {
    int size = macroBlockDimension * macroBlockDimension * macroBlockDimension;
    for (int k=0;k<size;k++) {
        output[matrix[k]] = input[k];
    }
    return 1;
}


// the input buffer is quantized mega blocks of short int
// the output buffer is the same, but re-ordered within each macro block (Cb,Cr,lume[8],alpha[8])
int performPhaseEncZigZag(unsigned char** inputBuffer, struct EncDecData tvData, unsigned char** outputBuffer,unsigned int* outputBufferLength) {
    
    int debugZigzagEnc = 0;
    
    int elementsInMacroBlock = macroBlockDimension * macroBlockDimension *macroBlockDimension;
    short int* zigZagMatrix = (short int*)calloc(elementsInMacroBlock,sizeof(short int));
    generateZigZagMatrix(zigZagMatrix);
    
    // Create a long enough buffer that will have enough room as if all the mega blocks have content
    int sizeOfMacroBlockInBytes = sizeof(short int) * macroBlockDimension * macroBlockDimension * macroBlockDimension;
    int sizeOfMegaBlockInBytes = 4 + ( 2 + 2 * megaBlockDimension * megaBlockDimension * megaBlockDimension) * sizeOfMacroBlockInBytes; // 4 is the header + ( 2 which are cb and cr + 2 which are luma and alpha times 8)
    int megaBlocksInWidth = tvData.tvsize.width / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInDeapth = tvData.tvsize.depth / (macroBlockDimension * megaBlockDimension);
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
            if (debugZigzagEnc) printf("all zeros mega block\n");
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
            zigZagBuffer(readintPtr,writingPtr,zigZagMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;
            
            
            
            //Cr
            zigZagBuffer(readintPtr, writingPtr, zigZagMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;
            
            // 8 luma + 8 alpha
            for (int j=0; j<2 ; j++) {
                for (int m=0; m<megaBlockDimension * megaBlockDimension * megaBlockDimension; m++){
                    zigZagBuffer(readintPtr, writingPtr, zigZagMatrix);
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
    free (zigZagMatrix);
    zigZagMatrix = 0;
    *outputBufferLength = totalBytesWrote;
    
    return 1;
}

int performPhaseDecZigZag(unsigned char** inputBuffer,struct EncDecData tvData, unsigned char** outputBuffer) {
    
    int debugZigzagtDec = 0;
    
    int elementsInMacroBlock = macroBlockDimension * macroBlockDimension *macroBlockDimension;
   short int* zigZagMatrix = (short int*)calloc(elementsInMacroBlock,sizeof(short int));
    generateZigZagMatrix(zigZagMatrix);
    
    // Create a long enough buffer that will have enough room as if all the mega blocks have content
    int sizeOfMacroBlockInBytes = sizeof(short int) * macroBlockDimension * macroBlockDimension * macroBlockDimension;
    int sizeOfMegaBlockInBytes = 4 + ( 2 + 2 * megaBlockDimension * megaBlockDimension * megaBlockDimension) * sizeOfMacroBlockInBytes; // 4 is the header + ( 2 which are cb and cr + 2 which are luma and alpha times 8)
    int megaBlocksInWidth = tvData.tvsize.width / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInDeapth = tvData.tvsize.depth / (macroBlockDimension * megaBlockDimension);
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
            if (debugZigzagtDec) printf("all zeros mega block\n");
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
            unZigZagBuffer(readintPtr,writingPtr,zigZagMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;
            
            //Cr
            unZigZagBuffer(readintPtr, writingPtr, zigZagMatrix);
            writingPtr += elementsInMacroBlock;
            readintPtr += elementsInMacroBlock;
            
            // 8 luma + 8 alpha
            for (int j=0; j<2 ; j++) {
                
                for (int m=0; m<megaBlockDimension * megaBlockDimension * megaBlockDimension; m++){
                    unZigZagBuffer(readintPtr, writingPtr, zigZagMatrix);
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
    free (zigZagMatrix);
    zigZagMatrix = 0;
    
    return 1;
}

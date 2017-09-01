//
//  EncDecDCT.cpp
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 6/13/17.
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

#import "EncDecDCT.h"


/* some utils */
void allocateDCTMacroBlock(struct DCTMacroBlock* block) {
    block->buffer = (short int*)calloc(macroBlockDimension * macroBlockDimension * macroBlockDimension * sizeof(short int),1);
}

void allocateDCTMegaBlock(struct DCTMegaBlock* megaBlock) {
    
    // allocate chroma
    allocateDCTMacroBlock(&(megaBlock->CbBlock));
    allocateDCTMacroBlock(&(megaBlock->CrBlock));
    
    
    // allocate alpha & luma arrays of blocks
    int totalAlphaMacroBlocks = megaBlockDimension * megaBlockDimension * megaBlockDimension;
    for (int k=0; k<totalAlphaMacroBlocks; k++) {
        allocateDCTMacroBlock(&(megaBlock->alphaBlocks[k]));
        allocateDCTMacroBlock(&(megaBlock->lumaMacroBlocks[k]));
    }
    
    
}

/*
 
 The input buffer is in form of mega blocks. Each mega block starts with a header, stating if all alphas are zeros or not.
 Then there are 2 + 8 + 8 macro blocks of data. First two are Cb and Cr, then 8 luma then 8 alpha
 Each one of these will run through DCT.
 
 The output format is the same (2+8+8) only with short int values instead of U-char. This makes the output buffer twice as long than the input buffer, But, after quantization takes place, we will go back to U-char values
 
 
 
 */
int performPhaseDCT(unsigned char** inputBuffer, struct EncDecData tvData, unsigned char** outputBuffer,unsigned int* outputBufferLength) {
    
    int debugDCTEnc = 0;
 
    // Create a long enough buffer that will have enough room as if all the mega blocks have content
    int sizeOfDCTMacroBlockInBytes = sizeof(short int) * macroBlockDimension * macroBlockDimension * macroBlockDimension;
    int sizeOfDCTMegaBlockInBytes = 4 + ( 2 + 2 * megaBlockDimension * megaBlockDimension * megaBlockDimension) * sizeOfDCTMacroBlockInBytes; // 4 is the header + ( 2 which are cb and cr + 2 which are luma and alpha times 8)
    int megaBlocksInWidth = tvData.tvsize.width / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInDeapth = tvData.tvsize.depth / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInHeight = tvData.tvsize.height / (macroBlockDimension * megaBlockDimension);
    int totalMegaBlocksOnTV = megaBlocksInWidth * megaBlocksInDeapth * megaBlocksInHeight;
    unsigned int totalSizeOfTempWritingBufferInBytes = totalMegaBlocksOnTV * sizeOfDCTMegaBlockInBytes;
    short int* tempOutputBuffer = (short int*)calloc(totalSizeOfTempWritingBufferInBytes,1);
    short int* writingPtr = tempOutputBuffer;
    
    float c[N+2];
    initFastDCT(c);
    
    // We have an allocated temp writing buffer.
    // Start reading the input buffer mega blocks
    unsigned char* readintPtr = *inputBuffer;
 
  
    int macroBlockSize = macroBlockDimension * macroBlockDimension * macroBlockDimension;
    
    for (int k=0; k<totalMegaBlocksOnTV; k++) {
    //    printf(".");
        
        
        // read the header.
        //If no alpha components, skip the next mega block and write an empty header to the output buffer
        if (readintPtr[0] != 255) {
            printf("Error. expecting a mega block header first 111111111 here."); exit(-1);}
        if (readintPtr[1] != 0) {
            printf("Error. expecting a mega block header second 00000000 here."); exit(-1);
        }
        if (readintPtr[2] != 255) {printf("Error. expecting a mega block header third 111111111 here."); exit(-1);}
        if (readintPtr[3] == 0) {
            if (debugDCTEnc) printf("all zeros mega block\n");
            // copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            readintPtr += 4;
            writingPtr += 2;
        }
        else {
            // still, copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            readintPtr += 4;
            writingPtr += 2;
            
            // Start reading Cb and Cr from the input buffer and run them through DCT. Store the result in the new DCTMegaBlock
            
            // Perform DCT on the Cb
            performFast3DDCT(readintPtr, writingPtr, c);
            readintPtr += macroBlockSize;
            writingPtr += macroBlockSize;

            
            // DCT on the Cr
            performFast3DDCT(readintPtr, writingPtr, c);
            readintPtr += macroBlockSize;
            writingPtr += macroBlockSize;

            
            // DCT the luma
            for (int l=0;l<megaBlockDimension * megaBlockDimension * megaBlockDimension;l++) {
                performFast3DDCT(readintPtr, writingPtr, c);
                readintPtr += macroBlockSize;
                writingPtr += macroBlockSize;

            }

            // DCT the alpha
            for (int l=0;l<megaBlockDimension * megaBlockDimension * megaBlockDimension;l++) {
                performFast3DDCT(readintPtr,writingPtr, c);
                readintPtr += macroBlockSize;
                writingPtr += macroBlockSize;

            }
        }
    }
   
  //  printf("\n");

    // Write the tempOutBuffer to the real output buffer and set the output length for the caller to use
    unsigned int totalBytesWrote = (unsigned int)((sizeof(short int))*(writingPtr - tempOutputBuffer));
    *outputBuffer = (unsigned char*)calloc(totalBytesWrote,1);
    memcpy(*outputBuffer, tempOutputBuffer, totalBytesWrote);
    *outputBufferLength = totalBytesWrote;

    free(tempOutputBuffer);
    
    
    return 1;
}

/*
 The input buffer is DCTMegaBlocks with small headers. The mega blocks are in short int values of coeeficients.
 The output should be MegaBlocks of unsigned char with the actual values after running IDCT
 
 The input buffer is twice as long as the output buffer because of the values (short int vs. unsigned char)
 
 This will be called after multipying the values by the coefficients of the quantization tables.
 
*/
int performPhaseIDCT(unsigned char** inputBuffer,struct EncDecData tvData, unsigned char** outputBuffer) {
    
    int debugDCTEnc = 0;
    
    // Create a long enough buffer that will have enough room as if all the mega blocks have content
    int sizeOfDCTMacroBlockInBytes = sizeof(unsigned char) * macroBlockDimension * macroBlockDimension * macroBlockDimension;
    int sizeOfDCTMegaBlockInBytes = 4 + ( 2 + 2 * megaBlockDimension * megaBlockDimension * megaBlockDimension) * sizeOfDCTMacroBlockInBytes; // 4 is the header + ( 2 which are cb and cr + 2 which are luma and alpha times 8)
    int megaBlocksInWidth = tvData.tvsize.width / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInDeapth = tvData.tvsize.depth / (macroBlockDimension * megaBlockDimension);
    int megaBlocksInHeight = tvData.tvsize.height / (macroBlockDimension * megaBlockDimension);
    int totalMegaBlocksOnTV = megaBlocksInWidth * megaBlocksInDeapth * megaBlocksInHeight;
    unsigned int totalSizeOfTempWritingBufferInBytes = totalMegaBlocksOnTV * sizeOfDCTMegaBlockInBytes;
    unsigned char* tempOutputBuffer = (unsigned char*)calloc(totalSizeOfTempWritingBufferInBytes,1);
    unsigned char* writingPtr = tempOutputBuffer;
    
    float c[N+2];
    initFastDCT(c);
    
    // We have an allocated temp writing buffer.
    // Start reading the input buffer mega blocks
    short int* readintPtr = (short int*)*inputBuffer;
    
    
    int macroBlockSize = macroBlockDimension * macroBlockDimension * macroBlockDimension;
    
    for (int k=0; k<totalMegaBlocksOnTV; k++) {
        
      //  printf(".");

        
        unsigned char* byteReadingPtr = (unsigned char*)readintPtr;
        
        // read the header.
        //If no alpha components, skip the next mega block and write an empty header to the output buffer
        if (byteReadingPtr[0] != 255) {
            printf("Error. expecting a mega block header first 111111111 here."); exit(-1);}
        if (byteReadingPtr[1] != 0) {printf("Error. expecting a mega block header second 00000000 here."); exit(-1);}
        if (byteReadingPtr[2] != 255) {printf("Error. expecting a mega block header third 111111111 here."); exit(-1);}
        if (byteReadingPtr[3] == 0) {
            if (debugDCTEnc) printf("all zeros mega block\n");
            // copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            
         //   printf("writing offset is %d\n",(int)(writingPtr - tempOutputBuffer));

            readintPtr += 2;
            writingPtr += 4;
        }
        else {
            // still, copy the header and advance the pointers
            memcpy(writingPtr, readintPtr,4);
            readintPtr += 2;
            writingPtr += 4;
            
          //  printf("writing offset is %d\n",(int)(writingPtr - tempOutputBuffer));

            // Start reading Cb and Cr from the input buffer and run them through DCT. Store the result in the new DCTMegaBlock
            
            // Perform DCT on the Cb
            performFast3DIDCT(readintPtr, writingPtr, c);
            readintPtr += macroBlockSize;
            writingPtr += macroBlockSize;
            
            // DCT on the Cr
            performFast3DIDCT(readintPtr, writingPtr, c);
            readintPtr += macroBlockSize;
            writingPtr += macroBlockSize;

            
            // DCT the luma
            for (int l=0;l<megaBlockDimension * megaBlockDimension * megaBlockDimension;l++) {
                performFast3DIDCT(readintPtr, writingPtr, c);
                readintPtr += macroBlockSize;
                writingPtr += macroBlockSize;

            }
            
            // DCT the alpha
            for (int l=0;l<megaBlockDimension * megaBlockDimension * megaBlockDimension;l++) {
                performFast3DIDCT(readintPtr, writingPtr, c);
                readintPtr += macroBlockSize;
                writingPtr += macroBlockSize;

            }
        }
    }
    
    
   // printf("\n");

    // Write the tempOutBuffer to the real output buffer and set the output length for the caller to use
    unsigned int totalBytesWrote = (unsigned int)(writingPtr - tempOutputBuffer);
    *outputBuffer = (unsigned char*)calloc(totalBytesWrote,1);
    memcpy(*outputBuffer, tempOutputBuffer, totalBytesWrote);

    free(tempOutputBuffer);
    
    
    return 1;
    
    return 1;
}

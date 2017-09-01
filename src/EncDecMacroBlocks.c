//
//  EncDecMacroBlocks.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 5/25/17.
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

#import "EncDecMacroBlocks.h"


void allocateMacroBlock(struct macroBlock* block) {
    block->buffer = (unsigned char*)calloc(macroBlockDimension * macroBlockDimension * macroBlockDimension * sizeof(unsigned char),1);
}

void allocateMegaBlock(struct megaBlock* megaBlock) {
    
    // allocate chroma
    allocateMacroBlock(&(megaBlock->CbBlock));
    allocateMacroBlock(&(megaBlock->CrBlock));
    
    
    // allocate alpha & luma arrays of blocks
    int totalAlphaMacroBlocks = megaBlockDimension * megaBlockDimension * megaBlockDimension;
    for (int k=0; k<totalAlphaMacroBlocks; k++) {
        allocateMacroBlock(&(megaBlock->alphaBlocks[k]));
        allocateMacroBlock(&(megaBlock->lumaMacroBlocks[k]));
    }
    
    
}

void freeMegaBlock(struct megaBlock* megaBlock) {
    free(megaBlock->CbBlock.buffer);
    free(megaBlock->CrBlock.buffer);
    for (int k=0; k<megaBlockDimension * megaBlockDimension * megaBlockDimension; k++) {
        free(megaBlock->lumaMacroBlocks[k].buffer);
        free(megaBlock->alphaBlocks[k].buffer);
    }
}


unsigned int getIndexOfLocationInTVSize(struct zLocation location, struct TVSize tvSize) {
    
    unsigned answer = location.y * tvSize.width * tvSize.depth;
    answer += location.z * tvSize.width;
    answer += location.x;
    return answer;
}

struct zLocation getZLocationOfIndexInTVSize(int index, struct TVSize tvSize) {
    
    struct zLocation answer;
    answer.y = index / (tvSize.width * tvSize.depth);
    int indexMinusHeight = index - answer.y * (tvSize.width * tvSize.depth);
    answer.z = indexMinusHeight / tvSize.depth;
    int indexMinusDeapth = indexMinusHeight - answer.z * tvSize.depth;
    answer.x = indexMinusDeapth;

    return answer;
}

struct zLocation makeZLocation(int x, int z, int y) {
    struct zLocation answer;
    answer.x = x; // w
    answer.z = z; // d
    answer.y = y; // h

    return answer;
}

struct TVSize makeTVSize(int width, int depth, int height) {
    struct TVSize answer;
    answer.width = width;
    answer.depth = depth;
    answer.height = height;
    return answer;
}



// This grabs a buffer of chroma blocks and converts it to mega and macro blocks.
// This length of the result buffer is unknown, until we are done with the encoding.
// This is because if the entire mega block is alpha zero, there is no point in compressing it
// There is no way of knowing until we create the blocks and itterate
int performPhaseEncodeMacroBlocks(unsigned char** inBuffer, struct EncDecData tvData, unsigned char** outBuffer, unsigned int* outputBufferLength) {
    
    
    // best thing to do is to create a buffer as if nothing is zero. we can later realloc the outbuffer to a smaller one
    int tvWidth = tvData.tvsize.width;
    int tvDeapth = tvData.tvsize.depth;
    int tvHeight = tvData.tvsize.height;
    int chromaBlocksInWidth = tvWidth / chromaBlockWidth;
    int chromaBlocksInDeapth = tvDeapth / chromaBlockDeapth;
    int chromaBlocksInHeight = tvHeight / chromaBlockHeight;
    
    int megaBlocksInWidth = chromaBlocksInWidth / macroBlockDimension;
    int megaBlocksInDeapth = chromaBlocksInDeapth / macroBlockDimension;
    int megaBlocksInPlate = megaBlocksInWidth * megaBlocksInDeapth;
    int megaBlocksPlatesInHeight = chromaBlocksInHeight / macroBlockDimension;
    int totalMegaBlocksInTV = megaBlocksPlatesInHeight * megaBlocksInPlate;
  
    int totalAlphaBlocksInAMegaBlock = megaBlockDimension * megaBlockDimension * megaBlockDimension;
    int totalLumaBlocksInAMegaBlock = megaBlockDimension * megaBlockDimension * megaBlockDimension;
    int totalCbBlocksInAMegaBlock = 1;
    int totalCrBlocksInAMegaBlock = 1;
    int totalMacroBlocksInAMegaBlock = totalAlphaBlocksInAMegaBlock + totalLumaBlocksInAMegaBlock + totalCbBlocksInAMegaBlock + totalCrBlocksInAMegaBlock;
    int sizeOfMacroBlockInBytes = macroBlockDimension * macroBlockDimension * macroBlockDimension * sizeof (unsigned char);
    int sizeOfMegaBlockSmallHeaderInBytes = 4; // this will be 11111111 00000000 11111111 0000000x where x is 0 or 1. if x==0, mega block is empty
    int sizeOfMegaBlockInBytes = sizeOfMacroBlockInBytes * totalMacroBlocksInAMegaBlock + sizeOfMegaBlockSmallHeaderInBytes;
    unsigned int sizeOfFullBuffer = sizeOfMegaBlockInBytes * totalMegaBlocksInTV;
    
    
    // allocate the memory for writing the mega blocks
    unsigned char* writingBuffer = (unsigned char*)calloc(sizeOfFullBuffer, 1);
    unsigned char* currentWritingPtr = writingBuffer;
    unsigned int totalBytesWroteToOutBuffer = 0;
    
    // now we start to fill in mega blocks and macro blocks with values from the input buffer
    // the input buffer is Cb,Cr,y0,a0,y1,a1,y2,a3,....,y7,a7 and off to the next group
    int sizeOfYCbCrGroupInBytes = 2 + 8 + 8;
    
    // create and allocate a new mega block and macro blocks
    struct megaBlock newMegaBlock;
    allocateMegaBlock(&newMegaBlock);
    
    int megaBlocksHeightInChromaBlocks = (megaBlockDimension * macroBlockDimension / chromaBlockHeight);
    int megaBlocksDeapthInChromaBlocks = (megaBlockDimension * macroBlockDimension / chromaBlockDeapth);
    int megaBlocksWidthInChromaBlocks = (megaBlockDimension * macroBlockDimension / chromaBlockWidth);
    
    for (int megaBlockHeightIndex = 0; megaBlockHeightIndex < megaBlocksPlatesInHeight; megaBlockHeightIndex++) {
        for (int megaBlockDeapthIndex = 0; megaBlockDeapthIndex < megaBlocksInDeapth; megaBlockDeapthIndex++) {
            for (int megaBlockWidthIndex = 0; megaBlockWidthIndex < megaBlocksInWidth; megaBlockWidthIndex++) {
                
                
            
                // We can start writing Cb and Cr
                // we need to find the chroma group index accoring to the current mega block and the location within the mega block
               
                
                for (int innerHeightInMegaBlock = 0; innerHeightInMegaBlock < megaBlocksHeightInChromaBlocks; innerHeightInMegaBlock++) {
                    for (int innerDeapthInMegaBlock = 0; innerDeapthInMegaBlock < megaBlocksDeapthInChromaBlocks; innerDeapthInMegaBlock++) {
                        for (int innerWidthInMegaBlock = 0; innerWidthInMegaBlock < megaBlocksWidthInChromaBlocks; innerWidthInMegaBlock++) {
                            
                           // this is the index of the mega block buffer  we will write into
                            int innerIndexOfThisChromaGroupInTheCurrentMegaBlock = getIndexOfLocationInTVSize(makeZLocation(innerWidthInMegaBlock, innerDeapthInMegaBlock, innerHeightInMegaBlock), makeTVSize(megaBlocksWidthInChromaBlocks, megaBlocksDeapthInChromaBlocks, megaBlocksHeightInChromaBlocks));
                            
                            // we need to find the index of the current inner choroma Block in the input array
                            // We start by the height
                            int heightOfTheCurrentChromaBlockWithinThisMegaBlockInTheInputChromaBlocks = megaBlockHeightIndex * megaBlocksHeightInChromaBlocks + innerHeightInMegaBlock;
                            
                            int totalChromaBlocksInHeightSoFar = heightOfTheCurrentChromaBlockWithinThisMegaBlockInTheInputChromaBlocks * chromaBlocksInWidth * chromaBlocksInDeapth;
                            
                            // Now to the depth:
                            
                            int depthOfTheCurrentChomaBlockWithinThisMegaBlockInTheInputChromaBlock = megaBlockDeapthIndex * megaBlocksDeapthInChromaBlocks + innerDeapthInMegaBlock;
                            
                            int totalChromaBlocksInDeapthSoFar = depthOfTheCurrentChomaBlockWithinThisMegaBlockInTheInputChromaBlock * chromaBlocksInWidth;
                            
                            // now the width
                            
                            int widthOfTheCurrentChromaBlockWithinThisMegaBlockInTheInputBlock = megaBlockWidthIndex * megaBlocksWidthInChromaBlocks + innerWidthInMegaBlock;
                            
                            // sum these up
                            int indexOfTheCurentChromaBlockWithinThisMegaBlockInTheInputBuffer = totalChromaBlocksInHeightSoFar + totalChromaBlocksInDeapthSoFar + widthOfTheCurrentChromaBlockWithinThisMegaBlockInTheInputBlock;
                
                            // now make sure to read the real value of cr and cb
                            int indexOfThisCb = indexOfTheCurentChromaBlockWithinThisMegaBlockInTheInputBuffer * sizeOfYCbCrGroupInBytes;
                            int indexOfThisCr = indexOfThisCb+1;
                            
                            newMegaBlock.CbBlock.buffer[innerIndexOfThisChromaGroupInTheCurrentMegaBlock] = ((unsigned char*)*inBuffer)[indexOfThisCb];
                            newMegaBlock.CrBlock.buffer[innerIndexOfThisChromaGroupInTheCurrentMegaBlock] = ((unsigned char*)*inBuffer)[indexOfThisCr];
                            
                        }
                    }
                }
                
                // We are now done filling the Cb and Cr of the mega block.
                //
                // Now we fill the Luma and Alpha mega blocks array.
                // Each mega block has 8 luma macro blocks and 8 alpha macro blocks
                //
                //
                for (int macroBlockHeightIndexInMegaBlock = 0; macroBlockHeightIndexInMegaBlock < megaBlockDimension; macroBlockHeightIndexInMegaBlock++) {
                    for (int macroBlockDeapthIndexInMegaBlock = 0; macroBlockDeapthIndexInMegaBlock < megaBlockDimension; macroBlockDeapthIndexInMegaBlock++) {
                        for (int macroBlockWidthIndexInMegaBlock = 0; macroBlockWidthIndexInMegaBlock < megaBlockDimension; macroBlockWidthIndexInMegaBlock++) {
                            
                            // set these pointers for later for writing purposes
                            int indexOfTheMacroBlockInTheMegaBlock = getIndexOfLocationInTVSize(makeZLocation(macroBlockWidthIndexInMegaBlock, macroBlockDeapthIndexInMegaBlock, macroBlockHeightIndexInMegaBlock), makeTVSize(megaBlockDimension, megaBlockDimension, megaBlockDimension));
                            unsigned char* currentMacroBlockWritingLocationOfLume = newMegaBlock.lumaMacroBlocks[indexOfTheMacroBlockInTheMegaBlock].buffer;
                            unsigned char* currentMacroBlockWritingLocationOfAlpha = newMegaBlock.alphaBlocks[indexOfTheMacroBlockInTheMegaBlock].buffer;
                            
                            
                            // In this macro block, we want to get information from the input buffer, that has chromaBlocks, and set them up two long buffers of the mega block.
                            // The mega block buffers are alphaBlocks (which are 8 macro blocks of alpha values) and lumaMacroBlocks (which are 8 macro blocks of luma)
                            //
                            //
                            // we now itterate each one of the macro blocks
                            for (int innerChromaBlockHeightIndex = 0; innerChromaBlockHeightIndex < macroBlockDimension / megaBlockDimension; innerChromaBlockHeightIndex ++) {
                                for (int innerChromaBlockDeapthIndex = 0; innerChromaBlockDeapthIndex < macroBlockDimension / megaBlockDimension; innerChromaBlockDeapthIndex ++) {
                                    for (int innerChromaBlockWidthIndex = 0; innerChromaBlockWidthIndex < macroBlockDimension / megaBlockDimension; innerChromaBlockWidthIndex ++) {
                                     
                                        // The steps are to calculate the index of the chromaBlock in the input array
                                        // and then, place it in the current allocated mega block, both alpha and luma
                                        //
                                        //
                                        
                                        // we first calculate the height of this chroma block in the input array
                                        // the height is the height of the mega block * the total chroma blocks in the mega block's plane
                                        // plus the heigt of the inner macro block * the total chroma blocks in a macro blocks plane
                                        // plus the height of the inner index of the chroma block * the total chroma bloakc in a chroma blocks plane
                                        int totalChromaBlocksInAMegaBlocksPlane = megaBlocksHeightInChromaBlocks * chromaBlocksInWidth * chromaBlocksInDeapth;
                                        int indexOfChromaBlocksAfterHeightOfMegaBlock = totalChromaBlocksInAMegaBlocksPlane * megaBlockHeightIndex;
                                        
                                        int totalChromaBlocksInAMacroBlocksPlane = (macroBlockDimension / chromaBlockHeight) * chromaBlocksInWidth * chromaBlocksInDeapth;
                                        int indexOfChromaBlocksOfHeightOfMacroBlock = totalChromaBlocksInAMacroBlocksPlane * macroBlockHeightIndexInMegaBlock;
                                        
                                        int totalChromaBlocksInAChromaBlocksPlane = chromaBlocksInDeapth * chromaBlocksInWidth;
                                        int indexOfChromaBlocksOfHeightOfInnerChromaBlockInMacroBlock = totalChromaBlocksInAChromaBlocksPlane * innerChromaBlockHeightIndex;
                                        
                                        // sum these up to get the total height
                                        int totalIndexOfHeightOfChromaBlocksOfThisInnerChromaBlockInInputBuffer =
                                        indexOfChromaBlocksAfterHeightOfMegaBlock + indexOfChromaBlocksOfHeightOfMacroBlock + indexOfChromaBlocksOfHeightOfInnerChromaBlockInMacroBlock;
                                        
                                        /////////
                                        // Now, the depth
                                        // the depth is the current mega block depth index * how many width of chroma bloak exist in a mega block * how many chorma blocks are in a row
                                        // plus the current macro block depth index * how many chroma blocks are in the width of a macro block
                                        // plus the current mega block inner index * how many chroma blocks are in a chrome block row
                                        int totalChromaBlocksInAMegaBlockWidth /*don't get confused by the word width. it is equivalent to the plane when calculating for depth*/ = (megaBlockDimension * macroBlockDimension / chromaBlockWidth )* chromaBlocksInWidth;
                                        int indexOfChromaBlocksOfTheMegaBlockDeapth = totalChromaBlocksInAMegaBlockWidth * megaBlockDeapthIndex;
                                        
                                        int totalChromaBlocksInAMacroBlockWidth = (macroBlockDimension / chromaBlockWidth) * chromaBlocksInWidth;
                                        int indexOfChromaBlocksOfTheMacroBlockDeapth = totalChromaBlocksInAMacroBlockWidth * macroBlockDeapthIndexInMegaBlock;
                                        
                                        int totalChromaBlocksInAChromaBlocksWidth = chromaBlocksInWidth;
                                        int indexOfChromaBlocksOfChromaBlocksDeapth = totalChromaBlocksInAChromaBlocksWidth * innerChromaBlockDeapthIndex;
                                        
                                        // sum these up to get total depth
                                        int totalIndexOfDeapthOfChromaBlocksOfThisInnerChromaBlockInInputBuffer =
                                        indexOfChromaBlocksOfTheMegaBlockDeapth + indexOfChromaBlocksOfTheMacroBlockDeapth + indexOfChromaBlocksOfChromaBlocksDeapth;
                                        
                                        ///////////
                                        /// Now the width
                                        // we need the total chroma blocks in a mega block width
                                        // plus the total chroma blocks in a macro block width
                                        // plus the inner width
                                        int indexOfChromaBlocksInMegaBlockWidth = megaBlockWidthIndex * macroBlockDimension;
                                        int indexOfChromaBlocksInMacroBlockWidth = macroBlockWidthIndexInMegaBlock * (macroBlockDimension / chromaBlockWidth);
                                        int totalIndexOfWidthOfChromaBlocksOfThisInnerChromaBlockInInputBuffer = indexOfChromaBlocksInMegaBlockWidth + indexOfChromaBlocksInMacroBlockWidth + innerChromaBlockWidthIndex;
                                        
                                        
                                        ////// Sum up all 3 to get the index in the input buffer
                                        int indexOfTheCurrentChromaBlockInTheInputBuffer = totalIndexOfHeightOfChromaBlocksOfThisInnerChromaBlockInInputBuffer + totalIndexOfDeapthOfChromaBlocksOfThisInnerChromaBlockInInputBuffer +totalIndexOfWidthOfChromaBlocksOfThisInnerChromaBlockInInputBuffer;
                                        
                                        //// Now we need to calculate where to write to in the macro block buffers
                                        // we have 8 buffers (mega blocks dimension ^ 3) to write to
                                        // each buffer is 8x8x8 (macro blocks dimension)
                                        // we grab the pixels from the inner chroma group
                                        // the chroma group is Cb,Cr,y0,a0,y1,a1,y2,a3,....,y7,a7
                                        // we already wrote the Cb and Cr
                                        // now we grab these 8 values, and line them up in the 8x8x8 array in such way that they are ready for the DCT
                                        // for exampe, if we have chorma groups 0,1,2,3 which is the first row of the macro block, we grab 0[y0 and y1] then 1[y0 y1] then 2[y0 y1] etc..
                                        // this way we get the "original" luma and alpha values lined up for the transform
                                        // the lume buffer will look like this 0[y0],0[y1],1[y0],....3[y1],0[y2],0[y3],... 3[y3],4[y0],4[y1],...7[y2],4[y2],4[y3],....7[y3], etc...
                                        // Same for alpha
                                        
                                     //   for (int k=0; k< chromaBlockWidth * chromaBlocksInDeapth * chromaBlocksInHeight; k++) {
                                        for (int k=0; k< chromaBlockWidth * chromaBlockDeapth * chromaBlockHeight; k++) {
                                            struct zLocation locationOfPixelInChromaBlock = getZLocationOfIndexInTVSize(k, makeTVSize(chromaBlockWidth, chromaBlockDeapth, chromaBlockHeight));
                                            
                                            int totalHeight = innerChromaBlockHeightIndex * chromaBlockHeight + locationOfPixelInChromaBlock.y;
                                            int indexOfHeight = totalHeight * macroBlockDimension * macroBlockDimension;
                                            
                                            int totalDeapth = innerChromaBlockDeapthIndex * chromaBlockDeapth + locationOfPixelInChromaBlock.z;
                                            int indexOfDeapth = totalDeapth * macroBlockDimension;
                                            
                                            int indexOfWidth = innerChromaBlockWidthIndex * chromaBlockWidth + locationOfPixelInChromaBlock.x;
                                            
                                            int indexOfThisPixelInTheMacroBlockBuffer = indexOfHeight + indexOfDeapth + indexOfWidth;
                                            
                                            // now write the value
                                            unsigned char* writingLocationLuma = currentMacroBlockWritingLocationOfLume + indexOfThisPixelInTheMacroBlockBuffer;
                                            unsigned char* writingLocationAlpha = currentMacroBlockWritingLocationOfAlpha + indexOfThisPixelInTheMacroBlockBuffer;
                                            
                                            int indexOfCurrentLumaInChromaGroup = 2 + k*2;
                                            int indexOfCurrentAlphaInChromaGroup = 3 + k*2;
                                            
                                            unsigned int startingByteOfChromaGroup = sizeOfYCbCrGroupInBytes * indexOfTheCurrentChromaBlockInTheInputBuffer;
                                            unsigned int lumaByteInInputBuffer = indexOfCurrentLumaInChromaGroup + startingByteOfChromaGroup;
                                            unsigned int alphaBtyeInInputBuffer = indexOfCurrentAlphaInChromaGroup + startingByteOfChromaGroup;
                                            
                                            *writingLocationLuma = (*inBuffer)[lumaByteInInputBuffer];
                                            *writingLocationAlpha = (*inBuffer)[alphaBtyeInInputBuffer];
                                            
                                        }
                                        
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Now all luma and alpha are also full
                // We can start placing mega blocks on a final buffer.
                // For each mega block, we check if all alpha values are zero.
                // We set the result to a small header. If there is one non zero, we then add the mega block
                // If not, we skip the mega block to the next header
                
                unsigned char allZeros = 1;
                for (int k=0; k<megaBlockDimension * megaBlockDimension * megaBlockDimension; k++) {
                    unsigned char* alphaBuffer = newMegaBlock.alphaBlocks[k].buffer;
                    for (int l=0; l<macroBlockDimension * macroBlockDimension * macroBlockDimension; l++) {
                        if (alphaBuffer[l] > 0) {
                            allZeros = 0;
                            break;
                        }
                    }
                    if (allZeros == 0) break;
                }
                
                currentWritingPtr[0] = 255;
                currentWritingPtr[1] = 0;
                currentWritingPtr[2] = 255;
                currentWritingPtr[3] = allZeros ? 0 : 1;
                
                currentWritingPtr += 4;
                totalBytesWroteToOutBuffer += 4;
                
                
                if (!allZeros) {
                // if all zeros, we can skip to the next block. If not, write the buffers in order
                // Cb Cr Luma alpha
                    
                    
                    int macroBlockSize = macroBlockDimension * macroBlockDimension * macroBlockDimension;
                    // Cb
                    memcpy(currentWritingPtr, newMegaBlock.CbBlock.buffer, macroBlockSize);
                    totalBytesWroteToOutBuffer += macroBlockSize;
                    currentWritingPtr += macroBlockSize;
                    // Cr
                    memcpy(currentWritingPtr, newMegaBlock.CrBlock.buffer, macroBlockSize);
                    totalBytesWroteToOutBuffer += macroBlockSize;
                    currentWritingPtr += macroBlockSize;
                    int macroBlocksInAMegaBlock = megaBlockDimension * megaBlockDimension * megaBlockDimension;
                    // Luma
                    for (int k =0; k<macroBlocksInAMegaBlock; k++) {
                        memcpy(currentWritingPtr, newMegaBlock.lumaMacroBlocks[k].buffer, macroBlockSize);
                        totalBytesWroteToOutBuffer += macroBlockSize;
                        currentWritingPtr += macroBlockSize;
                    }
                    // Alpha
                    for (int k =0; k<macroBlocksInAMegaBlock; k++) {
                        memcpy(currentWritingPtr, newMegaBlock.alphaBlocks[k].buffer, macroBlockSize);
                        totalBytesWroteToOutBuffer += macroBlockSize;
                        currentWritingPtr += macroBlockSize;
                    }
                }
            }
        }
    }
    
    *outBuffer = (unsigned char*)calloc(totalBytesWroteToOutBuffer, 1);
    memcpy(*outBuffer, writingBuffer, totalBytesWroteToOutBuffer);
    
    freeMegaBlock(&newMegaBlock);
    free(writingBuffer);
    writingBuffer = 0;
    
    *outputBufferLength = totalBytesWroteToOutBuffer;
    
    return 1;
}

#define debugMBDec 0

// The inverse method. takes macro blocks input and sets up chroma blocks.
// It can also read a zero macro blocks and fill it up with zeros.
// The results are chroma blocks of 2x2x2
int performPhaseDecodeMacroBlocks(unsigned char** inBuffer, struct EncDecData tvData, unsigned char** outBuffer) {
    
    
    
    // we start reading the input buffer, which is macro blocks with headers. The output buffer should be an array of chroma blocks
    
    
    int megaBlocksInWidth = tvData.tvsize.width  / (chromaBlockWidth * macroBlockDimension);
    int megaBlocksInDeapth = tvData.tvsize.depth / (chromaBlockDeapth * macroBlockDimension);
    int megaBlocksInHeight = tvData.tvsize.height / (chromaBlockHeight * macroBlockDimension);
    int totalMegaBlocksInTV = megaBlocksInWidth * megaBlocksInDeapth * megaBlocksInHeight;
    
    
    // allocate the output buffer
    int chromaBlockSizeInBytes = 2 + 2 * (chromaBlockHeight * chromaBlockDeapth * chromaBlockWidth); // cb + cr + 8 lumas 8 aplhas
    int chromaBlocksInWidth = tvData.tvsize.width / chromaBlockWidth;
    int chromaBlocksInDeapth = tvData.tvsize.depth / chromaBlockDeapth;
    int chromaBlocksInHeight = tvData.tvsize.height / chromaBlockHeight;
    int totalChromaBlocksInTV = chromaBlocksInWidth * chromaBlocksInDeapth * chromaBlocksInHeight;
    unsigned int outputBufferSizeInBytes = totalChromaBlocksInTV * chromaBlockSizeInBytes;
    *outBuffer = (unsigned char*)calloc(outputBufferSizeInBytes,1);
    
    if (debugMBDec) printf("There are %d mega blocks in tv\n",totalMegaBlocksInTV);
    
    unsigned char* currentInputReadingPtr = *inBuffer;

    
    for (int megaBlockHeightIndex = 0; megaBlockHeightIndex < megaBlocksInHeight; megaBlockHeightIndex++) {
        for (int megaBlockDeapthIndex = 0; megaBlockDeapthIndex < megaBlocksInDeapth; megaBlockDeapthIndex++) {
            for (int megaBlockWidthIndex = 0; megaBlockWidthIndex < megaBlocksInWidth; megaBlockWidthIndex++) {
                // we read the header of the macro block.
                if (currentInputReadingPtr[0] != 255) {
                    printf("Error. expecting a mega block header first 111111111 here. Read %d instead",currentInputReadingPtr[0]);
                    printf("Reading offset is %d\n",(int)(currentInputReadingPtr - *inBuffer));
                    exit(-1);
                }
                if (currentInputReadingPtr[1] != 0) {
                    printf("Error. expecting a mega block header second 00000000 here. Read %d instead",currentInputReadingPtr[1]);
                    exit(-1);
                }
                 if (currentInputReadingPtr[2] != 255) {printf("Error. expecting a mega block header third 111111111 here."); exit(-1);}
                if (currentInputReadingPtr[3] == 0) {
                     if (debugMBDec) printf("all zeros mega block\n");
                    currentInputReadingPtr += 4;
                    
                    for (int innerHeightInMegaBlock = 0; innerHeightInMegaBlock < macroBlockDimension; innerHeightInMegaBlock++) {
                        for (int innerDeapthInMegaBlock = 0 ; innerDeapthInMegaBlock < macroBlockDimension; innerDeapthInMegaBlock++) {
                            for (int innerWidthInMegaBlock = 0 ; innerWidthInMegaBlock < macroBlockDimension; innerWidthInMegaBlock++) {
                                
                                // look for the index of chroma block in the output buffer
                                // first find the location on the TV
                                
                                int hightOfMegaBlock = megaBlockHeightIndex * macroBlockDimension;
                                int heightOfChromaBlock = hightOfMegaBlock + innerHeightInMegaBlock;
                                
                                int depthOfMegaBlock = megaBlockDeapthIndex * macroBlockDimension;
                                int depthOfChromaBlock = depthOfMegaBlock + innerDeapthInMegaBlock;
                                
                                int widthOfMegaBlock = megaBlockWidthIndex * macroBlockDimension;
                                int widthOfChromaBlock = widthOfMegaBlock + innerWidthInMegaBlock;
                                
                                struct zLocation locationOnTV = makeZLocation(widthOfChromaBlock, depthOfChromaBlock, heightOfChromaBlock);
                                int indexOfChromaBlockInOutputBuffer = getIndexOfLocationInTVSize(locationOnTV, makeTVSize(chromaBlocksInWidth, chromaBlocksInDeapth, chromaBlocksInHeight));
                                
                                unsigned char* thisChromaBlockWritingBuffer = *outBuffer + (indexOfChromaBlockInOutputBuffer * chromaBlockSizeInBytes);
                                
                                // write zeros
                                for (int k=0; k<chromaBlockSizeInBytes;k++) thisChromaBlockWritingBuffer[k] = 0;
                                
                            }
                        }
                    }
                    
                } else {
                    if (debugMBDec)  printf("mega block has content\n");
                    currentInputReadingPtr += 4;
                    
                    // We start with Cb Cr. In these two we don't need to look at macro blocks yet
                    
                    for (int j=0;j<2;j++) { // one for Cb other for Cr
                    
                        for (int innerHeightInMegaBlock = 0; innerHeightInMegaBlock < macroBlockDimension; innerHeightInMegaBlock++) {
                            for (int innerDeapthInMegaBlock = 0 ; innerDeapthInMegaBlock < macroBlockDimension; innerDeapthInMegaBlock++) {
                                for (int innerWidthInMegaBlock = 0 ; innerWidthInMegaBlock < macroBlockDimension; innerWidthInMegaBlock++) {
                                    
                                    // look for the index of chroma block in the output buffer
                                    // first find the location on the TV
                                    
                                    int hightOfMegaBlock = megaBlockHeightIndex * macroBlockDimension;
                                    int heightOfChromaBlock = hightOfMegaBlock + innerHeightInMegaBlock;
                                    
                                    int depthOfMegaBlock = megaBlockDeapthIndex * macroBlockDimension;
                                    int depthOfChromaBlock = depthOfMegaBlock + innerDeapthInMegaBlock;
                                    
                                    int widthOfMegaBlock = megaBlockWidthIndex * macroBlockDimension;
                                    int widthOfChromaBlock = widthOfMegaBlock + innerWidthInMegaBlock;
                                    
                                    struct zLocation locationOnTV = makeZLocation(widthOfChromaBlock, depthOfChromaBlock, heightOfChromaBlock);
                                    int indexOfChromaBlockInOutputBuffer = getIndexOfLocationInTVSize(locationOnTV, makeTVSize(chromaBlocksInWidth, chromaBlocksInDeapth, chromaBlocksInHeight));
                                    
                                    unsigned char* thisChromaBlockWritingBuffer = *outBuffer + (indexOfChromaBlockInOutputBuffer * chromaBlockSizeInBytes);
                                    
                                    // write Cb/Cr
                                    thisChromaBlockWritingBuffer[j] = *currentInputReadingPtr;
                                    currentInputReadingPtr++;
                                    
                                }
                            }
                        }
                    }
                    
                    // we read through the Cb and Cr of the input buffer
                    // Now we start reading luma values of 8 macro blocks, then alpha of 8 macro blocks
                    
                    for (int j=0; j<2; j++) { // one for luma one for alpha
                    
                        ///// now we need to itterate each macro block for luma and alpha
                        for (int macroBlockHeightIndexInMegaBlock = 0; macroBlockHeightIndexInMegaBlock < megaBlockDimension; macroBlockHeightIndexInMegaBlock++) {
                            for (int macroBlockDeapthIndexInMegaBlock = 0; macroBlockDeapthIndexInMegaBlock < megaBlockDimension; macroBlockDeapthIndexInMegaBlock++ ) {
                                for (int macroBlockWidthIndexInMegaBlock = 0; macroBlockWidthIndexInMegaBlock < megaBlockDimension; macroBlockWidthIndexInMegaBlock++) {
                                    
                                    for (int innerHeightIndexInMacroBlock = 0; innerHeightIndexInMacroBlock < macroBlockDimension; innerHeightIndexInMacroBlock ++) {
                                        for (int innerDeapthIndexInMacroBlock = 0; innerDeapthIndexInMacroBlock < macroBlockDimension; innerDeapthIndexInMacroBlock++) {
                                            for (int innerWidthIndexInMacroBlock = 0; innerWidthIndexInMacroBlock < macroBlockDimension; innerWidthIndexInMacroBlock++) {
                                                
                                                // get the location of this chroma block in the output buffer
                                                // index is total height * chromablocks in plane + total depth * chroma blocks in widht + totalWidth
                                                // this can be achieved by inding the index of the chroma blocks in the output buffer like this:
                                                //getIndexOfLocation(totalHeight,totalDeapth,totalWidth)inTVSize(chromaBlocksInTVHeight,chromaBlocksInTvDeapth,chromaBlocksInTVWidth)
                                                //
                                                // We start with total hight, which is  height of the Meaga block * total chroma blocks in a mega bclok heigh + height of macro block * chroma blocks in a macro block + height of the inner pixel / height of the chroma block (remainder will be the inner index in the chroma block buffer according to [cr,cb,y0,a0,y1,a1,...,a7]
                                                //
                                                int chromaBlocksInAMegaBlockHeight = megaBlockDimension * macroBlockDimension / chromaBlockHeight;
                                                int heightOfMegaBlockInChromaBlocks = megaBlockHeightIndex * chromaBlocksInAMegaBlockHeight;
                                                int chromaBlocksInAMacroBlockHeight = macroBlockDimension / chromaBlockHeight;
                                                int heightOfMacroBlockInChromaBlocks = macroBlockHeightIndexInMegaBlock * chromaBlocksInAMacroBlockHeight;
                                                int chromaBlockHeightOfThisPixel = innerHeightIndexInMacroBlock / chromaBlockHeight;
                                                int totalHeightInChromaBlocks = heightOfMegaBlockInChromaBlocks + heightOfMacroBlockInChromaBlocks + chromaBlockHeightOfThisPixel;
                                                
                                                // The total depth is the depth of this mega block * total chroma blocks in a mega blcoks depth + depth of macro block * totalChroma blocks in macro block * depth of inner pixel / depth of chroma block (remainder will be the inner index in the chroma blocks as above
                                                int chromaBlocksInAMegaBlockDeapth = megaBlockDimension * macroBlockDimension / chromaBlockDeapth;
                                                int depthOfMegaBlockInChromaBlocks = megaBlockDeapthIndex * chromaBlocksInAMegaBlockDeapth;
                                                int chromaBlocksInAMacroBlockDeapth = macroBlockDimension / chromaBlockDeapth;
                                                int depthOfMacroBlockInChromaBlocks = macroBlockDeapthIndexInMegaBlock * chromaBlocksInAMacroBlockDeapth;
                                                int chromaBlockDeapthOfThisPixel = innerDeapthIndexInMacroBlock / chromaBlockDeapth;
                                                int totalDeapthtInChromaBlocks = depthOfMegaBlockInChromaBlocks + depthOfMacroBlockInChromaBlocks + chromaBlockDeapthOfThisPixel;
                                                
                                                
                                                // the total width is the width of the mega block * total chroma blocks in a mega block width + depth of the macro block within the mega block * total chroma block in macro block * innder width of pixel / width of chroma block (remainder etcc as above)
                                                int chromaBlocksInAMegaBlockWidth = megaBlockDimension * macroBlockDimension / chromaBlockWidth;
                                                int widthOfMegaBlockInChromaBlocks = megaBlockWidthIndex * chromaBlocksInAMegaBlockWidth;
                                                int chromaBlocksInAMacroBlockWidth = macroBlockDimension / chromaBlockWidth;
                                                int widthOfMacroBlockInChromaBlocks = macroBlockWidthIndexInMegaBlock * chromaBlocksInAMacroBlockWidth;
                                                int chromaBlockWidthOfThisPixel = innerWidthIndexInMacroBlock / chromaBlockWidth;
                                                int totalWidthtInChromaBlocks = widthOfMegaBlockInChromaBlocks + widthOfMacroBlockInChromaBlocks + chromaBlockWidthOfThisPixel;
                                                
                                                // grab the location of the chroma block on the TV (on the ouput buffer really)
                                                struct zLocation locationOfChromaBlockOnTV = makeZLocation(totalWidthtInChromaBlocks, totalDeapthtInChromaBlocks, totalHeightInChromaBlocks);
                                                // Calculate the index of chroma block in the output buffer
                                                unsigned int indexOfChromaBlockInOutputBuffer = getIndexOfLocationInTVSize(locationOfChromaBlockOnTV, makeTVSize(chromaBlocksInWidth, chromaBlocksInDeapth, chromaBlocksInHeight));
                                                // The ptr to the beginning of the chroma block
                                                unsigned char* pointerToTheCurrentChromaBlockInTheOutputBuffer = *outBuffer + (indexOfChromaBlockInOutputBuffer * chromaBlockSizeInBytes);
                                                
                                                // So the pointer is set to the beginning of the chroma block. We advance by 2 which are Cb and Cr
                                                pointerToTheCurrentChromaBlockInTheOutputBuffer += 2;
                                                
                                                // now we should write luma/alpha
                                                // if j == 0 this is luma (y)
                                                // since the buffer is y,a,y,a, etc.. we add j to the pointer to set the start offset to alpha if needed
                                                pointerToTheCurrentChromaBlockInTheOutputBuffer += j;
                                                
                                                // now we get the actual pixel from the macro block and set it to the appropriate index in the buffer using the remainders
                                                //
                                                // first get the  remainders
                                                int heightInChromaBlock = innerHeightIndexInMacroBlock % chromaBlockHeight;
                                                int depthInChromaBlock = innerDeapthIndexInMacroBlock % chromaBlockDeapth;
                                                int widthInChromaBlock = innerWidthIndexInMacroBlock % chromaBlockWidth;
                                                
                                                int indexInSmallChromaBuffer = getIndexOfLocationInTVSize(makeZLocation(widthInChromaBlock, depthInChromaBlock, heightInChromaBlock), makeTVSize(chromaBlockWidth, chromaBlockDeapth, chromaBlockHeight));
                                                
                                                // we double the index by the 2 string (y,a,y,a)
                                                indexInSmallChromaBuffer *= 2;
                                                
                                                // and we write the value
                                                pointerToTheCurrentChromaBlockInTheOutputBuffer[indexInSmallChromaBuffer] = *currentInputReadingPtr;
                                                currentInputReadingPtr++;

                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }



    
    
    return 1;
}

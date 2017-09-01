//
//  FiveDVFrameEncDec.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 5/24/17.
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

#import "FiveDVFrameEncDec.h"


// when compressing a frame we know the uncompressed size, but not the compressed size. the last argument will retun the size. The 2nd argument should be allocated within this method, and freed by the caller
int compressTVFrame(zPixel** uncompressedFrameBuffer,struct EncDecData tvData,unsigned char** compressedFrameBuffer,unsigned int * compressedFrameLength) {
    
    // for now we include only the first phase for testing
    // in the future the length of the compressed frame is unknown until hoffman coding takes place. For now, we just to chroma block here
    
    unsigned char* chromaBlocksBuffer;
    performPhaseEncodeChromaBlocks((unsigned char**)uncompressedFrameBuffer, tvData, &chromaBlocksBuffer);
    
    unsigned char* megaBlocksBuffer;
    performPhaseEncodeMacroBlocks(&chromaBlocksBuffer, tvData,&megaBlocksBuffer,compressedFrameLength);
    
    free(chromaBlocksBuffer);
    chromaBlocksBuffer = 0;

    unsigned char* dctBuffer;
    performPhaseDCT(&megaBlocksBuffer,tvData,&dctBuffer,compressedFrameLength);
    free(megaBlocksBuffer);
    megaBlocksBuffer = 0;
    
    unsigned char* quantizedBuffer;
    performPhaseEncQuantization(&dctBuffer,tvData,&quantizedBuffer, compressedFrameLength);
    free(dctBuffer);
    dctBuffer = 0;
    
    performPhaseEncZigZag(&quantizedBuffer, tvData, compressedFrameBuffer, compressedFrameLength);
    free(quantizedBuffer);
    
    
    return 1;
}

// decompressing should contain the length of the input buffer
int decompressTVFrame(unsigned char** compressedFrameBuffer, unsigned int compressedFrameLength, struct EncDecData tvData , zPixel**uncompressedFrameBuffer) {
    
    
    unsigned char* unZigZaggedBuffer;
    performPhaseDecZigZag(compressedFrameBuffer, tvData, &unZigZaggedBuffer);
    
    unsigned char* dequantizedBuffer;
    performPhaseDecQuantization(&unZigZaggedBuffer, tvData, &dequantizedBuffer);
    free (unZigZaggedBuffer);
    unZigZaggedBuffer = 0;
    
    unsigned char* IDCTBuffer;
    performPhaseIDCT(&dequantizedBuffer, tvData, &IDCTBuffer);
    free(dequantizedBuffer);
    dequantizedBuffer = 0;
    
    // in the future we will use the 2nd argument to verify hoffman coding was decompressed correctly
    unsigned char* chromaBlocksBuffer;
    performPhaseDecodeMacroBlocks(&IDCTBuffer, tvData, &chromaBlocksBuffer);
   
    free (IDCTBuffer);
    IDCTBuffer = 0;
    
    performPhaseDecodeChromaBlocks(&chromaBlocksBuffer, tvData, (unsigned char**)uncompressedFrameBuffer);
    
    free(chromaBlocksBuffer);
    chromaBlocksBuffer = 0;
    
    
    return 1;
}

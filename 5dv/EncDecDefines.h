//
//  EndDecDefines.h
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


#ifndef EndDecDefines_h
#define EndDecDefines_h

#import "SimDefs.h"

#define chromaBlockWidth 2
#define chromaBlockHeight 2
#define chromaBlockDeapth 2

#define macroBlockDimension 8  // a macro block is 8 x 8 x8 values long. It could be filled with luma values which came from 4x4x4 chroma groups, or from Cr or Cb that came from 8x8x8 chroma groups.

#define megaBlockDimension 2 // a mega block is 2x2x2 macro blocks where Cb and Cr are shared from all 8 macro blocks

struct FDVFileHeader {
    
    struct TVSize tvsize;
    float totalFrames;
    float framesPerSecond;
    struct TVSize chromaBlockSize;
    float dataSizeInBytes;
    
    
};


struct EncDecData {
    
    struct TVSize tvsize;
    float totalFrames;
    float framesPerSecond;
    struct TVSize chromaBlockSize;
    float dataSizeInBytes;

};




typedef enum encodeFramePhase {
    phaseChromaBlocks,
    phaseMacroBlocks,
    phaseDCT,
    phaseQuantizeDCT,
    phaseZigZag,
    phaseHoffman
    
}encodeFramePhase;


struct yCbCr {
    unsigned char y;
    unsigned char Cb;
    unsigned char Cr;
};

struct yCbCrGroup {
    unsigned char Cb;
    unsigned char Cr;
    void* YandAlpha;
};

// represents an 8x8x8 block of values (they could be luma,alpha or chroma)
struct macroBlock {
    unsigned char* buffer; 
};

struct DCTMacroBlock {
    short int* buffer;
};

struct megaBlock {
    struct macroBlock alphaBlocks[megaBlockDimension * megaBlockDimension * megaBlockDimension];
    struct macroBlock lumaMacroBlocks[megaBlockDimension * megaBlockDimension * megaBlockDimension];
    struct macroBlock CrBlock;
    struct macroBlock CbBlock;
};

struct DCTMegaBlock {
    struct DCTMacroBlock alphaBlocks[megaBlockDimension * megaBlockDimension * megaBlockDimension];
    struct DCTMacroBlock lumaMacroBlocks[megaBlockDimension * megaBlockDimension * megaBlockDimension];
    struct DCTMacroBlock CrBlock;
    struct DCTMacroBlock CbBlock;
};



struct zLocation {
    int x; // equivalent to width
    int y; // equivalent to height
    int z; // equivalent to depth
};

#endif /* EndDecDefines_h */

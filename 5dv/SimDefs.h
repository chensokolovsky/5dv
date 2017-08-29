//
//  SimDefs.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/7/16.
//  Copyright © 2016 chen. All rights reserved.
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

#ifndef SimDefs_h
#define SimDefs_h



#define debugMain 0

#define TVFramesPerZip 10



typedef struct line {
    float x1;
    float y1;
    float x2;
    float y2;
}line;

typedef struct zLine {
    float x1;
    float y1;
    float z1;
    float x2;
    float y2;
    float z2;
} zLine;


typedef struct zShape {
    int totalLines;
    struct zLine* zLines;
}zShape;

typedef struct drawing {
    int totalLines;
    struct line* lines;
}drawing;

typedef struct zPoint {
    float x;
    float y;
    float z;
}zPoint;


// can serve also as color definition
typedef struct zPixel {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char alpha;
}zPixel;



typedef struct TVSize {
    float width;
    float deapth;
    float height;
}TVSize;








#endif /* _SimDefs_h */

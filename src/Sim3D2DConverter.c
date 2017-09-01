//
//  Sim3D2DConverter.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/7/16.
//  Copyright © 2016 chen. All rights reserved.
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

#include <stdio.h>

#import "Sim3D2DConverter.h"


void initDrawingWithShape ( struct drawing* d, struct zShape* shape) {
    
    d->totalLines = shape->totalLines;
    d->lines = (line*)malloc (sizeof(line) * d->totalLines);
    
}


// rotates a single point on a single axis
void rotate (float x, float y, float ang, float* resX, float * resY) {
    // rotate the x axis
    float xDist = sqrtf(x * x + y * y);
    float add = 0;
    if (x < 0) add = M_PI;
    if (x > 0 && y < 0) add = 2  * M_PI;
    float ang1 = atan(y / x);
    ang1 += ang + add;
    *resX = xDist * cos (ang1);
    *resY = xDist * sin (ang1);
}

// converts a 3d point into 2 d according to 3 rotation angles
void convertPoint(float x1, float y1, float z1,float xAng,float yAng, float zAng,float* xRes, float* yRes) {
    
    int debug = 0;
    
    if (debug) printf("************************\n");
    if (debug) printf("input point : %.0f, %.0f , %.0f\n",x1,y1,z1);
    
    float radXAng = M_PI * xAng / 180.0;
    float radYAng = M_PI * yAng / 180.0;
    float radZAng = M_PI * zAng / 180.0;
    
    
    // rotate the x axis
    float z2,y2;
    rotate(z1, y1, radXAng, &z2, &y2);
    
    if (debug) printf("after rotating x by %.0f degrees point is: %.0f, %.0f, %.0f\n" ,xAng,x1,y2,z2);
    
    float x3,z3;
    rotate(x1,z2,radYAng,&x3,&z3);
    
    
    if (debug) printf("after rotating y by %.0f degrees point is: %.0f, %.0f, %.0f\n", yAng, x3, y2, z3);
    
    float x4,y4;
    rotate(x3, y2, radZAng, &x4, &y4);
    
    
    if (debug) printf("after rotating %.0f final point is: %.0f, %.0f\n",zAng,x4,y4);
    
    *xRes = x4;
    *yRes = y4;
    
    
    
}

void convertPoints (struct zPoint* zp, SDL_Point* p, float xAng, float yAng, float zAng) {
    
    float resX;
    float resY;
    convertPoint(zp->x, zp->y, zp->z, xAng, yAng, zAng,&resX, &resY);
    p->x = resX;
    p->y = resY;
    
    
}

void convertTV(TVSize size,SDL_Point* pts,float totalPixels, float xAng, float yAng, float zAng) {
    
    
    int i=0;
    for (int tvh = 0; tvh < size.height; tvh++) {
        for (int tvw=0; tvw< size.depth; tvw++) {
            for (int tvd = 0; tvd < size.width; tvd++) {
    
                SDL_Point* twodpoint = &(pts[i]);
                zPoint threedpoint;
                threedpoint.x = tvw;
                threedpoint.y = tvh;
                threedpoint.z = tvd;
                convertPoints(&threedpoint, twodpoint, xAng, yAng, zAng);
                i++;
            }
        }
    }
}

void convertDrawing(struct zShape* shape, struct drawing* drawing, float xAng, float yAng, float zAng) {
    
    
    for (int i = 0; i < shape->totalLines; i++) {
        
        struct zLine shapeLine = (shape->zLines)[i];
        
        line* drawingLine = &((drawing->lines)[i]);
        // from point
        convertPoint(shapeLine.x1,shapeLine.y1,shapeLine.z1,xAng,yAng,zAng,&(drawingLine->x1), &(drawingLine->y1));
        // to point
        convertPoint(shapeLine.x2,shapeLine.y2,shapeLine.z2,xAng,yAng,zAng,&(drawingLine->x2), &(drawingLine->y2));
        
        
    }
    
}





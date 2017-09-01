//
//  SpaceShaperCreator.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/22/16.
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

#import "SpaceShapesCreator.h"

// assuming tv is already cleared. Clearing sets the positions of each pixel in the array
void drawTVEdges(struct zPixel* tv, struct TVSize size, struct zPixel color) {
    
    int plateSize = size.width * size.depth;
        
    for (int tvh = 0; tvh < size.height; tvh++) {
        for (int tvw=0; tvw< size.depth; tvw++) {
            for (int tvd = 0; tvd < size.width; tvd++) {
                
                int indexInPlate = tvw * size.depth + tvd;
                int globalPixelIndex = tvh * plateSize + indexInPlate;
                
                zPixel* p = &(tv[globalPixelIndex]);
                
                int isEdge = 0;
                if (tvh == 0 && tvw == 0) isEdge = 1;
                if (tvw == 0 && tvd == 0) isEdge = 1;
                if (tvd == 0 && tvh == 0) isEdge = 1;
                if (tvh == size.height-1 && tvw == size.width-1) isEdge = 1;
                if (tvw == size.width-1 && tvd == size.depth-1) isEdge = 1;
                if (tvd == size.depth-1 && tvh == size.height-1) isEdge = 1;
                if (tvh == 0 && tvw == size.width-1) isEdge = 1;
                if (tvw == 0 && tvd == size.depth-1) isEdge = 1;
                if (tvd == 0 && tvh == size.height-1) isEdge = 1;
                if (tvh == size.height-1 && tvw == 0) isEdge = 1;
                if (tvw == size.width-1 && tvd == 0) isEdge = 1;
                if (tvd == size.depth-1 && tvh == 0) isEdge = 1;
                
                if (!isEdge) continue;
                
                int val = isEdge ? 255 : 0;
                
                p->R = val;
                p->G = val;
                p->B = val;
                p->alpha = val;
                
            }
        }
    }
}


void clearTV(struct zPixel* tv, struct TVSize size) {
    
    int i = 0;
    for (int tvh = 0; tvh < size.height; tvh++) {
        for (int tvw=0; tvw< size.depth; tvw++) {
            for (int tvd = 0; tvd < size.width; tvd++) {
                
                zPixel* p = &(tv[i]);
                
                p->R = 0;
                p->G = 0;
                p->B = 0;
                p->alpha = 0;
                
                i++;
                
            }
        }
    }
}

int distanceBetweenZPoints( struct zPoint pointA, struct zPoint pointB) {
    
    // squrt( (x2-x1)^2 + (y2-y1)^2 + (z2-z1)^2 )
    float x2minusx1sqr = pointB.x - pointA.x;
    x2minusx1sqr *= x2minusx1sqr;
    
    float y2minusy2sqr = pointB.y - pointA.y;
    y2minusy2sqr *= y2minusy2sqr;
    
    float z2minusz1sqr = pointB.z - pointA.z;
    z2minusz1sqr *= z2minusz1sqr;
    
    float sum = x2minusx1sqr + y2minusy2sqr + z2minusz1sqr;
   
    return (int)(sqrt(sum));
}


void addSphere(struct zPixel* tv, struct TVSize size, struct zPoint center, int radius, struct zPixel color,int filled) {
    
    int i= 0;
    
    for (int tvh = 0; tvh < size.height; tvh++) {
        for (int tvw=0; tvw< size.depth; tvw++) {
            for (int tvd = 0; tvd < size.width; tvd++) {
                
                zPixel* p = &(tv[i]);
                
                zPoint pixelLocation;
                pixelLocation.x = tvw;
                pixelLocation.y = tvh;
                pixelLocation.z = tvd;
                
                int isOnSphere = 0;
                int dist = distanceBetweenZPoints(pixelLocation,center);
                if (!filled) if (dist == radius) isOnSphere = 1;
                if (filled) if (dist <= radius) isOnSphere = 1;
                i++;

                
                if (!isOnSphere) continue;
                
                p->R = color.R;
                p->G = color.G;
                p->B = color.B;
                p->alpha = color.alpha;
                
                
            }
        }
    }

    
}




void fillTVWithColorSquares(struct zPixel* tv, struct TVSize size) {

    
    int i=0;
     for (int tvh = 0; tvh < size.height; tvh++) {
         for (int tvw=0; tvw< size.depth; tvw++) {
             for (int tvd = 0; tvd < size.width; tvd++) {
                 
                zPixel* p = &(tv[i]);
                 
                 p->R = (tvw < size.width * 0.5) ? 125 : 255;
                 p->G = (tvh > size.height * 0.5) ? 0 : 255;
                 p->B = (tvd > size.depth * 0.5) ? 0 : 255;
                 p->alpha = 10;
                 
                 i++;
                 
             }
         }
     }
};

//
//  SimShapeCreator.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/9/16.
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

#import "SimShapeCreator.h"


void createCube(struct zShape* s, struct zLine diagnal) {
    
    
    s->totalLines = 12;
    s->zLines = malloc(sizeof(struct zLine) *  s->totalLines);
    
    
    struct zLine* firstLine = &(s->zLines[0]);
    firstLine->x1 = diagnal.x1;
    firstLine->y1 = diagnal.y1;
    firstLine->z1 = diagnal.z1;

    firstLine->x2 = diagnal.x2;
    firstLine->y2 = diagnal.y1;
    firstLine->z2 = diagnal.z1;
    
    struct zLine* secondLine = &(s->zLines[1]);
  
    secondLine->x1 = diagnal.x2;
    secondLine->y1 = diagnal.y1;
    secondLine->z1 = diagnal.z1;
    
    secondLine->x2 = diagnal.x2;
    secondLine->y2 = diagnal.y2;
    secondLine->z2 = diagnal.z1;

    
    struct zLine* thirdLine = &(s->zLines[2]);
    thirdLine->x1 = diagnal.x2;
    thirdLine->y1 = diagnal.y2;
    thirdLine->z1 = diagnal.z1;
    
    thirdLine->x2 = diagnal.x1;
    thirdLine->y2 = diagnal.y2;
    thirdLine->z2 = diagnal.z1;

    
    struct zLine* fourthLine = &(s->zLines[3]);
    fourthLine->x1 = diagnal.x1;
    fourthLine->y1 = diagnal.y2;
    fourthLine->z1 = diagnal.z1;
    
    fourthLine->x2 = diagnal.x1;
    fourthLine->y2 = diagnal.y1;
    fourthLine->z2 = diagnal.z1;

    
    struct zLine* fifthLine = &(s->zLines[4]);
    fifthLine->x1 = diagnal.x2;
    fifthLine->y1 = diagnal.y2;
    fifthLine->z1 = diagnal.z2;
    
    fifthLine->x2 = diagnal.x2;
    fifthLine->y2 = diagnal.y1;
    fifthLine->z2 = diagnal.z2;

    
    struct zLine* sixthLine = &(s->zLines[5]);
    sixthLine->x1 = diagnal.x2;
    sixthLine->y1 = diagnal.y1;
    sixthLine->z1 = diagnal.z2;
    
    sixthLine->x2 = diagnal.x1;
    sixthLine->y2 = diagnal.y1;
    sixthLine->z2 = diagnal.z2;

    
    struct zLine* seventhLine = &(s->zLines[6]);
    seventhLine->x1 = diagnal.x1;
    seventhLine->y1 = diagnal.y1;
    seventhLine->z1 = diagnal.z2;
    
    seventhLine->x2 = diagnal.x1;
    seventhLine->y2 = diagnal.y2;
    seventhLine->z2 = diagnal.z2;
    
    
    struct zLine* eighthLine = &(s->zLines[7]);
    eighthLine->x1 = diagnal.x1;
    eighthLine->y1 = diagnal.y2;
    eighthLine->z1 = diagnal.z2;
    
    eighthLine->x2 = diagnal.x2;
    eighthLine->y2 = diagnal.y2;
    eighthLine->z2 = diagnal.z2;

    
    struct zLine* ninthLine = &(s->zLines[8]);
    
    ninthLine->x1 = diagnal.x1;
    ninthLine->y1 = diagnal.y1;
    ninthLine->z1 = diagnal.z1;
    
    ninthLine->x2 = diagnal.x1;
    ninthLine->y2 = diagnal.y1;
    ninthLine->z2 = diagnal.z2;

    
    struct zLine* tenthhLine = &(s->zLines[9]);
    
    tenthhLine->x1 = diagnal.x1;
    tenthhLine->y1 = diagnal.y2;
    tenthhLine->z1 = diagnal.z1;
    
    tenthhLine->x2 = diagnal.x1;
    tenthhLine->y2 = diagnal.y2;
    tenthhLine->z2 = diagnal.z2;
    
    zLine* eleventhLine = &(s->zLines[10]);
    
    eleventhLine->x1 = diagnal.x2;
    eleventhLine->y1 = diagnal.y1;
    eleventhLine->z1 = diagnal.z1;
    
    eleventhLine->x2 = diagnal.x2;
    eleventhLine->y2 = diagnal.y1;
    eleventhLine->z2 = diagnal.z2;

    struct zLine* twelvethLine = &(s->zLines[11]);
   
    twelvethLine->x1 = diagnal.x2;
    twelvethLine->y1 = diagnal.y2;
    twelvethLine->z1 = diagnal.z1;
    
    twelvethLine->x2 = diagnal.x2;
    twelvethLine->y2 = diagnal.y2;
    twelvethLine->z2 = diagnal.z2;

}

void createCenteredCube(struct zShape* s, float corner1, float corner2) {
   
    s->totalLines = 12;
    s->zLines = (zLine*)malloc(sizeof(zLine) *  s->totalLines);
    
    float big = corner1;
    float small = corner2;
    
    struct zLine* firstLine = &(s->zLines[0]);
    firstLine->x1 = small;
    firstLine->y1 = big;
    firstLine->x2 = big;
    firstLine->y2 = big;
    firstLine->z1 = small;
    firstLine->z2 = small;
    
    struct zLine* secondLine = &(s->zLines[1]);
    secondLine->x1 = big;
    secondLine->y1 = big;
    secondLine->x2 = big;
    secondLine->y2 = small;
    secondLine->z1 = small;
    secondLine->z2 = small;
    
    struct zLine* thirdLine = &(s->zLines[2]);
    thirdLine->x1 = big;
    thirdLine->y1 = small;
    thirdLine->x2 = small;
    thirdLine->y2 = small;
    thirdLine->z1 = small;
    thirdLine->z2 = small;
    
    struct zLine* fourthLine = &(s->zLines[3]);
    fourthLine->x1 = small;
    fourthLine->y1 = small;
    fourthLine->x2 = small;
    fourthLine->y2 = big;
    fourthLine->z1 = small;
    fourthLine->z2 = small;
    
    struct zLine* fifthLine = &(s->zLines[4]);
    fifthLine->x1 = small;
    fifthLine->y1 = big;
    fifthLine->x2 = big;
    fifthLine->y2 = big;
    fifthLine->z1 = big;
    fifthLine->z2 = big;
    
    struct zLine* sixthLine = &(s->zLines[5]);
    sixthLine->x1 = big;
    sixthLine->y1 = big;
    sixthLine->x2 = big;
    sixthLine->y2 = small;
    sixthLine->z1 = big;
    sixthLine->z2 = big;
    
    struct zLine* seventhLine = &(s->zLines[6]);
    seventhLine->x1 = big;
    seventhLine->y1 = small;
    seventhLine->x2 = small;
    seventhLine->y2 = small;
    seventhLine->z1 = big;
    seventhLine->z2 = big;
    
    struct zLine* eighthLine = &(s->zLines[7]);
    eighthLine->x1 = small;
    eighthLine->y1 = small;
    eighthLine->x2 = small;
    eighthLine->y2 = big;
    eighthLine->z1 = big;
    eighthLine->z2 = big;
    
    struct zLine* ninthLine = &(s->zLines[8]);
    ninthLine->x1 = small;
    ninthLine->y1 = small;
    ninthLine->x2 = small;
    ninthLine->y2 = small;
    ninthLine->z1 = small;
    ninthLine->z2 = big;
    
    struct zLine* tenthhLine = &(s->zLines[9]);
    tenthhLine->x1 = small;
    tenthhLine->y1 = big;
    tenthhLine->x2 = small;
    tenthhLine->y2 = big;
    tenthhLine->z1 = small;
    tenthhLine->z2 = big;
    
    struct zLine* eleventhLine = &(s->zLines[10]);
    eleventhLine->x1 = big;
    eleventhLine->y1 = small;
    eleventhLine->x2 = big;
    eleventhLine->y2 = small;
    eleventhLine->z1 = small;
    eleventhLine->z2 = big;
    
    struct zLine* twelvethLine = &(s->zLines[11]);
    twelvethLine->x1 = big;
    twelvethLine->y1 = big;
    twelvethLine->x2 = big;
    twelvethLine->y2 = big;
    twelvethLine->z1 = small;
    twelvethLine->z2 = big;
    

}

//
//  MovieFrameGrabber.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/26/16.
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

#import "MovieFrameGrabber.h"


void grabThreeDImage(struct zPixel* tv, struct TVSize tvsize, int imageIndex) {
    
    
    zPoint center;
    center.x = 40 + imageIndex;
    center.y = 40;
    center.z = 40;
    
    zPoint center2;
    center2.x = 80;
    center2.y = 10 + imageIndex;
    center2.z = 75;
    
    zPixel color;
    color.R = 255;
    color.G = 255;
    color.B = 255;
    color.alpha = 255;
    
    zPixel color2;
    color2.R = 127;
    color2.G = 0;
    color2.B = 255;
    color2.alpha = 125;
    
    // fillTVWithColorSquares(zpixels, tvsize);
    clearTV(tv,tvsize);
    drawTVEdges(tv, tvsize, color);
    addSphere(tv, tvsize, center,30,color ,1 );
    addSphere(tv, tvsize, center2, 20, color2, 1);
    
    
}

void grabThreeDImage2(struct zPixel* tv, struct TVSize tvsize, int imageIndex) {
    
    
    
    
    // fillTVWithColorSquares(zpixels, tvsize);
    clearTV(tv,tvsize);
    zPixel color;
    color.R = 255;
    color.G = 255;
    color.B = 255;
    color.alpha = 255;
    drawTVEdges(tv, tvsize, color);

    int index = tvsize.width * tvsize.depth * imageIndex + tvsize.width * imageIndex + imageIndex;
    
    zPixel* p = &(tv[index]);
  
    p->R = 255;
    p->G = 255;
    p->B = 255;
    p->alpha = 255;
    
    
}



void createMovieToAFile2(char* str) {
    
  
    FILE* spheares1;
    fiveDVUFileHeader spheres1header;
    
    float x = 112;
    float y = 80;
    float z = 112;
    float totalFrames = 30;
    
    spheres1header.totalFrames = totalFrames;
    spheres1header.framesPerSecond = 5;
    
    spheres1header.tvsize.width = x;
    spheres1header.tvsize.height = y;
    spheres1header.tvsize.depth = z;
    
    float frameSizeInPizels = x*y*z;
    float totalSize = frameSizeInPizels*sizeof(zPixel);
    if (debugMain) printf("allocating %f bytes \n",totalSize);
    
    zPixel* frameBuffer = (zPixel*)malloc((size_t)totalSize);
    if (!frameBuffer) {
        printf("Error ! can't malloc all this data");
        return;
    }
    
    
    // create the file
    createFiveDVUFile(str, spheres1header, &spheares1);
    
    // add the frames
    for (int i=0; i< totalFrames; i++) {
        grabThreeDImage(frameBuffer, spheres1header.tvsize, i);
        writeFrameToFiveDVUFile(spheres1header.tvsize, frameBuffer, &spheares1);
    }
    
    
    free (frameBuffer);
    fclose(spheares1);
    

}


// This is version 10 (a fix for version 9) a single mega block movie
void createMovieToAFile3() {
    
    char filename[] = "/Users/chensokolovsky/Desktop/movies/SingleMegaBlock801.5dvu";
    char* str = filename;
    FILE* spheares1;
    fiveDVUFileHeader spheres1header;
    
    float x = 16;
    float y = 16;
    float z = 16;
    float totalFrames = 16;
    
    spheres1header.totalFrames = totalFrames;
    spheres1header.framesPerSecond = 5;
    
    spheres1header.tvsize.width = x;
    spheres1header.tvsize.height = y;
    spheres1header.tvsize.depth = z;
    
    float frameSizeInPizels = x*y*z;
    float totalSize = frameSizeInPizels*sizeof(zPixel);
    if (debugMain) printf("allocating %f bytes \n",totalSize);
    
    zPixel* frameBuffer = (zPixel*)malloc((size_t)totalSize);
    if (!frameBuffer) {
        printf("Error ! can't malloc all this data");
        return;
    }
    
    
    // create the file
    createFiveDVUFile(str, spheres1header, &spheares1);
    
    // add the frames
    for (int i=0; i< totalFrames; i++) {
        grabThreeDImage2(frameBuffer, spheres1header.tvsize, i);
        writeFrameToFiveDVUFile(spheres1header.tvsize, frameBuffer, &spheares1);
    }
    
    
    free (frameBuffer);
    fclose(spheares1);
    
    
}

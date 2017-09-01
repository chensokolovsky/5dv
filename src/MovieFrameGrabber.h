//
//  MovieFrameGrabber.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/26/16.
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

#ifndef MovieFrameGrabber_h
#define MovieFrameGrabber_h

#include <stdio.h>
#import "SpaceShapesCreator.h"
#include "FDVFile.h"


void grabThreeDImage(struct zPixel* tv, struct TVSize size, int imageIndex);

void createMovieToAFile2(char* str);
void createMovieToAFile3();


#endif /* MovieFrameGrabber_h */

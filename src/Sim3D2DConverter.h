//
//  Sim3D2DConverter.h
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

#ifndef Sim3D2DConverter_h
#define Sim3D2DConverter_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include "SimDefs.h"


void convertDrawing( struct zShape* shape,  struct drawing* drawing , float xAng, float yAng, float zAng);

void initDrawingWithShape ( struct drawing* d, struct zShape* shape);
void convertPoints (struct zPoint* zp, SDL_Point* p, float xAng, float yAng, float zAng);
void convertTV(TVSize size,SDL_Point* pts,float totalPixels, float xAng, float yAng, float zAng);

#endif /* Sim3D2DConverter_h */

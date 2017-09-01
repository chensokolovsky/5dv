//
//  SimShapeCreator.h
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 7/9/16.
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

#ifndef SimShapeCreator_h
#define SimShapeCreator_h

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "SimDefs.h"

void createCenteredCube(struct zShape* s, float corner1, float corner2);
void createCube(struct zShape* s, zLine diagnal);


#endif /* SimShapeCreator_h */

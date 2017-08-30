//
//  main.c
//  5dv
//
//  Created by Chen Sokolovsky on 8/27/17.
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

#include <stdio.h>
#include <SDL2/SDL.h>
#include "SimDefs.h"

#include "MovieFrameGrabber.h"

#include "Sim3D2DConverter.h"
#include "SimShapeCreator.h"
#include "EncDecZigZag.h"


/// This function prints how to use this executable
void printUsage() {
    
    printf("Usage: use one of these options\n");
    printf("1) 5dv enc [path to an uncompressed 5dvu file] [5dv path to save the compressed file]\n");
    printf("2) 5dv dec [path to a 5dv file] [path to save the uncompressed file]\n");
    printf("3) 5dv create [type] [destination path for the 5dvu file]. available types :spheres\n");
    printf("4) 5dv display [element 1] [optionl:element 2]. These must be uncompressed 5dvu files\n");
    printf("5) 5dv full [folder name]. Runs the full process: create, encode, decode, shhow. All files will get created in that folder. Make sure the folder exists and empty\n");
  
}

//sets up SDL and displays one or two uncompressed files (5dvu)
int displayFiles(int totalFiles,char* firstFilePath, char* secondFilePath) {
    
    // SDL window location on the screen
    int posX = 100;
    int posY = 100;
    int w = 400;
    int h = 300;
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    // Initialize SDL & create window
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow( "Server", posX, posY, w, h, 0 );
    
    // Create and init the renderer
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    
    float xAng = 0;
    float yAng = 0;
    float zAng = 0;
    
    
    // Open the new lossy file
    FILE* file;
    fiveDVUFileHeader header;
    openFiveDVUFile(firstFilePath, &header, &file);
    
    FILE* file2 = NULL;
    fiveDVUFileHeader header2;
    if (totalFiles == 2) openFiveDVUFile(secondFilePath, &header2, &file2);
    
    float totalTVPixels = header.tvsize.width * header.tvsize.depth * header.tvsize.height;
    
    // grab the first image from the movie
    int imageIndex = 0;
    
    // create 2d points for simulator
    SDL_Point* points = (SDL_Point*)malloc(totalTVPixels * sizeof(SDL_Point));
    
    SDL_Point* points2 = NULL;
    if (totalFiles == 2) points2 = (SDL_Point*)malloc(totalTVPixels * sizeof(SDL_Point));
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // converts pixels location from 3d to 2d. This action will be needed only when changing simulator angles
    convertTV(header.tvsize ,points,totalTVPixels,xAng,yAng,zAng);
    
    if (totalFiles == 2) convertTV(header.tvsize,points2,totalTVPixels,xAng,yAng,zAng);
    
    SDL_Event event;
    int quit = 0;
    
    zPixel* buffer = (zPixel*)malloc(totalTVPixels * sizeof(zPixel));
    zPixel* buffer2 = NULL;
    if (totalFiles == 2) buffer2 = (zPixel*)malloc(totalTVPixels * sizeof(zPixel));
    
    // read the first image
    readFrameFromFiveDVUFile(header.tvsize, buffer, imageIndex, &file);
    if (totalFiles == 2) readFrameFromFiveDVUFile(header.tvsize, buffer2, imageIndex, &file2);
    
    printf("reading image:");

    
    // Loop until an SDL_QUIT event is found
    while( !quit ){
        
        // Poll for events. SDL_PollEvent() returns 0 when there are no
        // more events on the event queue, our while loop will exit when
        // that occurs.
        while( SDL_PollEvent( &event ) ){
            // We are only worried about SDL_KEYDOWN and SDL_KEYUP events
            switch( event.type ){
                case SDL_KEYDOWN:
                    switch( event.key.keysym.sym ){
                        case SDLK_LEFT:
                            printf("left,");
                            yAng--;
                            break;
                        case SDLK_RIGHT:
                            printf("right,");
                            yAng++;
                            break;
                        case SDLK_UP:
                            printf("up,");
                            xAng++;
                            break;
                        case SDLK_DOWN:
                            printf("down,");
                            xAng--;
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP :
                    
                    break;
                    
                    // SDL_QUIT event (window close)
                case SDL_QUIT:
                    quit = 1;
                    break;
                    
                    
                default:
                    break;
            }
        }
        
        printf("%d,",imageIndex);
        
        // convert new 2d pixels locations
        convertTV(header.tvsize ,points,totalTVPixels,xAng,yAng,zAng);
        
        if (totalFiles == 2) convertTV(header.tvsize,points2,totalTVPixels,xAng,yAng,zAng);
        
        // clear the screen
        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear( renderer );
        
        // draw each pixel
        for (int i=0; i< totalTVPixels; i++) {
            SDL_Point* twodpoint = &(points[i]);
            zPixel* pixel = &(buffer[i]);
            SDL_SetRenderDrawColor(renderer, pixel->R,pixel->G,pixel->B,pixel->alpha);
            SDL_RenderDrawPoint(renderer,twodpoint->x + w * 0.65,twodpoint->y + h*0.5);
        }
        // draw each pixel
        if (totalFiles == 2) {
            for (int i=0; i< totalTVPixels; i++) {
                SDL_Point* twodpoint = &(points2[i]);
                zPixel* pixel = &(buffer2[i]);
                SDL_SetRenderDrawColor(renderer, pixel->R,pixel->G,pixel->B,pixel->alpha);
                SDL_RenderDrawPoint(renderer,twodpoint->x + w * 0.2,twodpoint->y + h*0.5);
            }
        }
        SDL_RenderPresent( renderer);
        
        // grabThreeDImage(zpixels, tvsize, imageIndex++);
        imageIndex++;
        readFrameFromFiveDVUFile(header.tvsize, buffer, imageIndex, &file);
        if (totalFiles == 2) readFrameFromFiveDVUFile(header.tvsize, buffer2, imageIndex, &file2);
        
        if (imageIndex >= header.totalFrames) {
            imageIndex=0;
            // break;
        }
        
        SDL_Delay(50);
    }
    
    
    fclose(file);
    if (totalFiles == 2) fclose(file2);
    free (points);
    if (totalFiles == 2) free(points2);
    free(buffer);
    if (totalFiles == 2) free(buffer2);
    
    printf("\nquitting\n");
    
    SDL_Quit();

    return 0;
}

// main entry
int main(int argc, const char * argv[])
{
    
    // need 3 arguments at least (first one is the executable name by default, so 2 more)
    if (argc < 3) {
        printUsage();
        return 0;
    }
    
    // check the first argument
    const char* firstArg = argv[1];
  
    /////////////
    /// create
    /////////////
    if (!strcmp(firstArg,"create")) {
        printf("create mode\n");
        
        // check total arguments count
        if (argc != 4) {
            printf("Error! Wrong number of arguments for creation\n");
            printUsage();
            return 0;
        }
        
        // check is the file name is 5dvu
        char* destinationPath = (char*)argv[3];
        char* res = strstr(destinationPath,".5dvu");
        if( res == NULL) {
            printf("Error! destination path must be a 5dvu file name\n");
            printUsage();
            return 0;
        }
        
        const char* secondArg = (char*)argv[2];
        if (!strcmp(secondArg, "spheres")) {
            createMovieToAFile2(destinationPath);
            printf("done\n");
            return 0;
        }
        else {
            printf("Sorry, this type is not supported at the moment\n");
            return 0;
        }
        
        // Run this once the path changes to create the file
        // Create the original uncompressed spheres
        
    }
    
    /////////////
    /// encode
    /////////////
    
     else if (!strcmp(firstArg,"enc")) {
    
         printf("encode mode\n");
         
         // check total arguments count
         if (argc != 4) {
             printf("Error! Wrong number of arguments for encoding\n");
             printUsage();
             return 0;
         }
         
         // check is the source file name is 5dvu
         char* sourcePath = (char*)argv[2];
         char* isfdvu = strstr(sourcePath,".5dvu");
         if( isfdvu == NULL) {
             printf("Error! destination path must be a 5dvu file name\n");
             printUsage();
             return 0;
         }

         
         // check is the file souce name is 5dv
         char* destinationPath = (char*)argv[3];
         isfdvu = strstr(destinationPath,".5dvu");
         char* isfdv = strstr(destinationPath,".5dv");

         if( isfdvu != NULL) {
             printf("Error! destination path must be a 5dv file name\n");
             printUsage();
             return 0;
         }
         else if (isfdv == NULL) {
             printf("Error! destination path must be a 5dv file name\n");
             printUsage();
             return 0;
         }
        
        // compress
        convert(sourcePath,destinationPath,1);
         
         printf("done.\n");
         return 0;
     }
    
    /////////////
    /// decode
    /////////////
    
    else if (!strcmp(firstArg,"dec")) {
    
        printf("decode mode\n");
        
        // check total arguments count
        if (argc != 4) {
            printf("Error! Wrong number of arguments for decoding\n");
            printUsage();
            return 0;
        }
        
        // check is the source file name is 5dvu
        char* sourcePath = (char*)argv[2];
        
        char* isfdvu = strstr(sourcePath,".5dvu");
        char* isfdv = strstr(sourcePath,".5dv");
        
        if( isfdvu != NULL || isfdv == NULL) {
            printf("Error! source path must be a 5dv file name\n");
            printUsage();
            return 0;
        }
        
        
        
        
        // check is the file destination name is 5dv
        char* destinationPath = (char*)argv[3];
        isfdvu = strstr(destinationPath,".5dvu");

        
        if( isfdvu == NULL) {
            printf("Error! destination path must be a 5dv file name\n");
            printUsage();
            return 0;
        }
        
        convert (sourcePath, destinationPath,0);
        
    }
    
    /////////////
    /// display
    /////////////
    
    else if (!strcmp(firstArg,"display")) {
        
        printf("display mode\n");
        
        // check total arguments count
        if (argc > 4) {
            printf("Error! Wrong number of arguments for display\n");
            printUsage();
            return 0;
        }
        
        int totalFiles = argc - 2;

        char* firstFilePath = (char*)argv[2];
        char* secondFilePath = NULL;
        if (totalFiles > 1) secondFilePath = (char*)argv[3];
        
        displayFiles(totalFiles,firstFilePath,secondFilePath);
        
        printf("done.\n");
        return 0;
        
    }
    
    /////////////
    /// full
    /////////////
    
    else if (!strcmp(firstArg,"full")) {
        
        printf("full mode\n");
        
        // check total arguments count
        if (argc > 3) {
            printf("Error! Wrong number of arguments for full run\n");
            printUsage();
            return 0;
        }

        char* folder = (char*)argv[2];
        char* uncompressedOriginalSpheresPath;
        if((uncompressedOriginalSpheresPath = malloc(strlen(folder)+strlen("/originalSpheres.5dvu")+1)) != NULL){
            uncompressedOriginalSpheresPath[0] = '\0';   // ensures the memory is an empty string
            strcat(uncompressedOriginalSpheresPath,folder);
            strcat(uncompressedOriginalSpheresPath,"/originalSpheres.d5vu");
        }
        
        char* compressedFilePath;
        if((compressedFilePath = malloc(strlen(folder)+strlen("/encoded.5dv")+1)) != NULL){
            compressedFilePath[0] = '\0';   // ensures the memory is an empty string
            strcat(compressedFilePath,folder);
            strcat(compressedFilePath,"/encoded.5dv");
        }
        
        char* decompressedFilePath;
        if((decompressedFilePath = malloc(strlen(folder)+strlen("/decoded.5dvu")+1)) != NULL){
            decompressedFilePath[0] = '\0';   // ensures the memory is an empty string
            strcat(decompressedFilePath,folder);
            strcat(decompressedFilePath,"/decoded.5dvu");
        }

        
        printf("creating spheres...");
        // create spheres
        createMovieToAFile2(uncompressedOriginalSpheresPath);
        printf("done.\n");

        // enc
        convert(uncompressedOriginalSpheresPath,compressedFilePath,1);
  
        // dec
        convert(compressedFilePath,decompressedFilePath,0);
  
        //show the spheres
         printf("displaying...\n");
        displayFiles(2, uncompressedOriginalSpheresPath, decompressedFilePath);
    }
    
    
    
    
    
    
    
}

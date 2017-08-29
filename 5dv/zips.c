//
//  main.c
//  zipTest1
//
//  Created by Chen Sokolovsky on 6/21/17.
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


#import "zips.h"



char* concat(const char *s1, const char *s2)
{
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int compressZip(unsigned char* inputBuffer, unsigned int inSize, unsigned char** outputBuffer, unsigned int* outSize, char* randomNameOut) {
    
    
    srand((unsigned)time(NULL));
    
    // write to a temp file
    char randomName[randomNameLength+1]; //48-58 65
    for (int k=0;k<8;k++) {
        int randVal = rand() % 16;
        if (randVal < 10) randVal += 48;
        else randVal += 55;
        randomName[k] = randVal;
    }
   
    if (debugZips) printf("rand name is : %s\n",randomName); else printf("|");
    for (int k=0;k<randomNameLength;k++) randomNameOut[k] = randomName[k];
    
    FILE* tempFullFile = fopen(concat(randomName, ".txt"), "w+");
    size_t ret = fwrite(inputBuffer, 1, inSize, tempFullFile);
     if (debugZips)printf("wrote %zu out of %d\n",ret,inSize);
    fclose(tempFullFile);
    
    
    // zip the file
    char str[] = "zip ";
    char* str2 = concat(str, randomName);
    char* str3 = concat(str2, ".zip ");
    char* str4 = concat(str3, randomName);
    system(concat(str4,".txt > /dev/null"));
 
    
    
    // read the zipped file to the buffer
    FILE* tempZippedFile = fopen(concat(randomName, ".zip"),"r+b");
    fseek(tempZippedFile, 0L, SEEK_END);
    long sz = ftell(tempZippedFile);
    rewind(tempZippedFile);
    
    *outputBuffer = (unsigned char*)calloc(sz,1);
    *outSize = (unsigned int)sz;
    
    size_t read = fread(*outputBuffer, 1, sz, tempZippedFile);
     if (debugZips)printf("read %zu out of %ld\n",read,sz);
    fclose(tempZippedFile);
    
    
    // delete the temp file and the zipped file
    system(concat("rm ",concat(randomName, ".txt")));
    system(concat("rm ",concat(randomName, ".zip")));
 
    return 1;
}

int expandZip(unsigned char* inputBuffer, unsigned int inSize, char* randomNameIn ,unsigned char** outputBuffer, unsigned int* outSize) {
    
    
    srand((unsigned)time(NULL));
    
    // write to a temp zip file
    char randomName[13]; //48-58 65
    for (int k=0;k<8;k++) {
        int randVal = rand() % 16;
        if (randVal < 10) randVal += 48;
        else randVal += 55;
        randomName[k] = randVal;
    }
    
    if (debugZips) printf("rand name is : %s\n",randomName); else printf("|");
    
    FILE* tempZippedFile = fopen(concat(randomName, ".zip"), "w+");
    size_t ret = fwrite(inputBuffer, 1, inSize, tempZippedFile);
     if (debugZips) printf("wrote %zu out of %d\n",ret,inSize);
    fclose(tempZippedFile);
    
    
    // zip the file
    char str[] = "unzip ";
    char* str2 = concat(str, randomName);
 //   char* str3 = concat(str2, ".zip ");
  //  char* str4 = concat(str3, " -p > ");
  //  char* str5 = concat(str4, randomName);
    system(concat(str2,".zip > /dev/null"));
    
    
    
    // read the zipped file to the buffer
    FILE* tempFullFile = fopen(concat(randomNameIn, ".txt"),"r+b");
    fseek(tempFullFile, 0L, SEEK_END);
    long sz = ftell(tempFullFile);
    rewind(tempFullFile);
    
    *outputBuffer = (unsigned char*)calloc(sz,1);
    *outSize = (unsigned int)sz;
    
    size_t read = fread(*outputBuffer, 1, sz, tempFullFile);
     if (debugZips)printf("read %zu out of %ld\n",read,sz);
    fclose(tempFullFile);
    
    
    // delete the temp file and the zipped file
    system(concat("rm ",concat(randomNameIn, ".txt")));
    system(concat("rm ",concat(randomName, ".zip")));
    
    return 1;
}

/*
int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, zip test!\n");
    
    unsigned char* buffer = (unsigned char*)malloc(1000);
    for (int k=0;k<1000;k++) buffer[k]='a';
    
    unsigned char* compressedBuffer;
    unsigned int compressedBufferLength;
    
    compress(buffer, 1000, &compressedBuffer, &compressedBufferLength);
    
    unsigned char* expandedBuffer;
    unsigned int exandedLength;
    
    expand(compressedBuffer,compressedBufferLength,&expandedBuffer,&exandedLength);
    
    
    return 0;
}
 */

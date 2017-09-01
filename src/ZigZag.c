//
//  ZigZag.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 6/20/17.
//  Copyright © 2017 chen. All rights reserved.
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


#import "ZigZag.h"


int numbersHighestDigit(int number) {
    
    if (number < 10) return number;
    if (number < 100) {
        int firstDigit = number / 10;
        int secondDigit = number - (firstDigit * 10);
        return firstDigit > secondDigit ? firstDigit: secondDigit;
    }
    
    int firstDigit = number / 100;
    int lastTwoDigits = number - (firstDigit * 100);
    int secondDigit = lastTwoDigits / 10;
    int thirdDigit = lastTwoDigits - (secondDigit * 10);
    
    int highestOfFirstTwo = firstDigit > secondDigit ? firstDigit : secondDigit;
    return highestOfFirstTwo > thirdDigit ? highestOfFirstTwo : thirdDigit;
    
}

// digit must be between 1 to 9
int doesNumberContainDigit(int number, int digit) {
    
    if (number < 10) return number == digit;
    if (number < 100) {
        int firstDigit = number / 10;
        if (firstDigit == digit) return 1;
        int secondDigit = number - (firstDigit * 10);
        if (secondDigit == digit) return 1;
        return 0;
    }
    int firstDigit = number / 100;
    if (firstDigit == digit) return 1;
    int lastTwoDigits = number - (firstDigit * 100);
    int secondDigit = lastTwoDigits / 10;
    if (secondDigit == digit) return 1;
    int thirdDigit = lastTwoDigits - (secondDigit * 10);
    if (thirdDigit == digit) return 1;
    return 0;
    
}

int createCombinationsThatIncludeDigit(int digit, short int** resultBuffer, int* bufferLength) {
    
    // create a temp buffer long enough (way too long). when done we will copy it to the result buffer
    // We will allocate the result buffer according to it's size
    int maximumCombinations = digit * 100 + digit * 10 + digit;
    short int* tempWritingBuffer = (short int*)calloc(maximumCombinations,sizeof(short int));
    
    /// this will grow as we add combinations
    int actualCombinations = 0;
    
    for (int i=0;i<maximumCombinations+1;i++) {
        
        int containsThisDigit = doesNumberContainDigit(i, digit);
        int heightesDigit = numbersHighestDigit(i);
        if (containsThisDigit && heightesDigit == digit) {
            tempWritingBuffer[actualCombinations] = i;
            actualCombinations++;
        }
    }
    
    *resultBuffer = (short int*)calloc(actualCombinations,sizeof(short int));
    memcpy(*resultBuffer,tempWritingBuffer,actualCombinations*sizeof(short int));
    *bufferLength = actualCombinations;
    
    free(tempWritingBuffer);
    
    return 1;
}

int generateCombinationMatrix(short int* resultMatrix, int heighestDigit, int* totalLength) {
    
    short int* writingPtr = resultMatrix;
    
    resultMatrix[0] = 0;
    writingPtr++;
    int length = 1;
    
    for (int k=0;k<heighestDigit;k++) {
        
        short int* thisDigitBuffer;
        int bufferLength = 0;
        createCombinationsThatIncludeDigit(k+1,&thisDigitBuffer,&bufferLength);
        memcpy(writingPtr,thisDigitBuffer,bufferLength * sizeof(short int));
        free (thisDigitBuffer);
        thisDigitBuffer = 0;
        writingPtr += bufferLength;
        length += bufferLength;
        
    }
    
    *totalLength = length;
    
    
    return 1;
}

int convertLocationToIndex (short int location) {
    
    if (location < 10) return location * macroBlockDimension * macroBlockDimension;
    if (location < 100) {
        int firstDigit = location / 10;
        int secondDigit = location % 10;
        return (firstDigit * macroBlockDimension + secondDigit * macroBlockDimension * macroBlockDimension);
    }
    int firstDigit = location / 100;
    int otherTwoDigits = location % 100;
    int seconndDigit = otherTwoDigits / 10;
    int lastDigit = otherTwoDigits % 10;
    return (firstDigit + seconndDigit * macroBlockDimension + lastDigit * macroBlockDimension *macroBlockDimension);
}

// This method returns an array of 8x8x8 values which are the order of a zig zag cube.
// when then read/write from/to the writing buffer according to this order
// For example, 0,9,65,1,10,.... meaning we write element #9 to index #2 of the writing buffer (or opposite when reading)
int generateZigZagMatrix(short int* resultMatrix) {
    
    
    int debugZigZagCreation = 0;
    if (debugZigZagCreation) printf("zigZag createion started");
    
    int length = 0;
    generateCombinationMatrix(resultMatrix,7,&length);
    
    for (int i = 0;i<length ;i++) {
        int val = resultMatrix[i];
        for (int k=i+1;k<length;k++) {
            if (val == resultMatrix[k]) {
                printf("\n error. value %d appears at index %d and %d\n",val,i,k);
            }
        }
    }
    
    if (debugZigZagCreation) {
        printf("\nresult:%d\n",length);
        for (int k=0;k<length;k++) {
            printf("%d,",resultMatrix[k]);
        }
        printf("\n\n");
    }
    
    // we now need to convert digits to locations
    // for example: 523 means 5,2,3 meaning w=5,d=2,h=3
    // this location is w + d*N + h * N*N;
    
    for (int k=0;k<length;k++) {
        resultMatrix[k] = convertLocationToIndex(resultMatrix[k]);
    }
    
    if (debugZigZagCreation) {
        for (int i = 0;i<length ;i++) {
            int val = resultMatrix[i];
            for (int k=i+1;k<length;k++) {
                if (val == resultMatrix[k]) {
                    printf("\n error. value %d appears at index %d and %d\n",val,i,k);
                }
            }
        }
        
        int highestVal = 0;
        printf("\nfinal result:%d\n",length);
        
        for (int g=0;g<length;g++) {
            if (resultMatrix[g] > highestVal) highestVal = resultMatrix[g];
            printf("%d,",resultMatrix[g]);
            
        }
        printf("\n highest val: %d\n", highestVal);
        
    }
    
    
    return 1;
}

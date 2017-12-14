
//  fastDct.c
//  chenSDLtest1
//
//  Created by Chen Sokolovsky on 6/19/17.
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

#include "fastDct.h"

// in comparison to the slower method, we took the c[8] and it is now called factor. We do the muliplication here in advance once instead of when running the dct
int initFastDCTTwo(float* c) {
    
    float factor = sqrt(.25);
    
    c[0] = factor * cos(M_PI * 0.0625);
    c[1] = factor * cos(M_PI * 0.125);
    c[2] = factor * cos(M_PI * 0.1875);
    c[3] = factor * cos(M_PI * 0.25);
    c[4] = factor * cos(M_PI * 0.3125);
    c[5] = factor * cos(M_PI * 0.375);
    c[6] = factor * cos(M_PI * 0.4375);
    
    // two consts as scalars
    c[7] = sqrt(.125); // for k0
    c[9] = sqrt(.5) * factor; // for idct
    
    return 1;
}

// the faster method (Two) is not allocating new q floats, also, the c[8] is multipied inside the c factors already
int fastDctTwo(float* n,float* o,float* c, float* q) {
    // the q's
    q[0] = n[0]-n[7];
    q[1] = n[1]-n[6];
    q[2] = n[2]-n[5];
    q[3] = n[3]-n[4];
    
    q[4] = n[0]+n[7];
    q[5] = n[1]+n[6];
    q[6] = n[2]+n[5];
    q[7] = n[3]+n[4];
    
    q[8] = q[4]+q[7];
    q[9] = q[5]+q[6];
    q[10] = q[4]-q[7];
    q[11] = q[5]-q[6];
    
    o[0] = c[7]*(q[8]+q[9]);
    o[1] = c[0]*q[0]+c[2]*q[1]+c[4]*q[2]+c[6]*q[3];
    o[2] = c[1]*q[10]+c[5]*q[11];
    o[3] = c[2]*q[0]-c[6]*q[1]-c[0]*q[2]-c[4]*q[3];
    o[4] = c[3]*(q[8]-q[9]);
    o[5] = c[4]*q[0]-c[0]*q[1]+c[6]*q[2]+c[2]*q[3];
    o[6] = c[5]*q[10]-c[1]*q[11];
    o[7] = c[6]*q[0]-c[4]*q[1]+c[2]*q[2]-c[0]*q[3];
    
    return 1;
}

// see forward dct TWO notes above
int fastIDctTwo(float* n,float* o,float* c, float* q) {
    
    // the q's
    q[0] = n[2]*c[1] + n[6]*c[5];
    q[1] = n[2]*c[5] - n[6]*c[1];
    q[2] = n[1]*c[0]+n[3]*c[2]+n[5]*c[4]+n[7]*c[6];
    q[3] = n[1]*c[6]-n[3]*c[4]+n[5]*c[2]-n[7]*c[0];
    
    q[4] = n[0]*c[9]+n[4]*c[3];
    q[5] = n[0]*c[9]-n[4]*c[3];
    
    q[6] = n[1]*c[2]-n[3]*c[6]-n[5]*c[0]-n[7]*c[4];
    q[7] = n[1]*c[4]-n[3]*c[0]+n[5]*c[6]+n[7]*c[2];
    
    q[8] = q[5] + q[1];
    q[9] = q[5] - q[1];
    q[10] = q[4] + q[0];
    q[11] = q[4] - q[0];
    
    o[0] = q[10]+ q[2];
    o[1] = q[8] + q[6];
    o[2] = q[9] + q[7];
    o[3] = q[11] + q[3];
    o[4] = q[11] - q[3];
    o[5] = q[9] - q[7];
    o[6] = q[8] - q[6];
    o[7] = q[10] - q[2];
    
    return 1;
}

// not used. We now use the faster method called initFastDCTTwo
int initFastDCT(float* c) {
    
    c[0] = cos(M_PI * 0.0625);
    c[1] = cos(M_PI * 0.125);
    c[2] = cos(M_PI * 0.1875);
    c[3] = cos(M_PI * 0.25);
    c[4] = cos(M_PI * 0.3125);
    c[5] = cos(M_PI * 0.375);
    c[6] = cos(M_PI * 0.4375);
    
    // two consts as scalars
    c[7] = sqrt(.125); // for k0
    c[8] = sqrt(.25); // for the rest
    c[9] = sqrt(.5); // for idct
    
    return 1;
}

// not used. We now use the faster method called fastDCTTwo
int fastDct(float* n,float* o,float* c) {
    
    
    // the q's
    float q1 = n[0]-n[7];
    float q2 = n[1]-n[6];
    float q3 = n[2]-n[5];
    float q4 = n[3]-n[4];
    
    float q5 = n[0]+n[7];
    float q6 = n[1]+n[6];
    float q7 = n[2]+n[5];
    float q8 = n[3]+n[4];
    
    float q9 = q5+q8;
    float q10 = q6+q7;
    float q11 = q5-q8;
    float q12 = q6-q7;
    
    o[0] = c[7]*(q9+q10);
    o[1] = c[8]*(c[0]*q1+c[2]*q2+c[4]*q3+c[6]*q4);
    o[2] = c[8]*(c[1]*q11+c[5]*q12);
    o[3] = c[8]*(c[2]*q1-c[6]*q2-c[0]*q3-c[4]*q4);
    o[4] = c[8]*c[3]*(q9-q10);
    o[5] = c[8]*(c[4]*q1-c[0]*q2+c[6]*q3+c[2]*q4);
    o[6] = c[8]*(c[5]*q11-c[1]*q12);
    o[7] = c[8]*(c[6]*q1-c[4]*q2+c[2]*q3-c[0]*q4);
    
    return 1;
}

// not used. We now use the faster method called fastIDCTTwo
int fastIDct(float* n,float* o,float* c) {
    
    // the q's
    float q1 = n[2]*c[1] + n[6]*c[5];
    float q2 = n[2]*c[5] - n[6]*c[1];
    float q3 = n[1]*c[0]+n[3]*c[2]+n[5]*c[4]+n[7]*c[6];
    float q4 = n[1]*c[6]-n[3]*c[4]+n[5]*c[2]-n[7]*c[0];
    
    float q5 = n[0]*c[9]+n[4]*c[3];
    float q6 = n[0]*c[9]-n[4]*c[3];;
   
    float q7 = n[1]*c[2]-n[3]*c[6]-n[5]*c[0]-n[7]*c[4];
    float q8 = n[1]*c[4]-n[3]*c[0]+n[5]*c[6]+n[7]*c[2];
    
    float q9 = q6 + q2;
    float q10 = q6 - q2;
    float q11 = q5 + q1;
    float q12 = q5 - q1;
    
    o[0] = c[8] * (q11+ q3);
    o[1] = c[8] * (q9 + q7);
    o[2] = c[8] * (q10 + q8);
    o[3] = c[8] * (q12 + q4);
    o[4] = c[8] * (q12 - q4);
    o[5] = c[8] * (q10 - q8);
    o[6] = c[8] * (q9 - q7);
    o[7] = c[8] * (q11 - q3);
    
    return 1;
}


int fast2DDct(float* in, float* out,float* c) {
    
    float temp[N*N];
    float col[N];
    float res[N];
    float q[N+4];
    
    for (int m=0;m<N;m++) {
        fastDctTwo(&(in[m*N]), &temp[m*N],c,q);
    }
    for (int b=0;b<N;b++) {
        for (int r=0;r<N;r++) col[r] = temp[r*N+b];
        fastDctTwo(col, res,c,q);
        for (int j=0;j<N;j++) out[j*N+b]= res[j];
    }
    
    
    return 1;
}

int fast2DIDct(float* in, float* out, float* c) {
    float temp[N*N];
    float col[N];
    float res[N];
    float q[N+4];
    
    for (int m=0;m<N;m++) {
        fastIDctTwo(&(in[m*N]), &temp[m*N],c,q);
    }
    
    for (int b=0;b<N;b++) {
        for (int r=0;r<N;r++) col[r] = temp[r*N+b];
        fastIDctTwo(col, res,c,q);
        for (int j=0;j<N;j++) out[j*N+b]= res[j];
    }
    
    
    return 1;
}


int fast3DDct(float* in, float* out,float* c) {
    
    float temp[N*N*N];
    float pole[N];
    float res[N];
    float q[N+4];
    
    for (int m=0;m<N;m++) {
        fast2DDct(&(in[m*N*N]), &temp[m*N*N],c);
    }

    int factors[N];
    int factorsS[N];
    for (int h=0; h<N;h++) {factors[h] = h*N*N; factorsS[h] = h*N;}
    for (int w=0;w<N;w++) {
        for (int d=0;d<N;d++) {
            for (int k=0;k<N;k++) pole[k] = temp[factorsS[w]+d+factors[k]];
            fastDctTwo(pole, res, c,q);
            for (int l=0;l<N;l++) out[factorsS[w]+d+factors[l]] = res[l];
        }
    }
    
    return 1;
}

int fast3DIDct(float* in, float* out,float* c) {
    
    float temp[N*N*N];
    float pole[N];
    float res[N];
    float q[N+4];
    
    for (int m=0;m<N;m++) {
        fast2DIDct(&(in[m*N*N]), &temp[m*N*N],c);
    }
    
    int factors[N];
    int factorsS[N];
    for (int h=0; h<N;h++) {factors[h] = h*N*N; factorsS[h] = h*N;}
    for (int w=0;w<N;w++) {
        for (int d=0;d<N;d++) {

            for (int k=0;k<N;k++) pole[k] = temp[factorsS[w]+d+factors[k]];
            fastIDctTwo(pole, res, c,q);
            for (int l=0;l<N;l++) out[factorsS[w]+d+factors[l]] = res[l];
        }
    }
    
    return 1;
}

// test the result
int compareBuffers(float* b1, float* b2, int size) {
    for (int k=0; k < size; k++) {
        int a1 = b1[k] * 1000;
        int a2 = b2[k] * 1000;
        if (a1 != a2) {
            int diff  = a1-a2;
            if (abs(diff) > 2) return -1;
        }
    }
    return 0;
}

	/*
	
	thisis the result of the coeefs
	we can now look at coeffs repetitiins
	1.000,0.981,0.924,0.831,0.707,0.556,0.383,0.195,
     1,a,b,c,d,e,f,g,
     1,c,f,-g,-d,-a,-b,-e,
     1,e,-f,-a,-d,g,b,c,
     1,g,-b,-e,d,c,-f,-a,
     1,-g,-b,e,d,-c,-f,a,
     1,-e,-f,a,-d,-g,b,-c,
     1,-c,f,g,-d,a,-b,e,
     1,-a,b,-c,d,-e,f,-g,
	
	
	k[0] = n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7
	k[1] = a(n0-n7) + c(n1-n6) + e(n2-n5) + g(n3-n4)
	k[2] = b(n0-n3-n4+n7) + f(n1-n2-n5+n6)
	k[3] = c(n0-n7) -g(n1-n6) -a(n2-n5) -e(n3-n4)
	k[4] = d(n0-n1-n2+n3+n4-n5-n6+n7)
	k[5] = e(n0-n7)-a(n1-n6)+g(n2-n5)+c(n3-n4)
	k[6] = f(n0-n3-n4+n7) -b(n1 -n2 -n5 +n6)
	k[7] = g(n0-n7) -e(n1-n6) +c(n2-n5) -a(n3-n4)
	
	-------------------------'
	
	We can define some constatns q:
     
     n0-n7 q1
     n1-n6 q2
     n2-n5 q3
     n3-n4 q4
     
     n0+n7 q5
     n1+n6 q6
     n2+n5 q7
     n3+n4 q8
     
     q5+q8 q9 (n0+n7+n3+n4)
     q6+q7 q10 (n1+n6+n2+n5)
     q5-q8 q11 (n0+n7-n3-n4)
     q6-q7 q12 (n1+n6-n2-n5)
     
     which makes up these equations:
     
     k[0] = const(q9+q10)
     k[1] = aq1+cq2+eq3+gq4
     k[2] = bq11+fq12
     k[3] = cq1-gq2-aq3-eq4
     k[4] = d(q9-q10)
     k[5] = eq1-aq2+gq3+cq4
     k[6] = fq11-bq12
     k[7] = gq1-eq2+cq3-aq4
     
     
     The cosine and 2 constants will only need to be calculated once for all fast dct.
     The last parameter will be the initted constants
     Another constant for idct making the constant buffers N + 2 (7 coeffs + 3 constants)
     
     
     *********************************
     
     The idct is a bit different
     
     the table of coeffs is this:
     1,1,1,1,1,1,1,1
     a,c,e,g,-g,-e,-c,-a
     b,f,-f,-b,-b,-f,-f,b
     c,-g,-a,-e,e,a,g,-c
     d,-d,-d,d,d,-d,-d,d
     e,-a,g,c,-c,-g,a,-e
     f,-b,f,-f,-f,b,-b,f
     g,-e,c,-a,a,-c,e,-g
     
     
     So the full formulas are (k0 is always const * k0) and the entire sum of each row also is doubled with a constant
	 n[0] = k0 + k1a + k2b + k3c + k4d + k5e + k6f + k7g
     n[1] = k0 + k1c + k2f - k3g -k4d -k5a - k6b - k7e
     n[2] = k0 + k1e = k2f = k3a = k4d + k5g + k6b + k7c
     n[3] = k0 + k1g - k2b - k3e + k4d +k5c - k6f - k7a
     n[4] = k0 - k1g - k2b _ k3e _ k4d - k5c - k6f _ k7a
     n[5] = k0 - k1e - k2f + k3a - k4d - k5g + kgb - k7c
     n[6] = k0 - k1c + k2f + k3g - k4d + k5a - k6b + k7e
     n[7] = k0 - k1a + k2b - k3c + k4d - k5e + k6f - k7g
     
     We can define some q's
      
     q1 = k2b + k6f
     q2 = k2f - k6b
     q3 = k1a + k3c + k5e + k7g
     q4 = k1g - k3e + k5c - k7a
     q5 = (const*)k0 + k4d
     q6 = (const*)k0 - k4d
     
     q7 = k1c - k3g - k5a - k7e
     q8 = k1e - k3a + k5g + k7c
     
     float q9 = q6 + q2;
     float q10 = q6 - q2;
     float q11 = q5 + q1;
     float q12 = q5 - q1;
     
     So final equations are (times constant)

     n[0] = q11+ q3;
     n[1] = q9 + q7;
     n[2] = q10 + q8;
     n[3] = q12 + q4;
     n[4] = q12 - q4;
     n[5] = q10 - q8;
     n[6] = q9 - q7;
     n[7] = q11 - q3;

     
     
    
     
	*/


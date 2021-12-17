#include <iostream>
#include <math.h>
#include "encoder.hpp"
#include "decoder.hpp"
using namespace std;

static float DCTmatrix[8][8];
static float I_DCTmatrix[8][8];

void DCTbackTransform(int16_t data[], float output[], uint32_t w, uint32_t h){
    // T(inverse)X
    float* v_output = new float[w*h];
    int block_x = w/8;
    int block_y = h/8;
    for(int i=0;i<block_y;i++){
        for(int j=0;j<block_x;j++){
            for(int k=0;k<8;k++){
                for(int l=0;l<8;l++){
                    v_output[i*8*w+j*8+k*w+l]=0;
                    for(int m=0;m<8;m++){
                        v_output[i*8*w+j*8+k*w+l]+= I_DCTmatrix[k][m]*data[i*8*w+j*8+m*w+l];
                    }
                }
            }

        }
    }
    // T(inverse)XT
    for(int i=0;i<block_y;i++){
        for(int j=0;j<block_x;j++){
            for(int k=0;k<8;k++){
                for(int l=0;l<8;l++){
                    output[i*8*w+j*8+k*w+l]=0;
                    for(int m=0;m<8;m++){
                        output[i*8*w+j*8+k*w+l]+= v_output[i*8*w+j*8+k*w+m]*DCTmatrix[m][l];
                    }
                }
            }

        }
    }
    delete [] v_output;
    return;
}

static void computeDCTMatrix(){
    for(int j=0;j<8;j++){
        DCTmatrix[0][j] = sqrt(0.125);
    }
    for(int i=1;i<8;i++){
        for(int j=0;j<8;j++){
            DCTmatrix[i][j] = sqrt(0.25)*cos((2*j+1)*i*M_PI/16);
        }
    }
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            I_DCTmatrix[i][j]=DCTmatrix[j][i];
        }
    }
        
}

void YuvConvertToRGB(uint8_t R[],uint8_t G[],uint8_t B[],float Y[],float U[],float V[],uint32_t size){
    float r,g,b;
    for(int i=0;i<size;i++){
    U[i]=U[i]-128;
    V[i]=V[i]-128;
    r = Y[i]+V[i];
    g = Y[i]-0.1942*U[i]-0.50937*V[i];
    b = Y[i]+U[i];
    R[i] = (uint8_t)round(r);
    G[i] = (uint8_t)round(g);
    B[i] = (uint8_t)round(b);
    }
}

void decompress(uint8_t R[],uint8_t G[],uint8_t B[], struct IMG* mi){
    uint32_t w = mi->width;
    uint32_t h = mi->height;
    computeDCTMatrix();
    float Y[w*h],U[w*h],V[w*h];
    int16_t data1[w*h],data2[w*h],data3[w*h];
    for(int i=0;i<w*h;i++){
        data1[i]= mi->data[i];
        data2[i]= mi->data[i+w*h];
        data3[i]= mi->data[i+2*w*h];
    }
    DCTbackTransform(data1,Y,w,h);
    DCTbackTransform(data2,U,w,h);
    DCTbackTransform(data3,V,w,h);
    YuvConvertToRGB(R,G,B,Y,U,V,w*h);
    return;
}

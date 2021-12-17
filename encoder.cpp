#include <iostream>
#include <math.h>
#include <fstream>
#include "huffman.hpp"
#include "encoder.hpp"
using namespace std;



static float DCTmatrix[8][8];
static float T_DCTmatrix[8][8];

// impelement midtread quantizer
void quantization(float input[], int16_t output[],uint32_t size){
    for(int i=0;i<size;i++){
        
        if(signbit(input[i])){
            output[i] = -1*(uint16_t)floor(abs(input[i])+0.5);
        }
        else
        {
            output[i] = (uint16_t)floor(abs(input[i])+0.5);    
        }
        
        
    }
}

void DCTtransform(uint8_t data[], float output[], uint32_t w, uint32_t h){
    // TX
    float* h_output = new float[w*h];
    int block_x = w/8;
    int block_y = h/8;
    for(int i=0;i<block_y;i++){
        for(int j=0;j<block_x;j++){
            for(int k=0;k<8;k++){
                for(int l=0;l<8;l++){
                    h_output[i*8*w+j*8+k*w+l]=0;
                    for(int m=0;m<8;m++){
                        h_output[i*8*w+j*8+k*w+l]+= DCTmatrix[k][m]*data[i*8*w+j*8+m*w+l];
                    }
                }
            }
        }
    }

    // TXT(inverse)
    for(int i=0;i<block_y;i++){
        for(int j=0;j<block_x;j++){
            for(int k=0;k<8;k++){
                for(int l=0;l<8;l++){
                    output[i*8*w+j*8+k*w+l]=0;
                    for(int m=0;m<8;m++){
                        output[i*8*w+j*8+k*w+l]+= h_output[i*8*w+j*8+k*w+m]*T_DCTmatrix[m][l];
                    }
                }
            }

        }
    }


    delete [] h_output;

    return;
}
// compute the 8*8 DCT matrix
void computeDCTmatrix(){
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
            T_DCTmatrix[i][j] = DCTmatrix[j][i];
        }
    }
}

void RGBCovertToYUV(uint8_t R[],uint8_t G[],uint8_t B[],uint8_t Y[],uint8_t U[],uint8_t V[],uint32_t size){
    float y,u,v;
    for(int i=0;i<size;i++){
    y = 0.299*R[i]+0.587*G[i]+0.114*B[i];
    u = -0.299*R[i]-0.587*G[i]+0.886*B[i];
    v = 0.701*R[i]-0.587*G[i]-0.114*B[i];
    Y[i] = (uint8_t)round(y);
    U[i] = (uint8_t)(round(u)+128);
    V[i] = (uint8_t)(round(v)+128);
    }
    return;
}


void compressImg(uint8_t R[],uint8_t G[],uint8_t B[],uint32_t w, uint32_t h,struct IMG* mi){
    uint8_t Y[w*h],U[w*h],V[w*h];
    float f_output1[w*h],f_output2[w*h],f_output3[w*h];
    int16_t i_output1[w*h],i_output2[w*h],i_output3[w*h];
    // RGB->YUV
    RGBCovertToYUV(R,G,B,Y,U,V,w*h);
    computeDCTmatrix();
    // do the DCT transform
    DCTtransform(Y,f_output1,w,h);
    DCTtransform(U,f_output2,w,h);
    DCTtransform(V,f_output3,w,h);
    // do the midtread quantization
    quantization(f_output1,i_output1,w*h); 
    quantization(f_output2,i_output2,w*h); 
    quantization(f_output2,i_output2,w*h);
    // save compressed data into an array (Y->U->V)
    int16_t* final_output = new int16_t[3*w*h];
    for(int i=0;i<3;i++){
        for(int j=0;j<w*h;j++){
            if(i==0)
                final_output[i*w*h+j]=i_output1[j];
            if(i==1)   
                final_output[i*w*h+j]=i_output2[j]; 
            if(i==2)
                final_output[i*w*h+j]=i_output3[j];
        }
    } 
    // call the Huffman coding method and compute total bits for compressed Y,U,V plane
    unsigned int bit_y, bit_u, bit_v;
    bit_y = computeHuffBits(i_output1,w*h);
    bit_u = computeHuffBits(i_output2,w*h);
    bit_v = computeHuffBits(i_output3,w*h);
    cout << "Compression ratio for the image is: " << (double)(bit_y+bit_u+bit_v)/(w*h*24)*100 <<"%" << endl;
    mi->data = new int16_t[3*w*h];
    // write the pixel data into IMG format
    for(int i=0;i<3*w*h;i++){
        mi->data[i] = final_output[i];
    }
    delete [] final_output;
    return;
}

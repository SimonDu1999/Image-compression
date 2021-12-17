#include <iostream>
#include <SDL2/SDL.h>
#include <string>
#include <fstream>
#include "encoder.hpp"
#include "decoder.hpp"
using namespace std;


int main(){
    // input the iamge file
    char imagePath[40];
    cout << "Enter you image path: " << endl;
    cin >> imagePath;
    FILE *imgFile = fopen(imagePath,"rb");
    if(imgFile == NULL){
        cout << "Could not open input image file" << endl;
        exit(0);
    }
  
    // store the width, height and the pixels data
    Uint32 dataOffset, width, height;
    Uint8* c_red;
    Uint8* c_green;
    Uint8* c_blue;
    struct IMG myImg;
    // IMG format case:
    bool isIMG = false;
    for(int i = 0;i<40;i++){
        if(imagePath[i]=='.'){
            if(imagePath[i+1]=='I' && imagePath[i+2]=='M' && imagePath[i+3]=='G'){
                fread(&width,sizeof(width),1,imgFile);
                fread(&height,sizeof(height),1,imgFile);
                myImg.data = new int16_t[3*width*height];
                c_red = new uint8_t[width*height];
                c_green = new uint8_t[width*height];
                c_blue = new uint8_t[width*height];
                decompress(c_red,c_green,c_blue,&myImg);
                isIMG = true;
                break;
            }
        }
    }
    // bmp format case:
    if(!isIMG){
    fseek(imgFile,10,SEEK_SET);
    fread(&dataOffset,sizeof(dataOffset),1,imgFile);
    fseek(imgFile,0x12,SEEK_SET);
    fread(&width,sizeof(width),1,imgFile);
    fseek(imgFile,0x16,SEEK_SET);
    fread(&height,sizeof(height),1,imgFile);
    Uint32 size = width*height;
    // initialize a 24-bit array to store all the pixels
    Uint8 pixel[3*size];
    fseek(imgFile,dataOffset,SEEK_SET);
    int n = fread(pixel,3*size,1,imgFile);
    if(n != 1){
        cout << "pixel reading fail: " <<  n << endl;
        exit(0);
    }
    Uint8 red[size];
    Uint8 green[size];
    Uint8 blue[size];
    c_red = new uint8_t[width*height];
    c_green = new uint8_t[width*height];
    c_blue = new uint8_t[width*height];
    for(int i=0;i<size;i++){
        blue[i] = pixel[3*i];
        green[i] = pixel[3*i+1];
        red[i] = pixel[3*i+2];
    }
    fclose(imgFile);
    // compress the image and write it into img format
    myImg.width = width;
    myImg.height = height;
    compressImg(red,green,blue,width,height,&myImg);
    // write the compressed data into a IM3 file
    for(int i=0;i<40;i++){
        if(imagePath[i]=='.'){
            imagePath[i+1]='I';
            imagePath[i+2]='M';
            imagePath[i+3]='3';
        }
    }
    FILE* IM3file = fopen(imagePath,"wb");
    fwrite(&width,sizeof(uint32_t),1,IM3file);
    fwrite(&height,sizeof(uint32_t),1,IM3file);
    fwrite(myImg.data,sizeof(int16_t),sizeof(myImg.data),IM3file);
    fclose(IM3file);
    // decompress the img format
    decompress(c_red,c_green,c_blue,&myImg);
    
    // implement the GUI and display the original image
    if ( SDL_Init( SDL_INIT_EVERYTHING) < 0){
        cout << "SDL could not initialize! SDL error: " << SDL_GetError() << endl;
    }
    SDL_Window *window = SDL_CreateWindow( "Original Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_ALLOW_HIGHDPI );
    SDL_Renderer *renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if ( NULL == window )
    {
        cout << "Could not create window: " << SDL_GetError( ) << endl;
        exit(0);
    }

    for(int i = 0; i < height; i++){
            for(int j = 0; j < width ; j++){
                SDL_SetRenderDrawColor(renderer,red[(height-i)*width+j],green[(height-i)*width+j],blue[(height-i)*width+j],255);
                SDL_RenderDrawPoint(renderer,j,i);
            }
        }
    SDL_RenderPresent(renderer);
    delete [] myImg.data;    
    }
    // show the compressed img
    SDL_Window *window2 = SDL_CreateWindow( "Compressed image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_ALLOW_HIGHDPI );
    SDL_Renderer *renderer2 = SDL_CreateRenderer(window2,-1,SDL_RENDERER_ACCELERATED);
    if ( NULL == window2 )
    {
        cout << "Could not create window: " << SDL_GetError( ) << endl;
        exit(0);
    }

    for(int i = 0; i < height; i++){
            for(int j = 0; j < width ; j++){
                SDL_SetRenderDrawColor(renderer2,c_red[(height-i)*width+j],c_green[(height-i)*width+j],c_blue[(height-i)*width+j],255);
                SDL_RenderDrawPoint(renderer2,j,i);
            }
        }
        SDL_RenderPresent(renderer2);
    SDL_Event windowEvent;
    
    while (true)
    {
        if (SDL_PollEvent(&windowEvent))
        {
            if (SDL_WINDOWEVENT == windowEvent.type && windowEvent.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                break;
            }
        }
        
        //Update the surface
        SDL_UpdateWindowSurface(window2);
    }
    

    SDL_DestroyWindow(window2);
    SDL_Quit();

    return 0;
}
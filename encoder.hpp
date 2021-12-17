#ifndef ENCODER_HPP
#define ENCODER_HPP
#include <iostream>

struct IMG{
    uint32_t width;
    uint32_t height;
    int16_t* data;          // data contains compressed Y for each pixel then compressed U then then compressed V
};

void compressImg(uint8_t R[],uint8_t G[],uint8_t B[], uint32_t w, uint32_t h, struct IMG* mi);

#endif
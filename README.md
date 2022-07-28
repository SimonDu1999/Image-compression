# Image-compression
Image-compression is a program that aims at compressing image using DCT and Huffman

## Techniques used: <br />
RGB ->YUV: reduce the input size by only contain the colour diﬀerences <br />
DCT: spatial redundancy remove(lossy) <br />
MidThread quantization: set the each interval of ﬂoating data after DCT to one integer <br />
Huﬀman coding: reduce the bit length of high frequency integer <br />

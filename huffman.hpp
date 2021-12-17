#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

struct Node{
    int16_t *symbol;
    uint16_t freq;                   // frequency
    Node* left;                 // left chid
    Node* right;                // right child
    Node(int16_t *symbol,uint16_t freq){             // constructor
        left = NULL;
        right = NULL;
        this->symbol = symbol;
        this->freq = freq;
    }
};

unsigned int computeHuffBits(int16_t* input, uint32_t size);

#endif
#include <iostream>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <queue>
#include <cstdint>
#include "huffman.hpp"
using namespace std;


// comparison method for priority queue
struct compare {                            
    bool operator()(Node* l, Node* r){
        return (l->freq > r->freq);
    }
};

struct Node* HuffTree(int16_t** numSymbols,long difNumSym){
    struct Node *left, *right, *top;
    priority_queue<Node*, vector<Node*>, compare> data;

    for(int i = 0; i<difNumSym;i++){
            data.push(new Node(&numSymbols[i][0],numSymbols[i][1]));
            
    } 
    while (data.size() != 1){
        left = data.top();
        data.pop();
        right = data.top();
        data.pop();
        // merge left and right nodes frequency and create a new parent to contain it
        top = new Node(NULL,left->freq+right->freq);
        top->left = left;
        top->right = right;
        data.push(top);
    }
    return data.top();
}

unsigned int computeHuffBits(Node* root,int bit){
    if(root -> left == NULL && root -> right == NULL){
        return bit*root->freq;
    }
    // each node is whether a leave node or a node that has both right and left children
    return computeHuffBits(root->left,bit+1)+computeHuffBits(root->right,bit+1);

}

unsigned int computeHuffBits(int16_t* input, uint32_t size){
    long difNumSym = 0;
    //f_sumSymbols[][0] contains the data value, and f_sumSymbols[][1] contains its frequency
    int16_t** f_numSymbols = new int16_t*[size];
    for(int i=0;i<size;++i){
        f_numSymbols[i] = new int16_t[2];
    }
    for(int i = 0; i < size; i++){
        for(int j = 0; j<2; j++){
            f_numSymbols[i][j] = 0;
        }
    }       
    // calculate the frequency of each single symbol
    for(int i = 0; i < size; i++){
        bool found = false;
        for(int j = 0;j < difNumSym;j++){
            if(input[i] == f_numSymbols[j][0]){
                f_numSymbols[j][1]++;
                found = true;
                break;
            }
        }
        if(!found){
            f_numSymbols[difNumSym][0] = input[i];
            f_numSymbols[difNumSym][1]++;
            difNumSym++;
        }
    } 
    // construct the Huffman Tree for single symbols
    struct Node* root = HuffTree(f_numSymbols,difNumSym);
    // use the Huffman Three to compute the total bits after compressing
    unsigned int Huffbits = computeHuffBits(root,0);
    for(int i = 0; i < size; ++i) {
        delete [] f_numSymbols[i];
    }
    delete [] f_numSymbols;
    return Huffbits;
}


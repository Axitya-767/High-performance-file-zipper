#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <map>
#include <queue>
#include <vector>
#include "HuffmanNode.h"

class HuffmanTree {
private:
    HuffmanNode* root; // The Master Node

public:
    // Constructor
    HuffmanTree() { root = nullptr; }

    // The Main Function: Takes the map and builds the tree
    void buildTree(const std::map<char, int>& frequencies);
    
    // Helper to see if it worked (Logic Check)
    HuffmanNode* getRoot() { return root; }
};

#endif
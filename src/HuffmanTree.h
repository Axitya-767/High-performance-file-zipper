#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

#include <map>
#include <string> 
#include <queue>
#include <vector>
#include <iostream> // Added for debug if needed
#include "HuffmanNode.h"

class HuffmanTree {
private:
    HuffmanNode* root;
    std::map<char, std::string> huffmanCodes; // Stores 'a' -> "101"

    // The Recursive "Worker" Function
    void generateCodes(HuffmanNode* node, std::string code);

public:
    HuffmanTree() : root(nullptr) {}
    
    void buildTree(const std::map<char, int>& frequencies);
    
    // Starts the recursion
    void generateHuffmanCodes(); 
    
    // Returns the map of codes for us to print
    std::map<char, std::string> getCodes() { return huffmanCodes; }
    
    HuffmanNode* getRoot() { return root; }
};

#endif
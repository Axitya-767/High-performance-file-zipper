#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

struct HuffmanNode {
    char character;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char ch, int freq) {
        character = ch;
        frequency = freq;
        left = nullptr;
        right = nullptr;
    }
};

// This struct teaches the Priority Queue how to order pointers.
// It acts as a "Judge" for the Min-Heap.
struct Compare {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        // We want the SMALLER frequency to have HIGHER priority.
        return left->frequency > right->frequency;
    }
};

#endif
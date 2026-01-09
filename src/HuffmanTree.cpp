#include "HuffmanTree.h"
#include <iostream>

void HuffmanTree::buildTree(const std::map<char, int>& frequencies) {
    // 1. Create the Min-Heap (Priority Queue)
    // We use our custom 'Compare' struct here.
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> minHeap;

    // 2. Fill the Heap with leaf nodes
    for (auto const& [key, val] : frequencies) {
        minHeap.push(new HuffmanNode(key, val));
    }

    // 3. The "Greedy" Loop
    // Keep merging until only one node is left
    while (minHeap.size() > 1) {
        // A. Extract smallest
        HuffmanNode* left = minHeap.top();
        minHeap.pop();

        // B. Extract second smallest
        HuffmanNode* right = minHeap.top();
        minHeap.pop();

        // C. Create a new internal parent node
        // '$' is a placeholder character for internal nodes
        HuffmanNode* top = new HuffmanNode('$', left->frequency + right->frequency);
        
        // D. Link them
        top->left = left;
        top->right = right;

        // E. Push back into the pit
        minHeap.push(top);
    }

    // 4. The last remaining node is the Root
    root = minHeap.top();
}
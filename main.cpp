#include <iostream>
#include "src/FrequencyCounter.h"
#include "src/HuffmanTree.h" // Import the new engine

int main() {
    std::string filePath = "input.txt";
    
    // 1. Count Frequencies (Phase 1)
    FrequencyCounter counter;
    std::map<char, int> frequencies = counter.countFrequencies(filePath);

    if (frequencies.empty()) {
        std::cout << "File is empty." << std::endl;
        return 1;
    }

    // 2. Build the Tree (Phase 2)
    HuffmanTree tree;
    tree.buildTree(frequencies);

    // 3. Verify the Construction
    // The Root Frequency should match the total characters in the file
    HuffmanNode* root = tree.getRoot();
    
    if (root) {
        std::cout << " Tree Built Successfully!" << std::endl;
        std::cout << "Root Node Frequency: " << root->frequency << std::endl;
    } else {
        std::cout << " Tree Construction Failed." << std::endl;
    }

    return 0;
}
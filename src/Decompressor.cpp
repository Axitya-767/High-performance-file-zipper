#include "Decompressor.h"
#include <fstream>
#include <iostream>

std::map<char, int> Decompressor::readHeader(std::ifstream& inFile) {
    std::map<char, int> frequencies;
    size_t mapSize;

    // 1. Read the size of the map
    inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));

    // 2. Read each pair [Char][Freq]
    for (size_t i = 0; i < mapSize; ++i) {
        char ch;
        int freq;
        inFile.read(&ch, sizeof(ch));
        inFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        frequencies[ch] = freq;
    }
    return frequencies;
}

void Decompressor::decompressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    std::ofstream outFile(outputFile);

    // STEP 1: Rebuild the Map
    std::map<char, int> frequencies = readHeader(inFile);

    // STEP 2: Rebuild the Tree
    HuffmanTree tree;
    tree.buildTree(frequencies);
    HuffmanNode* current = tree.getRoot();

    // STEP 3: Calculate Total Characters (To handle padding)
    int totalChars = 0;
    for (auto const& [key, val] : frequencies) {
        totalChars += val;
    }

    // STEP 4: Decode Bits
    char byte;
    while (inFile.get(byte)) {
        // Process all 8 bits in the byte (from Left 7 to Right 0)
        for (int i = 7; i >= 0; --i) {
            int bit = (byte >> i) & 1;

            if (bit == 0) current = current->left;
            else          current = current->right;

            // Found a leaf node?
            if (current->left == nullptr && current->right == nullptr) {
                outFile.put(current->character);
                current = tree.getRoot(); // Reset to top
                
                totalChars--;
                if (totalChars == 0) return; // STOP! Ignore padding.
            }
        }
    }
}

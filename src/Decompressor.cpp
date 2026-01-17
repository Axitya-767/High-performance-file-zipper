#include "Decompressor.h"
#include "HuffmanTree.h"
#include <fstream>
#include <iostream>
#include <map>

// Helper to read the header
std::map<char, int> Decompressor::readHeader(std::ifstream& inFile, long long& outOriginalSize) {
    std::map<char, int> frequencies;

    // 1. Read Original Size (8 bytes)
    inFile.read(reinterpret_cast<char*>(&outOriginalSize), sizeof(long long));

    // 2. Read Map Size (4 bytes)
    int mapSize = 0;
    inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(int));

    // Safety Check
    if (mapSize < 0 || mapSize > 256) return frequencies;

    // 3. Read Map Data
    for (int i = 0; i < mapSize; ++i) {
        char ch;
        int freq;
        inFile.read(&ch, sizeof(char));
        inFile.read(reinterpret_cast<char*>(&freq), sizeof(int));
        frequencies[ch] = freq;
    }
    return frequencies;
}

void Decompressor::decompressFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile, std::ios::binary);
    std::ofstream outFile(outputFile, std::ios::binary);

    if (!inFile.is_open()) return;

    // STEP 1: Get Size and Map
    long long originalSize = 0;
    std::map<char, int> frequencies = readHeader(inFile, originalSize);

    if (frequencies.empty()) return;

    // STEP 2: Build Tree
    HuffmanTree tree;
    tree.buildTree(frequencies);
    HuffmanNode* current = tree.getRoot();

    // STEP 3: Decode Loop
    char byte;
    long long totalBytesWritten = 0;

    // READ BYTE BY BYTE
    while (inFile.get(byte)) {
        // PROCESS BIT BY BIT
        for (int i = 7; i >= 0; --i) {

            // *** THE FIX: STOP EXACTLY HERE ***
            if (totalBytesWritten == originalSize) return;

            int bit = (byte >> i) & 1;
            if (bit == 0) current = current->left;
            else          current = current->right;

            // Found a character?
            if (current->left == nullptr && current->right == nullptr) {
                outFile.put(current->character);
                totalBytesWritten++;
                current = tree.getRoot(); // Reset to top

                // *** DOUBLE CHECK: STOP HERE TOO ***
                if (totalBytesWritten == originalSize) return;
            }
        }
    }
}

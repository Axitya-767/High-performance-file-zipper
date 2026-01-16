#include "Decompressor.h"
#include <fstream>
#include <iostream>
#include <map>

// In Decompressor.cpp
    std::map<char, int> Decompressor::readHeader(std::ifstream& inFile) {
    std::map<char, int> frequencies;

    // 1. Read Signature
    int signature = 0;
    inFile.read(reinterpret_cast<char*>(&signature), sizeof(int));

    // Valid signature is 1179014472 ("HUFF")
    if (signature != 1179014472) {
        std::cerr << "❌ Error: Invalid file signature. Found: " << signature << "\n";
        return frequencies;
    }

    // 2. Read Map Size (4-byte INT)
    int mapSize = 0;
    inFile.read(reinterpret_cast<char*>(&mapSize), sizeof(int));

    // Safety Check
    if (mapSize < 0 || mapSize > 256) {
        std::cerr << "❌ Error: Invalid Map Size: " << mapSize << "\n";
        return frequencies;
    }

    // 3. Read Data
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
    // FIX 2: Open output in BINARY mode too
    std::ofstream outFile(outputFile, std::ios::binary);

    if (!inFile.is_open()) {
        std::cerr << "❌ Error: Could not open input file " << inputFile << "\n";
        return;
    }

    // STEP 1: Rebuild the Map
    std::map<char, int> frequencies = readHeader(inFile);

    if (frequencies.empty()) {
        std::cout << "⚠️  Empty frequency map. File might be empty.\n";
        return;
    }

    // STEP 2: Rebuild the Tree
    HuffmanTree tree;
    tree.buildTree(frequencies);
    HuffmanNode* current = tree.getRoot();

    // Safety check
    if (current == nullptr) return;

    // STEP 3: Calculate Total Characters (Use long long to prevent overflow)
    long long totalChars = 0;
    for (const auto& pair : frequencies) {
        totalChars += pair.second;
    }

    std::cout << "🔍 Decompressing " << totalChars << " characters...\n";

    // STEP 4: Decode Bits
    char byte;
    while (inFile.get(byte)) {
        // Process all 8 bits in the byte (from Left 7 to Right 0)
        for (int i = 7; i >= 0; --i) {
            int bit = (byte >> i) & 1;

            if (bit == 0) current = current->left;
            else          current = current->right;

            // Safety check against corrupted trees
            if (current == nullptr) {
                std::cerr << "❌ Error: Tree navigation failed (nullptr).\n";
                return;
            }

            // Found a leaf node?
            if (current->left == nullptr && current->right == nullptr) {
                outFile.put(current->character);
                current = tree.getRoot(); // Reset to top

                totalChars--;
                if (totalChars == 0) {
                    std::cout << "✅ Decompression Complete.\n";
                    return; // STOP! Ignore padding bits.
                }
            }
        }
    }
}

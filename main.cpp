#include <iostream>
#include <fstream>
#include "src/FrequencyCounter.h"
#include "src/HuffmanTree.h"
#include "src/BitWriter.h"
#include "src/Decompressor.h"

int main() {
    std::string inputFile = "input.txt";
    std::string compressedFile = "output.huff";
    std::string decodedFile = "decoded.txt";

    // --- PHASE 1: Analyze ---
    std::cout << "1. Analyzing..." << std::endl;
    FrequencyCounter counter;
    auto frequencies = counter.countFrequencies(inputFile);

    // --- PHASE 2: Build Tree ---
    std::cout << "2. Building Tree..." << std::endl;
    HuffmanTree tree;
    tree.buildTree(frequencies);
    tree.generateHuffmanCodes();
    auto codes = tree.getCodes();

    // --- PHASE 3 & 4 & 5: Compress ---
    // [CRITICAL FIX] We add braces { } here to create a "Scope"
    {
        std::cout << "3. Compressing to " << compressedFile << "..." << std::endl;
        BitWriter writer(compressedFile);
        writer.writeHeader(frequencies); // Header

        std::ifstream inFile(inputFile, std::ios::binary);
        char ch;
        while (inFile.get(ch)) {
            writer.writeCode(codes[ch]); // Body
        }
    } 
    // [CRITICAL] At this closing brace, 'writer' is destroyed. 
    // It forces the file to close and SAVE immediately.

    // --- PHASE 6: Decompress ---
    std::cout << "4. Decompressing to " << decodedFile << "..." << std::endl;
    Decompressor decompressor;
    decompressor.decompressFile(compressedFile, decodedFile);

    std::cout << "✅ Cycle Complete! Check 'decoded.txt'." << std::endl;
    return 0;
}
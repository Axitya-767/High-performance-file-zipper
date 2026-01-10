#include <iostream>
#include <fstream>
#include "src/FrequencyCounter.h"
#include "src/HuffmanTree.h"
#include "src/BitWriter.h" // Import the new tool

int main() {
    std::string inputFile = "input.txt";
    std::string outputFile = "output.huff"; // The custom format!

    // --- PHASE 1: Analyze ---
    FrequencyCounter counter;
    auto frequencies = counter.countFrequencies(inputFile);
    if (frequencies.empty()) return 1;

    // --- PHASE 2: Build Tree ---
    HuffmanTree tree;
    tree.buildTree(frequencies);

    // --- PHASE 3: Generate Codes ---
    tree.generateHuffmanCodes();
    auto codes = tree.getCodes();

    // --- PHASE 4: Compress (Bit Packing) ---
    std::cout << "Compressing to " << outputFile << "..." << std::endl;
    
    BitWriter writer(outputFile);
    
    // NEW: Write the "Secret Key" (Header) first!
    writer.writeHeader(frequencies); 
    
    // Open input file AGAIN to read char by char
    std::ifstream inFile(inputFile, std::ios::binary);

    char ch;
    
    while (inFile.get(ch)) {
        // Look up the code (e.g., 'a' -> "101")
        std::string code = codes[ch];
        // Write it as raw bits
        writer.writeCode(code);
    }
    
    writer.flush(); // Finish up
    
    std::cout << "✅ Compression Complete!" << std::endl;
    return 0;
}
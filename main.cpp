#include <iostream>
#include "src/FrequencyCounter.h"
#include "src/HuffmanTree.h"

int main() {
    // 1. Analyze
    FrequencyCounter counter;
    auto frequencies = counter.countFrequencies("input.txt");

    if (frequencies.empty()) {
        std::cout << "File is empty!" << std::endl;
        return 1;
    }

    // 2. Build Tree
    HuffmanTree tree;
    tree.buildTree(frequencies);

    // 3. Generate Codes (Phase 3)
    tree.generateHuffmanCodes();
    auto codes = tree.getCodes();

    // 4. Print the Translation Table
    std::cout << "--- Huffman Binary Codes ---" << std::endl;
    for (auto const& [ch, code] : codes) {
        std::cout << "'" << ch << "' : " << code << std::endl;
    }

    return 0;
}
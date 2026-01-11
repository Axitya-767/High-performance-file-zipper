#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <string>
#include <map>
#include "HuffmanTree.h" // We need the tree logic to rebuild it

class Decompressor {
public:
    void decompressFile(const std::string& inputFile, const std::string& outputFile);

private:
    std::map<char, int> readHeader(std::ifstream& inFile);
};

#endif
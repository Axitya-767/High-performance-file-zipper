#ifndef DECOMPRESSOR_H
#define DECOMPRESSOR_H

#include <string>
#include <fstream>
#include <map>

class Decompressor {
public:
    void decompressFile(const std::string& inputPath, const std::string& outputPath);

private:
    // This was missing the second parameter in your header file!
    std::map<char, int> readHeader(std::ifstream& inFile, long long& outOriginalSize);
};

#endif

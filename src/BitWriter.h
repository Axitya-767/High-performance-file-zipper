#ifndef BITWRITER_H
#define BITWRITER_H

#include <string>
#include <fstream>
#include <vector>
#include <map>

class BitWriter {
public:
    BitWriter(const std::string& outputPath);
    ~BitWriter();

    // UPDATE: Now accepts originalSize
    void writeHeader(const std::map<char, int>& frequencies, long long originalSize);

    void writeCode(const std::string& code);
    std::string getFile() const { return outputPath; }

private:
    std::string outputPath;
    std::ofstream outFile;
    unsigned char currentByte;
    int bitCount;
    void flush();
};

#endif

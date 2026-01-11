#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <unordered_map>
#include <fstream>
#include <string>

class BitWriter {
private:
    std::ofstream outFile;
    unsigned char buffer; 
    int bitCount;         

public:
    BitWriter(const std::string& filePath);
    ~BitWriter();

    // MATCHED: Now uses unordered_map to match the .cpp
    void writeHeader(const std::unordered_map<char, int>& frequencies);
    
    void writeBit(int bit);
    void writeCode(const std::string& code);
    void flush();
};

#endif
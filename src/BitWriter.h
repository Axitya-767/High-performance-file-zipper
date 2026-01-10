#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <fstream>
#include <string>
#include <map>

class BitWriter {
private:
    std::ofstream outFile;
    unsigned char buffer; // The 8-bit container
    int bitCount;         // How full is the container? (0-8)

public:
    // Constructor: Opens the file in BINARY mode
    BitWriter(const std::string& filePath);
    // Destructor: Ensures file is closed properly
    ~BitWriter();
    void writeHeader(const std::map<char, int>& frequencies);
    // The Magic: Takes a "0" or "1" and packs it
    void writeBit(int bit);
    // Helper: Takes a whole string "101" and calls writeBit loop
    void writeCode(const std::string& code);
    // Force write the remaining partial byte
    void flush();
};

#endif
#include "BitWriter.h"

BitWriter::BitWriter(const std::string& filePath) {
    outFile.open(filePath, std::ios::binary);
    buffer = 0;
    bitCount = 0;
}

BitWriter::~BitWriter() {
    flush(); 
    if (outFile.is_open()) {
        outFile.close();
    }
}

void BitWriter::writeBit(int bit) {
    if (bit == 1) {
        buffer = buffer | (1 << (7 - bitCount));
    }
    bitCount++;

    if (bitCount == 8) {
        outFile.put(buffer);
        buffer = 0; 
        bitCount = 0;
    }
}

void BitWriter::writeCode(const std::string& code) {
    for (char c : code) {
        writeBit(c == '1');
    }
}

void BitWriter::flush() {
    if (bitCount > 0) {
        outFile.put(buffer);
        buffer = 0;
        bitCount = 0;
    }
}

// In Compressor.cpp (or BitWriter.cpp)
void BitWriter::writeHeader(std::ofstream& outFile, const std::map<char, int>& frequencies) {
    // 1. Write Magic Signature "HUFF" (0x46465548)
    // This tells the decompressor: "Yes, this is a valid Huffman file"
    int signature = 1179014472;
    outFile.write(reinterpret_cast<const char*>(&signature), sizeof(int));

    // 2. Write Map Size (Always as 4-byte INT)
    // We cast to int to prevent the "size_t" 8-byte error on Mac
    int size = static_cast<int>(frequencies.size());
    outFile.write(reinterpret_cast<const char*>(&size), sizeof(int));

    // 3. Write Data
    for (const auto& pair : frequencies) {
        char ch = pair.first;
        int freq = pair.second;
        outFile.write(&ch, sizeof(char));
        outFile.write(reinterpret_cast<const char*>(&freq), sizeof(int));
    }
}

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

void BitWriter::writeHeader(const std::map<char, int>& frequencies){
    // Write Magic Signature
    outFile.write("HUFF", 4);

    size_t mapSize = frequencies.size();
    outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize));

    for (const auto& entry : frequencies) {
        char character = entry.first;
        int frequency = entry.second;
        outFile.write(&character, sizeof(character));
        outFile.write(reinterpret_cast<const char*>(&frequency), sizeof(frequency));
    }
}
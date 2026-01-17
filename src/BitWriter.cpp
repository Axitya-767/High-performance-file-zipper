#include "BitWriter.h"
#include <stdexcept> // Needed for errors

BitWriter::BitWriter(const std::string& path) : outputPath(path), currentByte(0), bitCount(0) {
    outFile.open(path, std::ios::binary);

    // CRITICAL FIX: Check if file actually opened!
    if (!outFile.is_open()) {
        throw std::runtime_error("Could not create output file: " + path);
    }
}

BitWriter::~BitWriter() {
    flush();
    if (outFile.is_open()) {
        outFile.close();
    }
}

void BitWriter::writeHeader(const std::map<char, int>& frequencies, long long originalSize) {
    // 1. Write Original Size (8 bytes)
    outFile.write(reinterpret_cast<const char*>(&originalSize), sizeof(long long));

    // 2. Write Map Size (4 bytes)
    int mapSize = static_cast<int>(frequencies.size());
    outFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(int));

    // 3. Write Map Data
    for (const auto& pair : frequencies) {
        outFile.put(pair.first);
        int freq = pair.second;
        outFile.write(reinterpret_cast<const char*>(&freq), sizeof(int));
    }
}

void BitWriter::writeCode(const std::string& code) {
    for (char bit : code) {
        if (bit == '1') {
            currentByte |= (1 << (7 - bitCount));
        }
        bitCount++;
        if (bitCount == 8) {
            outFile.put(currentByte);
            currentByte = 0;
            bitCount = 0;
        }
    }
}

void BitWriter::flush() {
    if (bitCount > 0) {
        outFile.put(currentByte);
    }
}

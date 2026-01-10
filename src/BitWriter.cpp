#include "BitWriter.h"

BitWriter::BitWriter(const std::string& filePath) {
    // std::ios::binary is CRITICAL. Without it, the OS messes with your bytes.
    outFile.open(filePath, std::ios::binary);
    buffer = 0;
    bitCount = 0;
}

BitWriter::~BitWriter() {
    flush(); // Make sure no bits are left behind
    if (outFile.is_open()) {
        outFile.close();
    }
}

void BitWriter::writeBit(int bit) {
    // If the bit is '1', we turn on the specific bit in the byte.
    // If it's '0', we leave it alone (it's already 0 initialized).
    if (bit == 1) {
        // "7 - bitCount" means we fill from Left (MSB) to Right (LSB)
        buffer = buffer | (1 << (7 - bitCount));
    }
    
    bitCount++;

    // BUFFER FULL? WRITE IT!
    if (bitCount == 8) {
        outFile.put(buffer);
        buffer = 0; // Reset
        bitCount = 0;
    }
}

void BitWriter::writeCode(const std::string& code) {
    for (char c : code) {
        if (c == '0') writeBit(0);
        else writeBit(1);
    }
}

void BitWriter::flush() {
    // If there are leftover bits (e.g., 3 bits sitting in buffer)
    if (bitCount > 0) {
        outFile.put(buffer); // The remaining slots are just 0s (padding)
        buffer = 0;
        bitCount = 0;
    }
}
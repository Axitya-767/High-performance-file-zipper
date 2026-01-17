#include "FrequencyCounter.h"
#include <fstream>
#include <iostream>

std::map<char, int> FrequencyCounter::countFrequencies(const std::string& filePath) {
    std::map<char, int> freqMap;
    std::ifstream inputFile(filePath, std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return freqMap;
    }

    char rawBuffer;
    // FIX 1: Use 'inputFile' (not 'file')
    while (inputFile.get(rawBuffer)) {
        // FIX 2: Cast to unsigned char
        // FIX 3: Use 'freqMap' (not 'frequencies')
        freqMap[static_cast<unsigned char>(rawBuffer)]++;
    }

    inputFile.close();
    return freqMap;
}

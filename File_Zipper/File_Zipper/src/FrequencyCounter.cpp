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

    char ch;
    while (inputFile.get(ch)) {
        freqMap[ch]++;
    }

    inputFile.close();
    return freqMap;
}
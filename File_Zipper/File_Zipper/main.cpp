#include <iostream>
#include "src/FrequencyCounter.h"

int main() {
    std::string filePath = "input.txt";
    
    FrequencyCounter counter;
    std::map<char, int> frequencies = counter.countFrequencies(filePath);

    if (frequencies.empty()) {
        std::cout << "File is empty or could not be read." << std::endl;
        return 1;
    }

    std::cout << "--- Character Frequencies ---" << std::endl;
    for (auto const& [key, val] : frequencies) {
        std::cout << "'" << key << "' : " << val << std::endl;
    }

    return 0;
}
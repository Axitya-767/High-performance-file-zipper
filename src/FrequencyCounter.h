#ifndef FREQUENCY_COUNTER_H
#define FREQUENCY_COUNTER_H

#include <string>
#include <map>

class FrequencyCounter {
public:
    std::map<char, int> countFrequencies(const std::string& filePath);
};

#endif
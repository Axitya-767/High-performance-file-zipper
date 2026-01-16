#ifndef BIT_WRITER_H
#define BIT_WRITER_H

#include <unordered_map>
#include <fstream>
#include <string>
#include<map>

class BitWriter {
private:
    std::ofstream outFile;
    unsigned char buffer; 
    int bitCount;         

public:
    std::ofstream& getFile() { return outFile; }
    BitWriter(const std::string& filePath);
    ~BitWriter();

    // MATCHED: Now uses unordered_map to match the .cpp
    #include <map>
// ...
    void writeHeader(std::ofstream& outFile, const std::map<char, int>& frequencies);
    
    void writeBit(int bit);
    void writeCode(const std::string& code);
    void flush();
};

#endif

#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

class ZipperApp {
public:
    // The main entry point for batch processing
    void processDirectory(const std::string& inputDir, const std::string& outputDir, bool compressMode);

private:
    // Helper function to handle individual compression
    static void compressTask(std::string inputPath, std::string outputDir);
    
    // Helper function to handle individual decompression
    static void decompressTask(std::string inputPath, std::string outputDir);
    
    // Mutex for thread-safe console logging
    static std::mutex consoleMutex;
};
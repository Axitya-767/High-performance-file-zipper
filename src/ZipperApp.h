#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
#include <atomic>

namespace fs = std::filesystem;

class ZipperApp {
public:
    ZipperApp() : totalOriginalSize(0), totalCompressedSize(0), processedFiles(0) {}
    void processDirectory(const std::string& inputDir, const std::string& outputDir, bool compressMode);

private:
    void compressTask(std::string inputPath, std::string outputDir);
    void decompressTask(std::string inputPath, std::string outputDir);
    
    // ADD THESE TWO LINES TO FIX THE ERRORS:
    std::string formatBytes(long long bytes);
    void printReport(double durationSeconds);

    static std::mutex consoleMutex;
    std::atomic<long long> totalOriginalSize;
    std::atomic<long long> totalCompressedSize;
    std::atomic<int> processedFiles;
};
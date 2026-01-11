#include "ZipperApp.h"
#include "ThreadPool.h"
#include "FrequencyCounter.h"
#include "HuffmanTree.h"
#include "BitWriter.h"
#include "Decompressor.h"
#include <iostream>
#include <future>
#include <chrono>
#include <iomanip>
#include <sstream>

std::mutex ZipperApp::consoleMutex;

void ZipperApp::processDirectory(const std::string& inputDir, const std::string& outputDir, bool compressMode) {
    if (!fs::exists(outputDir)) fs::create_directory(outputDir);

    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<void>> results;
    auto startTime = std::chrono::high_resolution_clock::now();

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.is_regular_file() && entry.path().filename().string()[0] != '.') {
            std::string path = entry.path().string();
            if (compressMode && entry.path().extension() != ".huff") 
                results.emplace_back(pool.enqueue(&ZipperApp::compressTask, this, path, outputDir));
            else if (!compressMode && entry.path().extension() == ".huff")
                results.emplace_back(pool.enqueue(&ZipperApp::decompressTask, this, path, outputDir));
        }
    }

    for(auto && res : results) res.get();

    auto endTime = std::chrono::high_resolution_clock::now();
    printReport(std::chrono::duration<double>(endTime - startTime).count());
}

void ZipperApp::printReport(double durationSeconds) {
    if (processedFiles == 0) return;
    double saving = (totalOriginalSize > 0) ? 100.0 * (1.0 - (double)totalCompressedSize / totalOriginalSize) : 0;

    std::cout << "\n------------------------------------------------\n";
    std::cout << "📊 Stats: " << processedFiles << " files in " << std::fixed << std::setprecision(3) << durationSeconds << "s\n";
    std::cout << "   Saved: " << formatBytes(totalOriginalSize - totalCompressedSize) << " (" << std::fixed << std::setprecision(2) << saving << "%)\n";
    std::cout << "------------------------------------------------\n";
}

std::string ZipperApp::formatBytes(long long bytes) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    if (bytes < 1024) ss << bytes << " B";
    else if (bytes < 1048576) ss << (bytes / 1024.0) << " KB";
    else ss << (bytes / 1048576.0) << " MB";
    return ss.str();
}

void ZipperApp::compressTask(std::string inputPath, std::string outputDir) {
    try {
        long long origSize = fs::file_size(inputPath);
        std::string outPath = outputDir + "/" + fs::path(inputPath).filename().string() + ".huff";
        
        FrequencyCounter counter;
        auto freqs = counter.countFrequencies(inputPath);
        HuffmanTree tree;
        tree.buildTree(freqs);
        tree.generateHuffmanCodes();
        
        {
            BitWriter writer(outPath);
            writer.writeHeader(freqs);
            std::ifstream inFile(inputPath, std::ios::binary);
            char ch;
            auto codes = tree.getCodes();
            while (inFile.get(ch)) writer.writeCode(codes[(unsigned char)ch]);
        }

        totalOriginalSize += origSize;
        totalCompressedSize += fs::file_size(outPath);
        processedFiles++;
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "🚀 Compressed: " << fs::path(inputPath).filename() << "\n";
    } catch (...) {}
}

void ZipperApp::decompressTask(std::string inputPath, std::string outputDir) {
    try {
        std::string outPath = outputDir + "/" + fs::path(inputPath).stem().string();
        Decompressor dec;
        dec.decompressFile(inputPath, outPath);
        processedFiles++;
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "♻️  Restored:   " << fs::path(inputPath).filename() << "\n";
    } catch (...) {}
}
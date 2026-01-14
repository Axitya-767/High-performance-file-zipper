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
    // CHANGE 1: We don't just create the top folder anymore, we create them as needed.
    // But we still ensure the root output exists.
    if (!fs::exists(outputDir)) fs::create_directory(outputDir);

    unsigned int cores = std::thread::hardware_concurrency();
    ThreadPool pool(cores);
    std::vector<std::future<void>> results;

    auto startTime = std::chrono::high_resolution_clock::now();

    std::cout << "⚙️  Engine started with " << cores << " threads.\n";
    std::cout << "📂 Scanning '" << inputDir << "' recursively...\n";

    // CHANGE 2: Use recursive_directory_iterator to find files in subfolders
    for (const auto& entry : fs::recursive_directory_iterator(inputDir)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            std::string filename = entry.path().filename().string();
            
            // Skip hidden files
            if (filename[0] == '.') continue;

            // CHANGE 3: Calculate the specific subfolder for this file
            // Example: If file is "data/docs/notes.txt", relative path is "docs/notes.txt"
            // We need to pass the target folder "archives/docs" to the task.
            fs::path relativePath = fs::relative(entry.path(), inputDir);
            fs::path parentPath = relativePath.parent_path(); // "docs"
            std::string targetSubDir = (fs::path(outputDir) / parentPath).string();

            if (compressMode && entry.path().extension() != ".huff") {
                results.emplace_back(pool.enqueue(&ZipperApp::compressTask, this, path, targetSubDir));
            } 
            else if (!compressMode && entry.path().extension() == ".huff") {
                results.emplace_back(pool.enqueue(&ZipperApp::decompressTask, this, path, targetSubDir));
            }
        }
    }

    for(auto && res : results) res.get();

    auto endTime = std::chrono::high_resolution_clock::now();
    printReport(std::chrono::duration<double>(endTime - startTime).count());
}

void ZipperApp::compressTask(std::string inputPath, std::string outputDir) {
    // CHANGE 4: Create the specific sub-directory if it doesn't exist
    // Because 'outputDir' might be "archives/docs/work/" now.
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir); // Note: create_directories (plural) makes the whole path
    }

    std::string filename = fs::path(inputPath).filename().string();
    std::string outputPath = outputDir + "/" + filename + ".huff";

    try {
        long long origSize = fs::file_size(inputPath);
        
        FrequencyCounter counter;
        auto frequencies = counter.countFrequencies(inputPath);
        
            auto frequencies = counter.countFrequencies(inputPath);

// SAFETY CHECK: If the file is empty, don't build a tree!
    if (frequencies.empty()) {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "⚠️  Skipping empty file: " << filename << std::endl;
        return; 
    }

    HuffmanTree tree;
    tree.buildTree(frequencies);

        HuffmanTree tree;
        tree.buildTree(frequencies);
        tree.generateHuffmanCodes();
        
        {
            BitWriter writer(outputPath);
            writer.writeHeader(frequencies);
            std::ifstream inFile(inputPath, std::ios::binary);
            char ch;
            auto codes = tree.getCodes();
            while (inFile.get(ch)) writer.writeCode(codes[(unsigned char)ch]);
        }

        long long compressedBytes = fs::file_size(outputPath);
        
        totalOriginalSize += origSize;
        totalCompressedSize += compressedBytes;
        processedFiles++;

        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "🚀 Compressed: " << filename << std::endl;
    } catch (...) {}
}

void ZipperApp::decompressTask(std::string inputPath, std::string outputDir) {
    // CHANGE 5: Create sub-directory for decompression too
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
    }

    std::string filename = fs::path(inputPath).stem().string();
    std::string outputPath = outputDir + "/" + filename;

    // Avoid overwriting
    if (fs::exists(outputPath)) outputPath = outputDir + "/restored_" + filename;

    try {
        Decompressor decompressor;
        decompressor.decompressFile(inputPath, outputPath);
        processedFiles++;
        
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "♻️  Restored:    " << filename << std::endl;
    } catch (...) {}
}

// --- Helpers (No changes needed here) ---

void ZipperApp::printReport(double durationSeconds) {
    if (processedFiles == 0) {
        std::cout << "⚠️  No matching files found.\n";
        return;
    }
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
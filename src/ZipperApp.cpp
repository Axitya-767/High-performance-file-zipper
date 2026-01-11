#include "ZipperApp.h"
#include "ThreadPool.h"
#include "FrequencyCounter.h"
#include "HuffmanTree.h"
#include "BitWriter.h"
#include "Decompressor.h"
#include <iostream>
#include <future>

std::mutex ZipperApp::consoleMutex;

void ZipperApp::processDirectory(const std::string& inputDir, const std::string& outputDir, bool compressMode) {
    if (!fs::exists(outputDir)) fs::create_directory(outputDir);

    unsigned int cores = std::thread::hardware_concurrency();
    ThreadPool pool(cores);
    std::vector<std::future<void>> results;

    std::cout << "⚙️  Engine started with " << cores << " threads.\n";
    std::cout << "📂 Scanning '" << inputDir << "'...\n";

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            std::string ext = entry.path().extension().string();

            if (compressMode && ext != ".huff") {
                 // Skip hidden files
                if (entry.path().filename().string()[0] != '.')
                    results.emplace_back(pool.enqueue(compressTask, path, outputDir));
            } 
            else if (!compressMode && ext == ".huff") {
                results.emplace_back(pool.enqueue(decompressTask, path, outputDir));
            }
        }
    }

    // Wait for all threads to finish
    for(auto && res : results) res.get();
    std::cout << "✅ Batch processing complete.\n";
}

void ZipperApp::compressTask(std::string inputPath, std::string outputDir) {
    std::string filename = fs::path(inputPath).filename().string();
    std::string outputPath = outputDir + "/" + filename + ".huff";

    try {
        FrequencyCounter counter;
        auto frequencies = counter.countFrequencies(inputPath);
        
        HuffmanTree tree;
        tree.buildTree(frequencies);
        tree.generateHuffmanCodes();
        auto codes = tree.getCodes();

        {
            BitWriter writer(outputPath);
            writer.writeHeader(frequencies);
            std::ifstream inFile(inputPath, std::ios::binary);
            char ch;
            while (inFile.get(ch)) {
                writer.writeCode(codes[ch]);
            }
        }

        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "🚀 Compressed: " << filename << std::endl;

    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cerr << "❌ Error: " << e.what() << std::endl;
    }
}

void ZipperApp::decompressTask(std::string inputPath, std::string outputDir) {
    std::string filename = fs::path(inputPath).stem().string(); // Remove .huff
    std::string outputPath = outputDir + "/" + filename;

    // Avoid overwriting
    if (fs::exists(outputPath)) {
        outputPath = outputDir + "/restored_" + filename;
    }

    try {
        Decompressor decompressor;
        decompressor.decompressFile(inputPath, outputPath);

        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cout << "♻️  Restored:    " << filename << std::endl;

    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(consoleMutex);
        std::cerr << "❌ Error: " << e.what() << std::endl;
    }
}
#include <iostream>
#include <vector>
#include <string>
#include <filesystem> // Standard C++17 filesystem
#include <mutex>
#include <future>     // Required for tracking results
#include "src/FrequencyCounter.h"
#include "src/HuffmanTree.h"
#include "src/BitWriter.h"
#include "src/ThreadPool.h"

// Define a short alias for filesystem
namespace fs = std::filesystem;

std::mutex coutMutex;

void compressFileTask(std::string inputPath) {
    // Extract filename (e.g., "data/test.txt" -> "test")
    std::string filename = fs::path(inputPath).stem().string();
    
    // Route output to archives folder
    std::string outputPath = "archives/" + filename + ".huff";

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

        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "🚀 Archived: " << inputPath << " -> " << outputPath << std::endl;

    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cerr << "❌ Error processing " << inputPath << ": " << e.what() << std::endl;
    }
}

int main() {
    std::string inputDir = "data";
    std::string outputDir = "archives";

    // Ensure output directory exists
    if (!fs::exists(outputDir)) fs::create_directory(outputDir);

    unsigned int cores = std::thread::hardware_concurrency();
    std::cout << "🔍 Scanning '" << inputDir << "' using " << cores << " threads..." << std::endl;

    ThreadPool pool(cores);
    std::vector<std::future<void>> results;

    // Scan directory and launch tasks
    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            // Don't compress files that are already compressed
            if (fs::path(path).extension() != ".huff") {
                // Save the "future" result so we can wait for it later
                results.emplace_back(
                    pool.enqueue(compressFileTask, path)
                );
            }
        }
    }

    // CRITICAL: Wait for all threads to finish before exiting
    for(auto && result: results) {
        result.get(); 
    }

    std::cout << "✅ All files processed." << std::endl;
    return 0;
}
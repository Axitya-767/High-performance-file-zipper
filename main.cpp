#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>
#include "src/FrequencyCounter.h"
#include "src/HuffmanTree.h"
#include "src/BitWriter.h"
#include "src/Decompressor.h"
#include "src/ThreadPool.h" // Our new engine

std::mutex coutMutex; // Keeps console output from overlapping

void compressFileTask(std::string inputFile, std::string compressedFile) {
    // --- PHASE 1: Analyze ---
    FrequencyCounter counter;
    auto frequencies = counter.countFrequencies(inputFile);

    // --- PHASE 2: Build Tree ---
    HuffmanTree tree;
    tree.buildTree(frequencies);
    tree.generateHuffmanCodes();
    auto codes = tree.getCodes();

    // --- PHASE 3 & 4 & 5: Compress ---
    {
        BitWriter writer(compressedFile);
        writer.writeHeader(frequencies);
        std::ifstream inFile(inputFile, std::ios::binary);
        char ch;
        while (inFile.get(ch)) {
            writer.writeCode(codes[ch]);
        }
    } 

    std::lock_guard<std::mutex> lock(coutMutex);
    std::cout << "✅ Processed: " << inputFile << " -> " << compressedFile << " [Thread: " << std::this_thread::get_id() << "]" << std::endl;
}

int main() {
    int cores = std::thread::hardware_concurrency();
    std::cout << "🚀 Launching Parallel Engine with " << cores << " threads." << std::endl;
    
    ThreadPool pool(cores);
    
    // Example: Compressing multiple files at once
    std::vector<std::string> inputs = {"input.txt", "input.txt", "input.txt"}; // Use different files in reality
    
    for (int i = 0; i < inputs.size(); ++i) {
        std::string out = "output_" + std::to_string(i) + ".huff";
        pool.enqueue(compressFileTask, inputs[i], out);
    }

    return 0; // ThreadPool destructor waits for all tasks to finish
}
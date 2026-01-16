#include "ZipperApp.h"
#include <QtConcurrent>
#include <QFuture>
#include "FrequencyCounter.h"
#include "HuffmanTree.h"
#include "BitWriter.h"
#include "Decompressor.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

std::mutex ZipperApp::consoleMutex;

void ZipperApp::processOneGoCycle(const std::string& inputPath, const std::string& outputDir) {
    if (!fs::exists(outputDir)) fs::create_directories(outputDir);

    auto startTime = std::chrono::high_resolution_clock::now();

    // 1. CHECK: Is it a file?
    if (fs::is_regular_file(inputPath)) {
        fs::path p(inputPath);
        std::string filename = p.filename().string(); // e.g., "data.txt"
        std::string stem = p.stem().string();         // e.g., "data"
        std::string ext = p.extension().string();     // e.g., ".txt"

        // Construct Names
        // Compressed: data.txt.compressed
        std::string compressedFile = outputDir + "/" + filename + ".compressed";

        // Restored: data_new.txt (Instead of data.txt.new)
        std::string restoredFile = outputDir + "/" + stem + "_new" + ext;

        // STEP A: Compress
        emit statusChanged("⚡ Step 1: Compressing...");
        compressTo(inputPath, compressedFile);

        // STEP B: Decompress (Immediately)
        emit statusChanged("♻️ Step 2: Verifying (Decompressing)...");
        decompressTo(compressedFile, restoredFile);

        emit statusChanged("✅ Cycle Complete: " + QString::fromStdString(stem + "_new" + ext));
    }
    // 2. CHECK: Is it a folder?
    else if (fs::is_directory(inputPath)) {
        int count = 0;
        for (const auto& entry : fs::recursive_directory_iterator(inputPath)) {
            if (entry.is_regular_file()) {
                std::string subIn = entry.path().string();
                if (entry.path().filename().string()[0] == '.') continue; // skip hidden

                fs::path p(subIn);
                std::string stem = p.stem().string();
                std::string ext = p.extension().string();
                std::string parentDir = p.parent_path().string();

                // Define paths relative to the file location
                std::string subComp = subIn + ".compressed";
                std::string subNew = parentDir + "/" + stem + "_new" + ext;

                emit statusChanged("⚡ Cycle: " + QString::fromStdString(p.filename().string()));

                // Execute Cycle
                compressTo(subIn, subComp);
                decompressTo(subComp, subNew);
                count++;
            }
        }
        emit statusChanged("✅ Batch Complete: " + QString::number(count) + " files processed.");
    }
}

// Helper: Compress specific input to specific output
void ZipperApp::compressTo(std::string inputPath, std::string outputPath) {
    try {
        long long origSize = fs::file_size(inputPath);
        FrequencyCounter counter;
        auto frequencies = counter.countFrequencies(inputPath);
        if (frequencies.empty()) return;

        HuffmanTree tree;
        tree.buildTree(frequencies);
        tree.generateHuffmanCodes();

        {
            BitWriter writer(outputPath);
            writer.writeHeader(writer.getFile(), frequencies);

            std::ifstream inFile(inputPath, std::ios::binary);
            char ch;
            auto codes = tree.getCodes();

            long long processedBytes = 0;
            long long updateThreshold = origSize / 100;
            if (updateThreshold == 0) updateThreshold = 1;

            while (inFile.get(ch)) {
                writer.writeCode(codes[(unsigned char)ch]);
                processedBytes++;
                if (processedBytes % updateThreshold == 0) {
                    emit progressUpdated((int)((processedBytes * 100) / origSize));
                }
            }
            emit progressUpdated(100);
        }
    } catch (...) {
        emit statusChanged("❌ Compress Error");
    }
}

// Helper: Decompress specific input to specific output
void ZipperApp::decompressTo(std::string inputPath, std::string outputPath) {
    try {
        Decompressor decompressor;
        decompressor.decompressFile(inputPath, outputPath);
        emit progressUpdated(100);
    } catch (...) {
        emit statusChanged("❌ Decompress Error");
    }
}

// Required helpers to prevent linker errors
void ZipperApp::printReport(double durationSeconds) {}
std::string ZipperApp::formatBytes(long long bytes) { return ""; }

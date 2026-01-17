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

// Main Cycle Logic
void ZipperApp::processOneGoCycle(const std::string& inputPath, const std::string& outputDir) {
    if (!fs::exists(outputDir)) fs::create_directories(outputDir);

    if (fs::is_regular_file(inputPath)) {
        fs::path p(inputPath);
        std::string filename = p.filename().string();
        std::string stem = p.stem().string();
        std::string ext = p.extension().string();

        std::string compressedFile = outputDir + "/" + filename + ".compressed";
        std::string restoredFile = outputDir + "/" + stem + "_new" + ext;

        emit statusChanged("⚡ Step 1: Compressing...");

        // Call the bool function
        bool success = compressTo(inputPath, compressedFile);

        if (!success) {
            emit statusChanged("❌ Compression Failed! Check file permissions.");
            return;
        }

        emit statusChanged("♻️ Step 2: Verifying...");
        decompressTo(compressedFile, restoredFile);

        emit statusChanged("✅ Cycle Complete: " + QString::fromStdString(stem + "_new" + ext));
    }
    else if (fs::is_directory(inputPath)) {
        emit statusChanged("⚠️ Single file mode recommended for verification.");
    }
}

// Compression Logic
bool ZipperApp::compressTo(std::string inputPath, std::string outputPath) {
    try {
        long long origSize = fs::file_size(inputPath);

        FrequencyCounter counter;
        auto frequencies = counter.countFrequencies(inputPath);
        if (frequencies.empty()) return false;

        HuffmanTree tree;
        tree.buildTree(frequencies);
        tree.generateHuffmanCodes();

        {
            BitWriter writer(outputPath);
            // Write Header with size
            writer.writeHeader(frequencies, origSize);

            std::ifstream inFile(inputPath, std::ios::binary);

            char rawBuffer;
            unsigned char ch;
            auto codes = tree.getCodes();

            // BINARY SAFE LOOP
            while (inFile.get(rawBuffer)) {
                ch = static_cast<unsigned char>(rawBuffer);
                // Ensure the map key matches the insertion key
                writer.writeCode(codes[static_cast<char>(ch)]);
            }
        }

        emit progressUpdated(100);
        return true;

    } catch (...) {
        emit statusChanged("❌ Compress Error");
        return false;
    }
}

// Decompression Logic
void ZipperApp::decompressTo(std::string inputPath, std::string outputPath) {
    try {
        Decompressor decompressor;
        decompressor.decompressFile(inputPath, outputPath);
        emit progressUpdated(100);
    } catch (...) {
        emit statusChanged("❌ Decompress Error");
    }
}

// Linker shims
void ZipperApp::printReport(double) {}
std::string ZipperApp::formatBytes(long long) { return ""; }

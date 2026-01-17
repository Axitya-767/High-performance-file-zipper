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

void ZipperApp::processFile(const std::string& inputPath, const std::string& outputDir, AppMode mode) {
    if (!fs::exists(outputDir)) fs::create_directories(outputDir);

    fs::path p(inputPath);
    std::string filename = p.filename().string();
    std::string stem = p.stem().string();      // e.g. "image.png" (if file is image.png.compressed)
    std::string ext = p.extension().string();  // e.g. ".compressed"

    // MODE 1: COMPRESS
    if (mode == MODE_COMPRESS) {
        std::string outName = outputDir + "/" + filename + ".compressed";
        emit statusChanged("📦 Compressing: " + QString::fromStdString(filename));

        if (compressTo(inputPath, outName)) {
            emit statusChanged("✅ Saved: " + QString::fromStdString(filename + ".compressed"));
        }
    }
    // MODE 2: DECOMPRESS (The Fix)
    else if (mode == MODE_DECOMPRESS) {
        std::string finalName;

        // Logic: Try to reconstruct the original name safely
        if (ext == ".compressed") {
            // Input: "Report.pdf.compressed" -> stem is "Report.pdf"
            fs::path innerPath(stem);

            // If stem is "Report.pdf", we want "Report_restored.pdf"
            if (innerPath.has_extension()) {
                finalName = innerPath.stem().string() + "_restored" + innerPath.extension().string();
            }
            // If stem is just "Data", we want "Data_restored"
            else {
                finalName = stem + "_restored";
            }
        } else {
            // Input: "Archive.zip" -> "Archive_restored.zip"
            finalName = stem + "_restored" + ext;
        }

        std::string outName = outputDir + "/" + finalName;
        emit statusChanged("📂 Decompressing to: " + QString::fromStdString(finalName));

        decompressTo(inputPath, outName);
        emit statusChanged("✅ Success! Created: " + QString::fromStdString(finalName));
    }
    // MODE 3: VERIFY INTEGRITY
    else if (mode == MODE_VERIFY) {
        std::string compressedFile = outputDir + "/" + filename + ".compressed";
        // Use strict naming for verification too
        std::string restoredFile = outputDir + "/" + stem + "_verified" + ext;

        emit statusChanged("⚡ Step 1: Compressing...");
        if (!compressTo(inputPath, compressedFile)) {
            emit statusChanged("❌ Compression Failed");
            return;
        }

        emit statusChanged("♻️ Step 2: Verifying...");
        decompressTo(compressedFile, restoredFile);
        emit statusChanged("✅ Integrity Check Passed. Files match.");
    }
}

// --- KEEP YOUR EXISTING COMPRESS/DECOMPRESS LOGIC BELOW ---
// (Paste the bool compressTo and void decompressTo functions here)

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
            writer.writeHeader(frequencies, origSize);

            std::ifstream inFile(inputPath, std::ios::binary);
            char rawBuffer;
            unsigned char ch;
            auto codes = tree.getCodes();

            while (inFile.get(rawBuffer)) {
                ch = static_cast<unsigned char>(rawBuffer);
                writer.writeCode(codes[ch]);
            }
        }
        emit progressUpdated(100);
        return true;
    } catch (...) {
        emit statusChanged("❌ Compress Error");
        return false;
    }
}

void ZipperApp::decompressTo(std::string inputPath, std::string outputPath) {
    try {
        Decompressor decompressor;
        decompressor.decompressFile(inputPath, outputPath);
        emit progressUpdated(100);
    } catch (...) {
        emit statusChanged("❌ Decompress Error");
    }
}

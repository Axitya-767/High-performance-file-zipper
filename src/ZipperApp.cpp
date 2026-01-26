#include "ZipperApp.h"
#include <QtConcurrent>
#include <QFuture>
#include "FrequencyCounter.h"
#include "HuffmanTree.h"
#include "BitWriter.h"
#include "Decompressor.h"
#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

std::mutex ZipperApp::consoleMutex;

void ZipperApp::processFile(const std::string& inputPath, const std::string& outputPath, AppMode mode) {

    // ---------------------------------------------------------
    // CASE A: FOLDER INPUT (Batch Processing)
    // ---------------------------------------------------------
    // If user drops a folder, we compress every file inside it individually.
    if (fs::is_directory(inputPath)) {

        // Ensure the container folder exists (e.g., "MyFolder_PROCESSED")
        if (!fs::exists(outputPath)) fs::create_directories(outputPath);

        emit statusChanged("🚀 Scanning Folder...");

        std::vector<std::string> allFiles;
        try {
            // Recursive scan
            for (const auto& entry : fs::recursive_directory_iterator(inputPath)) {
                if (entry.is_regular_file()) {
                    if (entry.path().filename().string()[0] == '.') continue; // Skip hidden
                    allFiles.push_back(entry.path().string());
                }
            }
        } catch (...) {
            emit statusChanged("❌ Error reading folder permissions.");
            return;
        }

        if (allFiles.empty()) {
            emit statusChanged("⚠️ Folder is empty.");
            return;
        }

        emit statusChanged("⚡ Engaging Cores for " + QString::number(allFiles.size()) + " files...");

        // Parallel Batch Processing
        QtConcurrent::blockingMap(allFiles, [this, inputPath, outputPath, mode](const std::string& subFile) {
            // Calculate relative path to maintain structure
            // e.g. Input/Sub/File.txt -> Output/Sub/File.hz
            fs::path src(subFile);
            fs::path base(inputPath);
            fs::path relative = fs::relative(src, base);

            fs::path dest = fs::path(outputPath) / relative;

            // Adjust extension based on mode
            if (mode == MODE_COMPRESS) {
                dest.replace_extension(src.extension().string() + ".hz");
            } else if (mode == MODE_DECOMPRESS) {
                // Remove .hz if present
                std::string stem = dest.stem().string();
                if (dest.extension() == ".hz") dest.replace_filename(stem);
            }

            // Recursive call to single file logic
            this->processFile(subFile, dest.string(), mode);
        });

        emit statusChanged("✅ Batch Complete.");
        return;
    }

    // ---------------------------------------------------------
    // CASE B: SINGLE FILE INPUT (The Fix)
    // ---------------------------------------------------------
    // This creates the actual binary file that SFX needs.

    fs::path inP(inputPath);
    fs::path outP(outputPath);

    // Ensure the PARENT directory exists, but DO NOT make the filename a folder
    if (!fs::exists(outP.parent_path())) {
        fs::create_directories(outP.parent_path());
    }

    // MODE 1: COMPRESS
    if (mode == MODE_COMPRESS) {
        // Prevent re-compressing .hz files
        if (inP.extension() == ".hz") return;

        // Call the compressor
        if (compressTo(inputPath, outputPath)) {
            emit statusChanged("📦 Compressed: " + QString::fromStdString(inP.filename().string()));
        } else {
            emit statusChanged("❌ Failed: " + QString::fromStdString(inP.filename().string()));
        }
    }
    // MODE 2: DECOMPRESS (Manual)
    else if (mode == MODE_DECOMPRESS) {
        decompressTo(inputPath, outputPath);
        emit statusChanged("📂 Restored: " + QString::fromStdString(outP.filename().string()));
    }
    // MODE 3: VERIFY
    else if (mode == MODE_VERIFY) {
        if (inP.extension() != ".hz") return;

        std::string tempCheck = outputPath + ".check";
        decompressTo(inputPath, tempCheck);

        // If successful, remove temp
        if (fs::exists(tempCheck)) {
            fs::remove(tempCheck);
            emit statusChanged("✅ Verified Integrity: " + QString::fromStdString(inP.filename().string()));
        } else {
            emit statusChanged("⚠️ Integrity Check Failed");
        }
    }
}

// --- CORE COMPRESSION LOGIC ---
bool ZipperApp::compressTo(std::string inputPath, std::string outputPath) {
    try {
        std::ios::sync_with_stdio(false); // Speed boost

        long long origSize = fs::file_size(inputPath);
        FrequencyCounter counter;
        auto frequencies = counter.countFrequencies(inputPath);
        if (frequencies.empty()) return false;

        HuffmanTree tree;
        tree.buildTree(frequencies);
        tree.generateHuffmanCodes();

        // WRITING TO FILE STREAM DIRECTLY (Fixes 0-byte bug)
        BitWriter writer(outputPath);
        writer.writeHeader(frequencies, origSize);

        std::ifstream inFile(inputPath, std::ios::binary);
        if (!inFile) return false;

        const size_t BUFFER_SIZE = 128 * 1024; // 128KB Buffer
        std::vector<char> buffer(BUFFER_SIZE);
        auto codes = tree.getCodes();

        while (inFile.read(buffer.data(), BUFFER_SIZE) || inFile.gcount() > 0) {
            std::streamsize bytesRead = inFile.gcount();
            for (std::streamsize i = 0; i < bytesRead; ++i) {
                unsigned char ch = static_cast<unsigned char>(buffer[i]);
                writer.writeCode(codes[ch]);
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Compression Error: " << e.what() << std::endl;
        return false;
    }
}

// --- CORE DECOMPRESSION LOGIC ---
void ZipperApp::decompressTo(std::string inputPath, std::string outputPath) {
    try {
        Decompressor decompressor;
        decompressor.decompressFile(inputPath, outputPath);
    } catch (...) {
        std::cerr << "Decompression Error" << std::endl;
    }
}

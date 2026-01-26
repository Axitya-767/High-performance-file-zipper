#include "Utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

// Helper to handle paths cleanly
std::string getSmartOutputPath(const std::string& inputPathStr, bool isCompressing) {
    fs::path inputPath(inputPathStr);
    std::string fullName = inputPath.filename().string();
    fs::path parent = inputPath.parent_path();

    if (isCompressing) return (parent / (fullName + ".hz")).string();
    return (parent / (fullName + "_Extracted")).string();
}

void createSFX(const std::string& hzFilePath, const std::string& stubPath) {
    fs::path hzPath(hzFilePath);

    // Naming: "File.pdf.hz" -> "File.pdf_Compressed.command"
    std::string baseName = hzPath.stem().string();
    std::string finalExeName = hzPath.parent_path().string() + "/" + baseName + "_Compressed.command";

    if (!fs::exists(stubPath)) {
        std::cerr << "CRITICAL: Stub missing at " << stubPath << std::endl;
        return;
    }

    std::ifstream stubIn(stubPath, std::ios::binary);
    std::ifstream dataIn(hzFilePath, std::ios::binary);
    std::ofstream sfxOut(finalExeName, std::ios::binary);

    // 1. Write the Stub Executable
    sfxOut << stubIn.rdbuf();

    // 2. Write the Marker
    // This separates code from data. Must match Stub.cpp exactly.
    std::string marker = "|||HZ_DATA_START|||";
    sfxOut.write(marker.c_str(), marker.size());

    // 3. Write the Compressed Data
    // Note: dataIn already starts with the correct Header (Size + Map)
    // because BitWriter wrote it that way. We just copy it.
    sfxOut << dataIn.rdbuf();

    sfxOut.close();

    // Make executable
    fs::permissions(finalExeName, fs::perms::owner_all | fs::perms::group_exec | fs::perms::others_exec);

    // Cleanup the .hz file
    fs::remove(hzFilePath);

    std::cout << "Created SFX: " << finalExeName << std::endl;
}

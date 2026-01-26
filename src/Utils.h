#ifndef UTILS_H
#define UTILS_H

#include <string>

// Generates the correct output folder name
std::string getSmartOutputPath(const std::string& inputPathStr, bool isCompressing);

// Joins the compiled stub and the compressed data into an executable
void createSFX(const std::string& compressedFilePath, const std::string& stubPath);

#endif
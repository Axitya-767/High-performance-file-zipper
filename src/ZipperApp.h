#ifndef ZIPPERAPP_H
#define ZIPPERAPP_H

#include <QObject>
#include <QString>
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

namespace fs = std::filesystem;

class ZipperApp : public QObject {
    Q_OBJECT

public:
    explicit ZipperApp(QObject *parent = nullptr) : QObject(parent) {}

    // The new "One Go" function
    void processOneGoCycle(const std::string& inputPath, const std::string& outputDir);

signals:
    void progressUpdated(int percent);
    void statusChanged(QString message);

private:
    // Updated to allow custom output names (like .new)
    void compressTo(std::string inputPath, std::string outputPath);
    void decompressTo(std::string inputPath, std::string outputPath);

    void printReport(double durationSeconds);
    std::string formatBytes(long long bytes);

    long long totalOriginalSize = 0;
    long long totalCompressedSize = 0;
    int processedFiles = 0;
    static std::mutex consoleMutex;
};

#endif

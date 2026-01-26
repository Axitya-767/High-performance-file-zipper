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

    // 1. Define Modes
    enum AppMode {
        MODE_COMPRESS,
        MODE_DECOMPRESS,
        MODE_VERIFY
    };

    // 2. Declare the main processor
    void processFile(const std::string& inputPath, const std::string& outputDir, AppMode mode);

signals:
    void progressUpdated(int percent);
    void statusChanged(QString message);

private:
    bool compressTo(std::string inputPath, std::string outputPath);
    void decompressTo(std::string inputPath, std::string outputPath);

    // Helpers
    void printReport(double) {}
    std::string formatBytes(long long) { return ""; }

    static std::mutex consoleMutex;
};

#endif

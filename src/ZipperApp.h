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
    // NEW: Define the modes
    enum AppMode {
        MODE_COMPRESS,
        MODE_DECOMPRESS,
        MODE_VERIFY // The "Check Integrity" cycle
    };

    explicit ZipperApp(QObject *parent = nullptr) : QObject(parent) {}

    // UPDATE: Now accepts a 'mode' parameter
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

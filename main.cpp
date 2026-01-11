#include <iostream>
#include "src/ZipperApp.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage:\n  ./zipper -c (Compress)\n  ./zipper -d (Decompress)\n";
        return 1;
    }

    std::string flag = argv[1];
    ZipperApp app;

    if (flag == "-c") {
        // Reads from 'data', writes to 'archives'
        app.processDirectory("data", "archives", true);
    } 
    else if (flag == "-d") {
        // Reads from 'archives', writes back to 'data'
        app.processDirectory("archives", "data", false);
    } 
    else {
        std::cerr << "❌ Invalid argument. Use -c or -d.\n";
        return 1;
    }

    return 0;
}
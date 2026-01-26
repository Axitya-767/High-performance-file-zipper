# FileZipper

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Framework](https://img.shields.io/badge/framework-Qt%206-green.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows%20%7C%20Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

**FileZipper** is a high-performance, multithreaded file compression utility that generates **self-extracting executables**. Built with C++17 and the Qt 6 framework, it implements a custom Huffman Coding algorithm to provide lossless compression without requiring the recipient to have any special software installed.

## 📖 Overview

Traditional compression tools (like zip/tar) require the end-user to have a compatible extractor. FileZipper solves this by bundling a lightweight **Decompression Stub** directly with the compressed payload.

When you compress a file with FileZipper, the output is not a `.zip` archive, but a **standalone executable** (e.g., `archive.app` or `archive.exe`). When the recipient runs this executable, it automatically reconstructs the original file.

## 🏗 Technical Architecture

The system is modularized into three core components, leveraging `QtConcurrent` for non-blocking UI operations.

### 1. The Encoder (Compression Engine)
Located in `src/HuffmanTree.cpp` and `src/BitWriter.cpp`.
* **Frequency Analysis:** Scans the input file to build a frequency map of bytes ($O(N)$).
* **Tree Construction:** Uses a priority queue to build a canonical Huffman Tree, assigning shorter binary prefixes to frequent characters.
* **Bit Packing:** Compresses data bits into a dense binary stream, handling padding for byte alignment.

### 2. The Injector (Stub Mechanism)
Located in `src/ZipperApp.cpp` and `src/Utils.cpp`.
* FileZipper maintains a pre-compiled binary called the **Stub**.
* **Binary Injection:** When compressing, the app reads the `stub_executable`, writes it to a new file, appends a unique magic marker (`|||HZ_DATA_START|||`), and then streams the compressed Huffman payload immediately after it.
* **Result:** A hybrid binary that is valid executable code at the start, and compressed data at the end.

### 3. The Decoder (The Stub)
Located in `tools/Stub.cpp`.
* A lightweight C++ runtime with zero external dependencies.
* **Self-Reflection:** When executed, the Stub opens its own binary file (`argv[0]`) in read-mode.
* **Seek & Extract:** It scans for the magic marker, deserializes the Huffman Tree found in the header, and decodes the remaining bitstream back to the original file.

## 🛠 Tech Stack

* **Language:** C++17
* **GUI Framework:** Qt 6 (Widgets)
* **Concurrency:** `QtConcurrent` (for background compression tasks)
* **Build System:** qmake
* **Deployment:** `macdeployqt` (macOS), `windeployqt` (Windows)

## 📂 Repository Structure

```text
├── src/
│   ├── Main.cpp               # Application Entry Point
│   ├── MainWindow.cpp         # UI Logic (Qt Slots/Signals)
│   ├── FrequencyCounter.cpp   # Byte frequency analysis
│   ├── HuffmanTree.cpp        # Tree construction algorithm
│   ├── BitWriter.cpp          # Low-level bit manipulation
│   ├── Decompressor.cpp       # Logic for unpacking (internal testing)
│   └── Utils.cpp              # File I/O helpers
├── tools/
│   └── Stub.cpp               # The standalone extraction engine
├── tests/                     # Sample data for validation
├── FileZipper.pro             # QMake Project Configuration
└── README.md                  # Project Documentation

🚀 Build Instructions
Prerequisites
Qt Creator (with Qt 6.x installed)

C++ Compiler (Clang, GCC, or MSVC)

Step 1: Clone the Repository
Bash
git clone [https://github.com/Axitya-767/FileZipper.git](https://github.com/Axitya-767/FileZipper.git)
cd FileZipper
Step 2: Compile the Stub
The "Stub" is the engine that gets attached to your compressed files. It must be built independently before running the main app.

macOS / Linux:

Bash
g++ tools/Stub.cpp -O3 -o tools/stub_executable -std=c++17
Windows (MinGW/MSVC):

Bash
g++ tools/Stub.cpp -O3 -o tools/stub_executable.exe -std=c++17
Step 3: Build & Run the App
Open FileZipper.pro in Qt Creator.

When prompted to configure the project, select your preferred kit (e.g., Desktop Qt 6.8.0).

Select Release configuration (bottom left monitor icon) for optimal performance.

Click Run (Green Play Button).

🤝 Contributing
Contributions are welcome! Please ensure you follow the existing code style.

Fork the Project

Create your Feature Branch (git checkout -b feature/AmazingFeature)

Commit your Changes (git commit -m 'Add some AmazingFeature')

Push to the Branch (git push origin feature/AmazingFeature)

Open a Pull Request

📄 License
Distributed under the MIT License. See LICENSE for more information.

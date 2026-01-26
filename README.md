# FileZipper

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Framework](https://img.shields.io/badge/framework-Qt%206-green.svg)
![Concurrency](https://img.shields.io/badge/concurrency-QtConcurrent-purple.svg)
![Platform](https://img.shields.io/badge/platform-macOS%20%7C%20Windows-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

**FileZipper** is a high-performance, multi-threaded compression utility engineered to produce self-extracting executables. Built with **C++17** and **Qt 6**, it leverages **QtConcurrent** to utilize multi-core processing for non-blocking UI operations, ensuring the interface remains responsive even during heavy compression tasks.

## 📖 Overview

Standard compression tools (like WinZip or 7-Zip) require the recipient to have compatible software installed. FileZipper solves this by encapsulating the compressed payload and a lightweight extraction engine (the "Stub") into a single executable binary.

When executed, the archive acts as its own decompressor, reconstructing the original files with bit-perfect accuracy.

## 🏗 Architecture & Design

The system is modularized into three core components, separating the UI thread from the worker threads.

### 1. The Encoder (Compression Engine)
Located in `src/HuffmanTree.cpp` and `src/BitWriter.cpp`.
* **Multi-Core Utilization:** Uses `QtConcurrent` to offload intensive frequency analysis and tree construction to worker threads, preventing the main GUI thread from freezing.
* **Frequency Analysis:** Scans input data to calculate byte frequency ($O(N)$).
* **Tree Construction:** Builds a canonical Huffman Tree to assign variable-length prefix codes.
* **Bit Packing:** Compresses data bits into a dense binary stream with optimized padding.

### 2. The Injector (Stub Mechanism)
Located in `src/ZipperApp.cpp` and `src/Utils.cpp`.
* **Binary Injection:** The application reads a pre-compiled "Stub" binary.
* **Payload Assembly:** It appends a unique delimiter (`|||HZ_DATA_START|||`) followed by the compressed Huffman bitstream directly to the end of the Stub.
* **Result:** A hybrid binary that functions as a standard executable but carries a hidden payload.

### 3. The Decoder (The Stub)
Located in `tools/Stub.cpp`.
* A minimal C++ runtime with **zero external dependencies**.
* **Self-Reflection:** When executed, the Stub opens its own binary file (`argv[0]`) in read-mode.
* **Seek & Extract:** It finds the magic marker, deserializes the Huffman Tree, and decodes the payload back to the original file.

## 🛠 Tech Stack

* **Language:** C++17
* **GUI Framework:** Qt 6.8 (Widgets)
* **Concurrency:** `QtConcurrent` (Task-based parallelism)
* **Build System:** QMake
* **Deployment:** `macdeployqt` (macOS), `windeployqt` (Windows)

## ⚡ Performance

* **Time Complexity:** $O(N \log N)$ for tree construction.
* **Space Complexity:** $O(K)$ constant space for the alphabet size (256 bytes).
* **Threading:** Asynchronous execution model ensures 60fps UI performance during large file operations.

## 🚀 Build Instructions

### Prerequisites
* **Qt 6.x** (Tested on 6.8)
* **C++ Compiler** (Clang for macOS, MSVC/MinGW for Windows)

### 1. Clone the Repository
```bash
git clone [https://github.com/Axitya-767/FileZipper.git](https://github.com/Axitya-767/FileZipper.git)
cd FileZipper

2. Compile the Stub
The extraction engine must be compiled independently before building the main application.

macOS / Linux:
Bash
g++ tools/Stub.cpp -O3 -o tools/stub_executable -std=c++17

Windows:
Bash
g++ tools/Stub.cpp -O3 -o tools/stub_executable.exe -std=c++17

3. Build the Main Application
Open FileZipper.pro in Qt Creator.

Configure the project for Release mode.

Run qmake followed by Build (or click the Green Play button).

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

🤝 Contributing
Contributions to further optimize the Huffman tree generation or add support for directory compression are welcome.

Fork the project.
Create your feature branch (git checkout -b feature/Optimization).
Commit your changes.
Push to the branch and open a Pull Request.

📄 License
Distributed under the MIT License. See LICENSE for more information.

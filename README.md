# FileZipper

![Language](https://img.shields.io/badge/language-C%2B%2B17-blue.svg)
![Framework](https://img.shields.io/badge/framework-Qt%206-green.svg)
![Concurrency](https://img.shields.io/badge/concurrency-QtConcurrent-purple.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

**FileZipper** is a high-performance compression utility offering a hybrid approach to file management. It features **Self-Extracting Archives** for single files and a robust **Decompression Mode** for handling complex directory structures. Built with **C++17** and **Qt 6**, it leverages multi-threading to ensure maximum performance.

## 📖 Key Features

* **Self-Extracting Files:** Convert any single file into a standalone executable that extracts itself. No software needed for the recipient.
* **Folder Archiving:** Compress entire directories into secure archives.
* **Integrated Decompressor:** A dedicated mode within the app to restore folder hierarchies from archives.
* **Multi-Core Processing:** Utilizes `QtConcurrent` to parallelize compression tasks, keeping the UI responsive.

## 🏗 System Architecture

The application handles two distinct workflows depending on the input type:

### 1. The Encoder (Compression Engine)
* **Core Logic:** Implements Canonical Huffman Coding ($O(N \log N)$ complexity).
* **Parallelization:** Frequency analysis is offloaded to worker threads to prevent UI freezing.
* **Bit-Level Optimization:** Custom `BitWriter` ensures efficient binary packing.

### 2. The Injector (Single File Mode)
* For single files, FileZipper appends the compressed payload to a pre-compiled **Stub**.
* **The Stub:** A lightweight executable that reads its own binary data (`argv[0]`) to self-extract.

### 3. The Decompressor (Folder Mode)
* Located in `src/Decompressor.cpp`.
* **Archive Handling:** For folders, the app generates a custom archive format.
* **Restoration:** The built-in Decompressor reads the header metadata to reconstruct the directory tree and place files in their correct paths.

## 🛠 How to Use

### Mode A: Single File (Self-Extracting)
1.  Click **"Select File"**.
2.  Choose your destination. The app creates a standalone `.command` (macOS) or `.exe` (Windows) file.
3.  **To Extract:** The recipient simply double-clicks the file. It extracts automatically without needing FileZipper.

### Mode B: Folders (Archive Mode)
1.  Click **"Select Folder"**.
2.  The app compresses the entire directory structure into an archive file.
3.  **To Extract:**
    * Open FileZipper.
    * Switch to **Decompress Mode**.
    * Select the archive file and click **Decompress**. The folder structure will be restored.


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

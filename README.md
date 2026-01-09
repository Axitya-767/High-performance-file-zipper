# High-Performance File Zipper

A C++17 implementation of the Huffman Coding algorithm for lossless file compression. This project focuses on systems-level programming, utilizing custom data structures and low-level bit manipulation to optimize storage efficiency.

## Project Overview

The goal of this project is to build a compression tool from first principles. Unlike wrappers around existing libraries (like zlib), this implementation constructs the compression engine from scratch. It reads raw binary data, analyzes character frequency, builds an optimal prefix tree (Huffman Tree), and generates a compressed binary output.

## Technical Architecture

The project is structured into modular components to separate the interface from the implementation logic.

### Directory Structure

* **src/**: Contains the core logic and data structure definitions.
    * `HuffmanNode.h`: Defines the structure of the tree nodes used in the Huffman algorithm.
    * `FrequencyCounter.h` / `.cpp`: Handles binary file I/O and generates the frequency map required for the priority queue.
* **main.cpp**: The driver program that initializes the compression workflow.
* **input.txt**: Test data for validating compression logic.

### Technology Stack

* **Language:** C++17 (Utilizing structured bindings and modern STL features).
* **Core Algorithms:** Huffman Coding (Greedy Strategy).
* **Data Structures:**
    * `std::map`: For initial frequency counting.
    * `std::priority_queue` (Min-Heap): For efficient tree construction.
    * Binary Trees: For prefix code generation.

## Project Status

**Current Phase: Phase 1 (Completed)**
* Implemented binary file reading capabilities.
* Developed the frequency analysis engine to map character occurrences.
* Integrated C++17 standards for safer memory handling and cleaner syntax.

**Upcoming Phases**
* **Phase 2:** Implementation of the Min-Heap to construct the Huffman Tree.
* **Phase 3:** Recursive traversal of the tree to generate variable-length binary codes.
* **Phase 4:** Implementation of a bit-writer to serialize the compressed data to disk.

## Build and Usage

### Prerequisites
* A C++ compiler supporting C++17 (GCC, Clang, or MSVC).
* Make (optional, for future build automation).

### Compilation
To compile the source code, run the following command from the root directory:

```bash
g++ -std=c++17 main.cpp src/FrequencyCounter.cpp -o zipper
./zipper

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <queue>
#include <cstdlib>

namespace fs = std::filesystem;

const size_t BUFFER_SIZE = 1024 * 1024; // 1MB Buffer for Speed

// CRITICAL: Struct matches your HuffmanTree.cpp logic
struct HuffmanNode {
    char character;
    int frequency;
    long long sequence; // Added to match your Compressor's stability logic
    HuffmanNode *left, *right;

    HuffmanNode(char c, int f, long long seq) 
        : character(c), frequency(f), sequence(seq), left(nullptr), right(nullptr) {}
};

// CRITICAL: Comparator that respects Sequence ID
// This ensures the tree is built identically to the compressor.
struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        if (l->frequency != r->frequency) {
            return l->frequency > r->frequency; // Smallest freq on top
        }
        return l->sequence > r->sequence; // Smallest sequence ID on top (FIFO stability)
    }
};

HuffmanNode* rebuildTree(std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> pq;
    
    // EXACT MATCH of your HuffmanTree.cpp logic:
    long long seqCounter = 0;

    // 1. Create Leaf Nodes
    for (auto const& [ch, freq] : frequencies) {
        pq.push(new HuffmanNode(ch, freq, seqCounter++));
    }

    // 2. Build Tree
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        // Internal node gets next sequence ID
        HuffmanNode* parent = new HuffmanNode('$', left->frequency + right->frequency, seqCounter++);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }
    return pq.top();
}

void extract(std::istream& in, const std::string& outPath) {
    long long originalSize; 
    int mapSize;
    
    // HEADER: Must match BitWriter.cpp exactly
    // 1. Original Size (8 bytes)
    in.read(reinterpret_cast<char*>(&originalSize), sizeof(long long));
    
    // 2. Map Size (4 bytes)
    in.read(reinterpret_cast<char*>(&mapSize), sizeof(int));
    
    if (mapSize < 0 || mapSize > 256) {
        std::cerr << "Error: Corrupt Header (MapSize: " << mapSize << ")." << std::endl;
        return; 
    }

    // 3. Map Data
    std::map<char, int> freqs;
    for (int i = 0; i < mapSize; ++i) {
        char c; int f;
        in.read(&c, sizeof(char)); 
        in.read(reinterpret_cast<char*>(&f), sizeof(int));
        freqs[c] = f;
    }

    if (freqs.empty()) return;

    // Rebuild Tree using Sequence Logic
    HuffmanNode* root = rebuildTree(freqs);
    HuffmanNode* curr = root;
    
    std::ofstream outFile(outPath, std::ios::binary);
    std::vector<char> inBuf(BUFFER_SIZE);
    std::vector<char> outBuf(BUFFER_SIZE);
    size_t outIdx = 0;
    long long total = 0;

    // DECOMPRESSION LOOP (Matches BitWriter MSB logic)
    while (in.read(inBuf.data(), BUFFER_SIZE) || in.gcount() > 0) {
        size_t n = in.gcount();
        for (size_t i = 0; i < n; ++i) {
            unsigned char byte = static_cast<unsigned char>(inBuf[i]);
            
            // Process bits 7 down to 0 (MSB First)
            for (int b = 7; b >= 0; --b) {
                
                // STOP immediately if we have enough bytes
                // (Ignores padding at end of file)
                if (total == originalSize) goto Done;

                int bit = (byte >> b) & 1;
                curr = bit ? curr->right : curr->left; // 1 = Right, 0 = Left (Standard)

                if (!curr->left && !curr->right) {
                    outBuf[outIdx++] = curr->character;
                    total++;
                    curr = root;

                    if (outIdx >= BUFFER_SIZE) {
                        outFile.write(outBuf.data(), BUFFER_SIZE);
                        outIdx = 0;
                    }

                    if (total == originalSize) goto Done;
                }
            }
        }
    }

Done:
    if (outIdx > 0) outFile.write(outBuf.data(), outIdx);
}

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    
    fs::path exePath = fs::absolute(argv[0]);
    fs::path targetDir = exePath.parent_path();
    
    // Name Recovery
    std::string filename = exePath.filename().string();
    size_t cmdPos = filename.find(".command");
    if (cmdPos != std::string::npos) filename = filename.substr(0, cmdPos);
    
    size_t tagPos = filename.find("_Compressed");
    if (tagPos != std::string::npos) {
        filename = filename.substr(0, tagPos);
    } else {
        filename += "_Restored";
    }

    fs::path outPath = targetDir / filename;
    if (fs::exists(outPath)) outPath = targetDir / ("Extracted_" + filename);

    std::ifstream self(exePath, std::ios::binary);
    
    // Marker Scan
    // Matches "|||HZ_DATA_START|||"
    std::string marker;
    marker += '|'; marker += '|'; marker += '|';
    marker += 'H'; marker += 'Z'; marker += '_';
    marker += 'D'; marker += 'A'; marker += 'T'; marker += 'A'; marker += '_';
    marker += 'S'; marker += 'T'; marker += 'A'; marker += 'R'; marker += 'T';
    marker += '|'; marker += '|'; marker += '|';
    
    std::string buf = "";
    char ch;
    bool found = false;

    while (self.get(ch)) {
        buf += ch;
        if (buf.length() > marker.length()) buf.erase(0, 1);
        if (buf == marker) { found = true; break; }
    }

    if (found) {
        std::cout << "Extracting to: " << filename << std::endl;
        extract(self, outPath.string());
        system("osascript -e 'tell application \"Terminal\" to close first window' & exit");
    } else {
        std::cout << "Error: Marker not found." << std::endl;
        std::cin.get();
    }

    return 0;
}
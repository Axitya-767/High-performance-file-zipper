#ifndef HUFFMAN_NODE_H
#define HUFFMAN_NODE_H

struct HuffmanNode {
    char character;
    int frequency;
    long long sequence; // NEW: Unique ID for stability
    HuffmanNode* left;
    HuffmanNode* right;

    // Updated Constructor: Accepts sequence number
    HuffmanNode(char ch, int freq, long long seq) {
        character = ch;
        frequency = freq;
        sequence = seq;
        left = nullptr;
        right = nullptr;
    }
};

// CRITICAL: The "Sequence" Tie-Breaker
struct Compare {
    bool operator()(HuffmanNode* left, HuffmanNode* right) {
        // 1. Primary Sort: Frequency
        if (left->frequency != right->frequency) {
            return left->frequency > right->frequency;
        }
        // 2. Secondary Sort: Sequence ID (First-Created is First-Processed)
        // This solves the "Internal Node" collision bug perfectly.
        return left->sequence > right->sequence;
    }
};

#endif

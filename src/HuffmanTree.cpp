#include "HuffmanTree.h"

// --- Phase 2 Logic (Keep this) ---
void HuffmanTree::buildTree(const std::map<char, int>& frequencies) {
    // Min-Heap with our new stable Comparator
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> minHeap;

    // THE COUNTER: Ensures every node has a unique ID
    long long seqCounter = 0;

    // 1. Create Leaf Nodes (Assign Sequence IDs)
    for (auto const& [key, val] : frequencies) {
        minHeap.push(new HuffmanNode(key, val, seqCounter++));
    }

    // 2. Build Tree
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();

        // Internal Node gets the next Sequence ID
        HuffmanNode* top = new HuffmanNode('$', left->frequency + right->frequency, seqCounter++);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }

    root = minHeap.top();
}

// --- Phase 3 Logic (New) ---
void HuffmanTree::generateCodes(HuffmanNode* node, std::string code) {
    if (!node) return;

    // CRITICAL FIX: Distinguish Leaf Nodes by structure, not value.
    // Real data nodes have NO children. Internal nodes have children.
    // OLD BROKEN CODE: if (node->character != '$')

    if (!node->left && !node->right) {
        huffmanCodes[node->character] = code;
    }

    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}

void HuffmanTree::generateHuffmanCodes() {
    generateCodes(root, "");
}

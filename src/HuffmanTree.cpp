#include "HuffmanTree.h"

// --- Phase 2 Logic (Keep this) ---
void HuffmanTree::buildTree(const std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> minHeap;

    for (auto const& [key, val] : frequencies) {
        minHeap.push(new HuffmanNode(key, val));
    }

    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top(); minHeap.pop();
        HuffmanNode* right = minHeap.top(); minHeap.pop();

        HuffmanNode* top = new HuffmanNode('$', left->frequency + right->frequency);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    root = minHeap.top();
}

// --- Phase 3 Logic (New) ---
void HuffmanTree::generateCodes(HuffmanNode* node, std::string code) {
    if (!node) return;

    // If it is a leaf node (contains a real character)
    if (node->character != '$') {
        huffmanCodes[node->character] = code;
    }

    // Traverse Left (0) and Right (1)
    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}

void HuffmanTree::generateHuffmanCodes() {
    generateCodes(root, "");
}
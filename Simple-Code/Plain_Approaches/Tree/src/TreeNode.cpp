#include "TreeNode.h"
/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
 
TreeNode* buildTreeFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return NULL;
    }

    std::string line;
    std::getline(file, line); // Read the first line
    if (line.empty()) return NULL;

    std::istringstream iss(line);
    std::string value;
    std::queue<TreeNode*> nodeQueue;

    // Create the root node
    std::getline(iss, value, ',');
    TreeNode* root = new TreeNode(std::stoi(value));
    nodeQueue.push(root);

    // Build the tree using level order traversal
    while (!nodeQueue.empty() && std::getline(iss, value, ',')) {
        TreeNode* current = nodeQueue.front();
        nodeQueue.pop();

        // Left child
        if (value != "null") {
            TreeNode* leftChild = new TreeNode(std::stoi(value));
            current->left = leftChild;
            nodeQueue.push(leftChild);
        }

        // Check for the next value for the right child
        if (std::getline(iss, value, ',')) {
            if (value != "null") {
                TreeNode* rightChild = new TreeNode(std::stoi(value));
                current->right = rightChild;
                nodeQueue.push(rightChild);
            }
        }
    }

    file.close();
    return root;
}

TreeNode* findNode(TreeNode* root, int value) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->val == value) {
        return root;
    }

    TreeNode* leftSearch = findNode(root->left, value);
    if (leftSearch != nullptr) {
        return leftSearch;
    }

    return findNode(root->right, value);
}

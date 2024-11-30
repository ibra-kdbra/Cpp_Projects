#ifndef TREE_DRAWER_H
#define TREE_DRAWER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <functional>
#include "TreeNode.h"

class TreeDrawer {
public:
    // Function to calculate the height of the tree
    static int getHeight(TreeNode* root) {
        if (!root) return 0;
        return std::max(getHeight(root->left), getHeight(root->right)) + 1;
    }

    // Function to print the tree in a more visually clear way
    static void printTreeDraw(TreeNode* root, std::ofstream& outputFile) {
        int h = getHeight(root);
        int width = (1 << h) - 1; // Maximum width of the tree

        std::vector<std::vector<std::string>> treeLayout(h, std::vector<std::string>(width, " ")); // Initialize 2D array for tree layout

        // Helper function to fill treeLayout with node values
        std::function<void(TreeNode*, int, int, int)> fillTreeLayout = [&](TreeNode* node, int level, int pos, int spacing) {
            if (!node) return;

            treeLayout[level][pos] = std::to_string(node->val);

            // Recursively place the left and right children with appropriate spacing
            fillTreeLayout(node->left, level + 1, pos - spacing / 2, spacing / 2);
            fillTreeLayout(node->right, level + 1, pos + spacing / 2, spacing / 2);
        };

        // Start the filling process, starting with the root node
        fillTreeLayout(root, 0, (width - 1) / 2, width / 2);

        // Write the tree to file
        outputFile << "output: Tree Structure (Visual Representation):\n";
        outputFile << "```plaintext\n";  // Start of code block in Markdown

        // Printing each level of the tree
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < width; ++j) {
                outputFile << treeLayout[i][j] << " ";
            }
            outputFile << "\n";
        }

        outputFile << "\n```";  // End of code block in Markdown
    }
};

#endif // TREE_DRAWER_H

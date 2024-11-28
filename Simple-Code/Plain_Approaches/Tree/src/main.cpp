#include <iostream>
#include <fstream>
#include "TreeNode.h"
#include "Algorithms.h"
#include "TreeDraw.h"

using namespace std;

int main() {
    int choice;
    string filename;

    cout << "Enter the input file name (.md or .txt): ";
    cin >> filename;

    ofstream outputFile(filename, ios::app);
    if (!outputFile.is_open()) {
        cerr << "Error: Cannot open file " << filename << " for writing." << endl;
        return -1;
    }

    TreeNode* root = buildTreeFromFile(filename);
    if (!root) {
        cout << "Failed to build tree from file." << endl;
        return -1;
    }

    cout << "Select an algorithm:" << endl;
    cout << "1. Diameter of Binary Tree" << endl;
    cout << "2. Equal Tree Partition" << endl;
    cout << "3. Flatten Binary Tree" << endl;
    cout << "4. Invert Binary Tree" << endl;
    cout << "5. Lowest Common Ancestor (LCA)" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    int n1, n2;
    int result;

    outputFile << "\ncommand case '" << choice << "' ----------------\n";

    switch (choice) {
        case 1:
            result = solveDiameter(root);
            outputFile << "output: Diameter of the binary tree: " << result << endl;
            break;

        case 2:
            if (isPartitionPossible(root)) {
                outputFile << "output: Equal partition possible" << endl;
            } else {
                outputFile << "output: Equal partition NOT possible" << endl;
            }
            TreeDrawer::printTreeDraw(root, outputFile);
            break;

        case 3:
            root = flattenBinaryTree(root);
            outputFile << "output: Binary tree has been flattened." << endl;
            TreeDrawer::printTreeDraw(root, outputFile);
            break;

        case 4:
            root = invertBinaryTree(root);
            outputFile << "output: Binary tree has been inverted." << endl;
            TreeDrawer::printTreeDraw(root, outputFile);
            break;

        case 5:
            cout << "Enter the two nodes for which you want to find the LCA: ";
            cin >> n1 >> n2;

            result = findLCA(root, n1, n2);
            if (result != -1) {
                outputFile << "output: LCA (First Approach): " << result << endl;
            } else {
                outputFile << "output: No common ancestor found for " << n1 << " and " << n2 << endl;
            }

            result = lca(root, n1, n2);
            if (result != -1) {
                outputFile << "output: LCA (Second Approach): " << result << endl;
            } else {
                outputFile << "output: One or both nodes are not present in the tree." << endl;
            }
            break;

        default:
            outputFile << "output: Invalid choice!" << endl;
            break;
    }
    outputFile.close();

    cout << "The output has been written to " << filename << ". Please check the file for results." << endl;

    return 0;
}


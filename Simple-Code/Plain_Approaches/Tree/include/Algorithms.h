#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "TreeNode.h"

// Diameter of Binary Tree
int solveDiameter(TreeNode* root);

// Equal Tree Partition
bool isPartitionPossible(TreeNode* root);

// Flatten Binary Tree
TreeNode* flattenBinaryTree(TreeNode* root);

// Invert Binary Tree
TreeNode* invertBinaryTree(TreeNode* root);

// LCA Algorithms
int findLCA(TreeNode* root, int n1, int n2); // First approach
int lca(TreeNode* root, int n1, int n2);    // Second approach with auxiliary checks

#endif

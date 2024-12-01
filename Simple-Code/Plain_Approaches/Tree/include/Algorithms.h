#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "TreeNode.h"

// Single traversal algorithm
int solverSingleTraversal(TreeNode* A, int B, int C);

// Stack-based traversal algorithm
int solverWithStacks(TreeNode* A, TreeNode* B);

// Distance between nodes in a binary tree
int distanceBetweenRootAndNode(TreeNode* A, int key);
int distanceBetweenTwoNodes(TreeNode* A, int B, int C);

// Convert sorted array to balanced BST
TreeNode* sortedArrayToBST(const std::vector<int>& A);

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

#include "Algorithms.h"


TreeNode* util(const std::vector<int>& A, int start, int end) {
    if (start > end) return nullptr;
    int mid = start + (end - start) / 2;
    TreeNode* node = new TreeNode(A[mid]);
    node->left = util(A, start, mid - 1);  // Left subtree
    node->right = util(A, mid + 1, end);   // Right subtree
    return node;
}

// Convert sorted array to balanced BST
TreeNode* sortedArrayToBST(const std::vector<int>& A) {
    return util(A, 0, A.size() - 1);
}

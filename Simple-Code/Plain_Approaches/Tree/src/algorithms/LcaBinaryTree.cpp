#include "Algorithms.h"

// First Approach: Single traversal for LCA
int findLCA(TreeNode* root, int n1, int n2) {
    if (root == NULL)
        return -1;

    if (root->val == n1 || root->val == n2)
        return root->val;

    int leftLCA = findLCA(root->left, n1, n2);
    int rightLCA = findLCA(root->right, n1, n2);

    if (leftLCA != -1 && rightLCA != -1)
        return root->val;

    return (leftLCA != -1) ? leftLCA : rightLCA;
}

// Auxiliary function to check if both nodes exist
void isBothNodeExists(TreeNode* root, int n1, int n2, int& count) {
    if (root == NULL || count == 2)
        return;

    if (root->val == n1)
        count++;

    if (root->val == n2)
        count++;

    isBothNodeExists(root->left, n1, n2, count);
    isBothNodeExists(root->right, n1, n2, count);
}

// Second Approach: Check both nodes' existence, then find LCA
int lca(TreeNode* root, int n1, int n2) {
    int count = 0;
    isBothNodeExists(root, n1, n2, count);

    int result = findLCA(root, n1, n2);
    return (count == 2) ? result : -1;
}

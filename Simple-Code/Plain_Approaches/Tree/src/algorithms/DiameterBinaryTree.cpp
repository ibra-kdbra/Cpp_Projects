#include "Algorithms.h"

int diameterOfBinaryTree(TreeNode* root, int* height) {
    if (root == NULL) {
        *height = 0;
        return 0;
    }

    int lh = 0, rh = 0;
    int ld = diameterOfBinaryTree(root->left, &lh);
    int rd = diameterOfBinaryTree(root->right, &rh);

    *height = std::max(lh, rh) + 1;
    return std::max(lh + rh + 1, std::max(ld, rd));
}

int solveDiameter(TreeNode* root) {
    int height = 0;
    return diameterOfBinaryTree(root, &height) - 1;
}

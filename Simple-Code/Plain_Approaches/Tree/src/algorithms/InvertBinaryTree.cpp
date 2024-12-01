//without swapping 2 variable using 3rd variable
#include "Algorithms.h"

TreeNode* invertBinaryTree(TreeNode* root) {
    if (root == NULL)
        return NULL;

    TreeNode* left = invertBinaryTree(root->left);
    TreeNode* right = invertBinaryTree(root->right);

    root->left = right;
    root->right = left;

    return root;
}

//this is the best one in terms of complexity 
//O(n) S(H)
#include "Algorithms.h"

TreeNode* flattenBinaryTree(TreeNode* root) {
    if (root == NULL)
        return NULL;

    TreeNode* left = flattenBinaryTree(root->left);
    TreeNode* right = flattenBinaryTree(root->right);

    if (left != NULL) {
        TreeNode* temp = left;
        while (temp->right != NULL)
            temp = temp->right;

        temp->right = right;
        root->right = left;
        root->left = NULL;
    }

    return root;
}


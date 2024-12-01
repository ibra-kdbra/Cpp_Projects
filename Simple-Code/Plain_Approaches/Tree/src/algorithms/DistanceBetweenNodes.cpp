#include "Algorithms.h"

int distanceBetweenRootAndNode(TreeNode* A, int key) {
    int res = 0;
    while (1) {
        if (key < A->val) {
            res++;
            A = A->left;
        }
        else if (A->val < key) {
            res++;
            A = A->right;
        }
        else
            break;
    }

    return res;
}

// Distance between two nodes in a binary tree
int distanceBetweenTwoNodes(TreeNode* A, int B, int C) {
    while (1) {
        if (B < A->val && C < A->val)
            A = A->left;
        else if (A->val < B && A->val < C)
            A = A->right;
        else
            break;
    }

    return distanceBetweenRootAndNode(A, B) + distanceBetweenRootAndNode(A, C);
}

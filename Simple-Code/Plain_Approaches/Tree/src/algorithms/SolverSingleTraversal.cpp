#include "Algorithms.h"

int solverSingleTraversal(TreeNode* A, int B, int C) {
    if (A == NULL)
        return 0;

    if (A->val < B && A->val < C)
        return solverSingleTraversal(A->right, B, C);
    else if (A->val > B && A->val > C)
        return solverSingleTraversal(A->left, B, C);
    else if (A->val == B && A->val == C)
        return 1;

    return 1 + solverSingleTraversal(A->left, B, C) + solverSingleTraversal(A->right, B, C);
}

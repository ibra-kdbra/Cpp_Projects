#include "Algorithms.h"

int findTotalSum(TreeNode* root) {
    if (root == NULL)
        return 0;
    return root->val + findTotalSum(root->left) + findTotalSum(root->right);
}

bool checkPartition(TreeNode* root, int totalSum, int& currentSum) {
    if (root == NULL)
        return false;

    currentSum += root->val;

    if (currentSum == totalSum / 2) {
        return true;
    }

    bool leftPartition = checkPartition(root->left, totalSum, currentSum);
    bool rightPartition = checkPartition(root->right, totalSum, currentSum);

    currentSum -= root->val;

    return leftPartition || rightPartition;
}

bool isPartitionPossible(TreeNode* root) {
    int totalSum = findTotalSum(root);
    
    if (totalSum % 2 != 0) {
        return false;
    }

    int currentSum = 0;
    return checkPartition(root, totalSum, currentSum);
}


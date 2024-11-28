/*corner test case when sum of all nodes is 0 (if you will check for root also 
and it will return always true as sum and sum/2 are same if sum=0)*/

#include "Algorithms.h"

int findTotalSum(TreeNode* root) {
    if (root == NULL)
        return 0;

    return root->val + findTotalSum(root->left) + findTotalSum(root->right);
}

bool checkPartition(TreeNode* root, int totalSum, int& currentSum) {
    if (root == NULL) return false;

    int leftSum = 0, rightSum = 0;

    bool left = checkPartition(root->left, totalSum, leftSum);
    bool right = checkPartition(root->right, totalSum, rightSum);

    currentSum = root->val + leftSum + rightSum;

    if (currentSum == totalSum / 2)
        return true;

    return left || right;
}

bool isPartitionPossible(TreeNode* root) {
    int totalSum = findTotalSum(root);
    if (totalSum % 2 != 0)
        return false;

    int currentSum = 0;
    return checkPartition(root, totalSum, currentSum);
}

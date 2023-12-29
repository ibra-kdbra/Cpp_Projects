/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

//without swapping 2 variable using 3rd variable
TreeNode* Solution::invertTree(TreeNode* A) {
    if(A==NULL)
        return NULL;
        
    TreeNode* leftSubTreeRoot=invertTree(A->left);
    TreeNode* rightSubTreeRoot=invertTree(A->right);
    
    A->left=rightSubTreeRoot;
    A->right=leftSubTreeRoot;
    
    return A;
}

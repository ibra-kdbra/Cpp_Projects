/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

//1st approach
int diameterOfBinaryTree(TreeNode* root,int *height) {
    if(root==NULL) {
        *height=0;
        return 0;
    }
    
    int lh=0,rh=0;
    int ld=diameterOfBinaryTree(root->left,&lh);
    int rd=diameterOfBinaryTree(root->right,&rh);
    
    *height=max(lh,rh)+1;
    return max(lh+rh+1,max(ld,rd));
}
int Solution::solve(TreeNode* A) {
    int height=0;
    return diameterOfBinaryTree(A,&height)-1;
}

//2nd approach
int height(TreeNode* root,int& diameter)
{
    if(root==NULL)return 0;
    int leftHeight=height(root->left,diameter);
    int rightHeight=height(root->right,diameter);
    diameter=max(diameter,1+leftHeight+rightHeight);
    return 1+max(leftHeight,rightHeight);
}
int Solution::solve(TreeNode* A) {
    int diameter=INT_MIN;
    height(A,diameter);
    return diameter-1;
}
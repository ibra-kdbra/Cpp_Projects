/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

TreeNode* util(const vector<int> &A,int l,int r) {
    if(l>r)
        return NULL;
        
    int mid=l+(r-l)/2;
    TreeNode* node=new TreeNode(A[mid]);
    node->left=util(A,l,mid-1);
    node->right=util(A,mid+1,r);
    
    return node;
}
TreeNode* Solution::sortedArrayToBST(const vector<int> &A) {
    return util(A,0,A.size()-1);
}

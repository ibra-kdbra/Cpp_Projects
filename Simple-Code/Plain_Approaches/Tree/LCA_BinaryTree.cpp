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
//Did 1 traversal



//2nd approach
//Did 2 traversals
int findLCA(TreeNode* A, int B, int C) {
    if(A==NULL)
        return -1;
    
    if(A->val==B || A->val==C) {
        return A->val;
    }
        
    int left=findLCA(A->left,B,C);
    int right=findLCA(A->right,B,C);
    
    if(left!=-1 && right!=-1)
        return A->val;
        
    return (left!=-1 ? left : right);
}
void isBothNodeExists(TreeNode* A, int B, int C, int &cnt) {
    if(A==NULL || cnt==2)
        return ;
        
    if(A->val==B)
        cnt++;
    
     if(A->val==C) 
        cnt++;
     
    isBothNodeExists(A->left,B,C,cnt);
    isBothNodeExists(A->right,B,C,cnt);

    return ;
}
//did only one traversal to find whether both B and C exists or not
int Solution::lca(TreeNode* A, int B, int C) {
    int cnt=0;
    isBothNodeExists(A,B,C,cnt);
    
    int res=findLCA(A,B,C);
    if(cnt==2)
        return res;

    return -1;
}

//EXTRA
//3 traversal will be required for this
/*We can also use the following find function to check whether both key exist or not*/
bool find(TreeNode *root, int k)
{
    if(root == NULL)
        return false;

    if(root->val==k || find(root->left, k) || find(root->right, k))
        return true;

    return false;
}

/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
 
//3 methods (2 methods in complete solution for c++ and java) to solve this question 
//See other 2 methods as well
//this is the best one in terms of complexity 

//O(n) S(H)
pair<TreeNode*, TreeNode*> util(TreeNode *A) {
    if(A==NULL) {
        TreeNode *temp=NULL;
        return make_pair(temp,temp);   
    }
    
    pair<TreeNode*, TreeNode*> left=util(A->left);
    pair<TreeNode*, TreeNode*> right=util(A->right);
    A->left=NULL;
    
    if(left.first!=NULL) {
        A->right=left.first;
        (left.second)->right=right.first;
    }
        
    if(right.first!=NULL)    
        return make_pair(A,right.second); //right subtree exist
    else if(left.first!=NULL)
        return make_pair(A,left.second); //no right subtree but left subtree exist
        
    return make_pair(A,A); //no left subtree and no right subtree
} 
 
TreeNode* Solution::flatten(TreeNode* A) {
    TreeNode* root=util(A).first;
    return root;
}

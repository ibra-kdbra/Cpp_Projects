/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

/*
Given a binary search tree of integers. You are given a range [B, C]. 
Return the count of the number of nodes that lies in this range. 

Constraints
1 <= Number of nodes in binary tree <= 100000
0 <= B < = C <= 10^9 
*/

//single traversal
//not going through all the nodes
int Solution::solve(TreeNode* A, int B, int C) {
    if(A==NULL)
        return 0;
        
    if(A->val<B && A->val<C)
        return solve(A->right,B,C);
    else if(A->val>B && A->val>C)
        return solve(A->left,B,C);
    else if(A->val==B && A->val==C) //will work if remove this condition (avoiding extra traversal if B==C)
        return 1;
    
    return 1+solve(A->left,B,C)+solve(A->right,B,C);
}

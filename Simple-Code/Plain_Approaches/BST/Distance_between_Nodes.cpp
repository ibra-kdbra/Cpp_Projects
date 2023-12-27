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
O(H) S(1)
It is assumed that both keys exists in the tree (that's why not checking if(root!=NULL))
*/

//Iterative solution
int distanceBetweenRootAndNode(TreeNode* A, int key) {
    int res=0;
    while(1) {
        if(key<A->val) {
            res++;
            A=A->left;
        }
        else if(A->val<key) {
            res++;
            A=A->right;
        }
        else
            break;
    }

    return res;
}
int distanceBetweenTwoNodes(TreeNode* A,int B,int C) {
    while(1) {
        if(B<A->val && C<A->val)
            A=A->left;
        else if(A->val<B && A->val<C) 
            A=A->right;
        else 
            break;
    }
    
    return distanceBetweenRootAndNode(A,B)+distanceBetweenRootAndNode(A,C);
}
int Solution::solve(TreeNode* A, int B, int C) {
    return distanceBetweenTwoNodes(A,B,C);
}


//Recursive solution O(H) S(H)
/* 
int distanceBetweenRootAndNode(TreeNode* A, int key) {
    if(key<A->val) {
        return 1+distanceBetweenRootAndNode(A->left,key);
    }
    else if(A->val<key) {
        return 1+distanceBetweenRootAndNode(A->right,key);
    }
    
    return 0;
}
int distanceBetweenTwoNodes(TreeNode* A,int B,int C) {
    if(B<A->val && C<A->val) {
        return distanceBetweenTwoNodes(A->left,B,C);
    }
    else if(A->val<B && A->val<C) {
        return distanceBetweenTwoNodes(A->right,B,C);
    }
    
    return distanceBetweenRootAndNode(A,B)+distanceBetweenRootAndNode(A,C);
}
int Solution::solve(TreeNode* A, int B, int C) {
    return distanceBetweenTwoNodes(A,B,C);
}
*/

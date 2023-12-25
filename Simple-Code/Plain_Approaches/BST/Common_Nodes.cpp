/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
 
//O(m+n) S(h1+h2) 
//Did it in one pass for both trees
int Solution::solve(TreeNode* A, TreeNode* B) {
    long long int sum=0;
    
    stack<TreeNode*> st1,st2;
    while(1) {
        while(A) {
            st1.push(A);
            A=A->left;
        }
        
        while(B) {
            st2.push(B);
            B=B->left;
        }
        
        if(!st1.empty() && !st2.empty()) {
            TreeNode* firNode=st1.top();
            TreeNode* secNode=st2.top();
            
            if(firNode->val==secNode->val) {
                st1.pop();
                st2.pop();
                sum=(sum+firNode->val)%1000000007;
                A=firNode->right;
                B=secNode->right;
            } 
            else if(firNode->val<secNode->val) {
                st1.pop();
                A=firNode->right;
                B=NULL;
            }
            else if(firNode->val>secNode->val) {
                st2.pop();
                A=NULL;
                B=secNode->right;
            }
        }
        else 
            break;
    }
    
    return sum;
}

/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

//Iterative Solution
class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        while(1) {
            if(p->val<root->val && q->val<root->val) {
                root=root->left;
            }
            else if(root->val<p->val && root->val<q->val)
                root=root->right;
            else 
                break;
        }
        
        return root;
    }
};

//Recursive solution
/*
class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if(p->val<root->val && q->val<root->val) {
            return lowestCommonAncestor(root->left,p,q);
        }
        else if(root->val<p->val && root->val<q->val) {
            return lowestCommonAncestor(root->right,p,q);
        }
        
        return root;
    }
};
*/
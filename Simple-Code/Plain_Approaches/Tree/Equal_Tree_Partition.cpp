/**
 * Definition for binary tree
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

/*corner test case when sum of all nodes is 0 (if you will check for root also 
and it will return always true as sum and sum/2 are same if sum=0)*/

//1st approach
TreeNode *root;
int findTotalSum(TreeNode *A) {
    if(A==NULL)
        return 0;
        
    return A->val+findTotalSum(A->left)+findTotalSum(A->right);
}
int isPartitionPossible(TreeNode* A,int *currentSum,int totalSum) {
    if(A==NULL) {
        return 0;
    }
    
    int leftSum=0;
    int rightSum=0;
    
    int l=isPartitionPossible(A->left,&leftSum,totalSum);
    int r=isPartitionPossible(A->right,&rightSum,totalSum);
    
    *currentSum=A->val+leftSum+rightSum;
    if(*currentSum==totalSum/2 && root!=A) {
        return 1;
    } 
    
    return l||r;
}
int Solution::solve(TreeNode* A) {
    root=A;
    int totalSum=findTotalSum(A);
    if(totalSum%2)
        return 0;
    
    int sum=0;    
    return isPartitionPossible(A,&sum,totalSum);
}


//Same solution but taking global variable to mark the answer
//2nd approach
TreeNode *root;
int findTotalSum(TreeNode *A) {
    if(A==NULL)
        return 0;
        
    return A->val+findTotalSum(A->left)+findTotalSum(A->right);
}
int isPartitionPossible(TreeNode* A,int sum, int &chk) {
    if(A==NULL || chk)
        return 0;
    
    int sumOfSubtree=A->val+isPartitionPossible(A->left,sum,chk)+isPartitionPossible(A->right,sum,chk);
    if(sumOfSubtree==sum/2 && root!=A) {
        chk=1;
    }
    
    return sumOfSubtree;
}
int Solution::solve(TreeNode* A) {
    root=A;
    
    int totalSum=findTotalSum(A);
    if(totalSum%2)
        return 0;
        
    int chk=0;
    int temp=isPartitionPossible(A,totalSum,chk);
    return chk;
}
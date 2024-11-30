#ifndef TREENODE_H
#define TREENODE_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <string>
#include <algorithm>
#include <climits>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

TreeNode* buildTreeFromFile(const std::string& filename);

#endif

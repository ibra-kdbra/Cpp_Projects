#pragma once
#include <iostream>
#include <algorithm>
#include "ast_token.h"

namespace simple_parser {

class AstTreeViewer {
    private:
    AstToken* head;

    bool in_list(vector<int> *list, int val) {
        for (vector<int>::iterator it = list->begin(); it != list->end(); ++it) {
            if (val == *it)
                return true;
        }
        return false;
    }

    void print_string(AstToken *token, int depth, int ith_child, bool is_last_child, vector<int>& list) {
        for (int i = 0; i < depth; ++i) {
                if (is_last_child && i == depth - 1) {
                    std::cout << "\xE2\x94\x94\xE2\x94\x80";
                } else if (!is_last_child && i == depth - 1) {
                    std::cout << "\xE2\x94\x9c\xE2\x94\x80";
                }
                else if (in_list(&list, i )) {
                    std::cout << "\xE2\x94\x82 ";
                }
                else 
                    std::cout << "  ";
        }
        std::cout << " < " + token->peek_value() + " > " + token_type_to_string(token->peek_type());
        std::cout << endl;
    }

    void iterate(AstToken *parent, int ith_child, bool is_last_child) {
        static int depth = 0;
        static vector<int> list;

        ++depth;
        int child_len = parent->child_list.size();
        print_string(parent, depth, ith_child, is_last_child, list);
        for (int i = 0; i < child_len; ++i) {
            if (i == 0) {
                list.push_back(depth);
            }
            if (i == child_len - 1) {
                list.pop_back();
            }
            iterate(parent->child_list[i], i, i == child_len - 1);
        }
        --depth;
    }

    public:
    AstTreeViewer(AstToken *head):head(head){}
    void view() {
        iterate(head, 0, true);
    }

};

}
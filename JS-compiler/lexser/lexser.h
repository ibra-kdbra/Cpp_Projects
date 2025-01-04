#pragma once
#include "token.h"
#include <vector>

namespace simple_lexser {

class Lexser {
    private:
        int32_t line;
        int32_t index;
        Token_t *get_token(const char *curser, int32_t *skip);
    public:
        Lexser():line(1), index(0) {}
        ~Lexser() {
            for (std::vector<Token_t>::iterator it = token_list.begin(); 
            it != token_list.end();) {
                it = token_list.erase(it);
            }
        }
        void lex(const char *src);

        std::vector<Token_t> token_list;
        Token_t& get_token_by_index(int index);
};

}
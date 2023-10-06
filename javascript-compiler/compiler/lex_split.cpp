#include "lex_split.h"

using namespace std;
namespace simple_compiler {

vector<string> split_line(string line) {
    vector<string> ret;
    const char *str = line.c_str();
    char *start, *end;
    for (start = end = (char *)str; *start != '\0';) {
        for (; *start == ' ' || *start == '\t' || *start == ','; ++start);
        end = start;
        for (; *end != ' ' && *end != '\0' && *end != ','; ++end);
        if (start < end) {
            ret.push_back(string(start, end - start));
        }
        start = end;
    }
    return ret;
}

int get_variable_index(string variable) {
    const char *str = variable.c_str();
    assert(*str == 't');
    return atoi(str + 1);
}

const char *get_last_nth_of_char(const char *str, char ch, int n) {
    int char_num = 0;
    int index = 0;
    for(const char *start = str; *start != '\0'; ++start) {
        if (*start == ch)
            ++char_num;
    }
    assert(n <= char_num);
    for (const char *start = str + strlen(str) - 1; start != str - 1; --start) {
        if (*start == ch) {
            ++index;
        }
        if (index == n) {
            return start;
        }
    }
    return nullptr;
}

}
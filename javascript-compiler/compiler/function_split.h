#pragma once
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

using namespace std;
namespace simple_compiler {

class FunctionSplitter {
    public:
    vector<vector<string>> functions;

    private:
    void function_split(char *buf) {
        char *start;
        char *end;

        for (start = buf, end = buf; *end != '\0';) {
            for (;*end != '\n' && *end != '\0'; ++end);
            if (start != end) {
                *end = '\0';
                if (strstr(start, ".start") || strstr(start, ".func_start")) {
                    vector<string> tmp{string(start)};
                    functions.push_back(tmp);
                } else {
                    functions[functions.size() - 1].push_back(string(start));
                }
            }
            end += 1;
            start = end;
        }
    }

    public:
    void print_functions() {
        for (vector<vector<string>>::iterator it = functions.begin(); it != functions.end(); ++it) {
            for (vector<string>::iterator iit = it->begin(); iit != it->end(); ++iit) {
                cout << *iit << endl;
            }
            cout << endl;
        }
    }

    FunctionSplitter(char *buf) {
        function_split(buf);
    }
};

}
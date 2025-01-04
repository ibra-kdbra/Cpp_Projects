#pragma once
#include <vector>
#include <map>
using namespace std;

namespace simple_interpreter {

class Scope {
    public:
    map<string, string> variable_list;
    string mark;

    Scope(){}
    Scope(string mark): mark(mark){}

    // string *get_string_variable(string var) {
    //     map<string, string>::iterator it = variable_list.find("string_" + var);
    //     if (it != variable_list.end()) {
    //         return &(it->second);
    //     }
    //     return nullptr;
    // }

    // string *get_int_variable(string var) {
    //     map<string, string>::iterator it = variable_list.find("int_" + var);
    //     if (it != variable_list.end()) {
    //         return &(it->second);
    //     }
    //     return nullptr;        
    // }

    // void add_string_variable(string var, string value) {
    //     variable_list.insert(make_pair("string_" + var, value));
    // }

    // void add_int_variable(string var, string value) {
    //     variable_list.insert(make_pair("int_" + var, value));        
    // }

    void add_variable(string var, string value) {
        variable_list.insert(make_pair(var, value));         
    }

    string *get_variable(string var) {
        map<string, string>::iterator it = variable_list.find(var);
        if (it != variable_list.end()) {
            return &(it->second);
        }
        return nullptr;          
    }
};

}
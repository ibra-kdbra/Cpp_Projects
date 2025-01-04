#pragma once
#include "scope.h"
#include <vector>
#include <assert.h>

using namespace std;
namespace simple_interpreter {

class ScopeManager {
    private:
    vector<Scope> scope_list;

    public:
    ScopeManager() {
        scope_list.push_back(Scope()); //the global variable scope
    }

    ~ScopeManager() {
        scope_list.pop_back();
    }

    void increase_scope_layer() {
        scope_list.push_back(Scope());
    }

    void increase_scope_layer(string mark) {
        scope_list.push_back(Scope(mark));
    }

    void decrease_scope_layer() {
        assert(scope_list.size() > 1);
        scope_list.pop_back();
    }

    // string get_string_value(string var) {
    //     string* ret;
    //     for (vector<Scope>::iterator it = scope_list.begin(); 
    //         it != scope_list.end(); ++it) {
    //         ret = it->get_string_variable(var);
    //         if (ret) {
    //             return *ret;
    //         }
    //     }
    //     printf("undefined variable %s\n", var.c_str());
    //     exit(-1);
    // }

    // int get_int_value(string var) {
    //     string* ret;
    //     for (vector<Scope>::iterator it = scope_list.begin(); 
    //         it != scope_list.end(); ++it) {
    //         ret = it->get_int_variable(var);
    //         if (ret) {
    //             return atoi(ret->c_str());
    //         }
    //     }
    //     printf("undefined variable %s\n", var.c_str());
    //     exit(-1);
    // }

    // void set_value(string var, string value) {
    //     string* ret;
    //     for (vector<Scope>::iterator it = scope_list.begin(); 
    //         it != scope_list.end(); ++it) {
    //         ret = it->get_string_variable(var);
    //         if (ret) {
    //             *ret = value;
    //             return;
    //         }
    //     }
    //     scope_list[scope_list.size() - 1].add_string_variable(var, value);
    // }

    // void set_value(string var, int value) {
    //     string* ret;
    //     for (vector<Scope>::iterator it = scope_list.begin(); 
    //         it != scope_list.end(); ++it) {
    //         ret = it->get_int_variable(var);
    //         if (ret) {
    //             *ret = to_string(value);
    //             return;
    //         }
    //     }
    //     scope_list[scope_list.size() - 1].add_int_variable(var, to_string(value));
    // }

    string get_variable(string var) {
        string* ret;
        for (vector<Scope>::reverse_iterator it = scope_list.rbegin(); 
            it != scope_list.rend(); ++it) {
            ret = it->get_variable(var);
            if (ret) {
                return *ret;
            }
        }
        printf("try get undefined variable %s\n", var.c_str());
        exit(-1);
    }

    void set_variable(string var, string value) {
        string* ret;
        for (vector<Scope>::reverse_iterator it = scope_list.rbegin(); 
            it != scope_list.rend(); ++it) {
            ret = it->get_variable(var);
            if (ret) {
                *ret = value;
                return;
            }
        }
        printf("try set unexist variable %s\n", var.c_str());
        exit(-1);
    }

    void new_variable(string var) {
        scope_list[scope_list.size() - 1].add_variable(var, "");
    }

    void new_and_set_variable(string var, string value) {
        scope_list[scope_list.size() - 1].add_variable(var, value);
    }

    Scope* current_scope() {
        return &scope_list[scope_list.size() - 1];
    }

    Scope* find_last_scope_with_mark(string mark) {
        for(vector<Scope>::reverse_iterator it = scope_list.rbegin(); it != scope_list.rend(); ++it) {
            if (it->mark == mark) {
                return &(*it);
            }
        }
        return nullptr;
    }
};

}
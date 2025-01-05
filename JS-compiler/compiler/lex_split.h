#pragma once
#include <vector>
#include <string>
#include <assert.h>
#include <cstring>

using namespace std;
namespace simple_compiler {

vector<string> split_line(string line);
int get_variable_index(string variable);
const char *get_last_nth_of_char(const char *str, char ch, int n);

}
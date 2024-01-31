#include <iostream>
#include "regex_to_dfa.h"
using namespace std;

int main() {
  //ifstream cin("data.in");
  //ofstream cout("data.out");

  RegexToDFAConverter converter;

  string regex; cin >> regex;
  cout << converter.convert(regex);

  return 0;
}

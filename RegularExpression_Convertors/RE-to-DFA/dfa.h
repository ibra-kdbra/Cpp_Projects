#ifndef REGEXTODFACONVERTER_DFA_H
#define REGEXTODFACONVERTER_DFA_H

#include <iostream>
#include <unordered_set>
#include <map>
using namespace std;

class DFA {
 public:
  DFA() = default;
  DFA(const DFA&) = default;
  DFA& operator=(const DFA&) = default;
  virtual ~DFA() = default;

  //returns the number of states
  int size() const;
  //adds a new state
  void add_node(int node);

  //set the initial state
  void set_initial(int node);
  //add a state to the list of final states
  void add_final(int node);

  //add a transition (a directed labeled edge)
  void add_transition(int from, char letter, int to);

  //print operator
  friend ostream& operator << (ostream& out, const DFA& dfa);
 private:
  //the set of states
  unordered_set<int> nodes_;
  //the set of final states
  unordered_set<int> final_nodes_;
  //the initial state
  int initial_;
  //the dfa transitions
  map<pair<int, char>, int> transitions_;
};

#endif //REGEXTODFACONVERTER_DFA_H

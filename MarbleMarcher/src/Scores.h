#pragma once
#include "Level.h"
#include <string>
#include <fstream>

class Scores {
public:
  Scores() { memset(this, -1, sizeof(Scores)); }

  bool HasUnlocked(int level) const {
    return level <= 0 || scores[level - 1] >= 0;
  }
  bool HasCompleted(int level) const {
    return scores[level] >= 0;
  }
  int Get(int level) const {
    return scores[level];
  }

  int GetStartLevel() const {
    for (int i = 0; i < num_levels; ++i) {
      if (!HasCompleted(i)) {
        return i;
      }
    }
    return 0;
  }

  bool Update(int level, int score) {
    if (score < scores[level] || scores[level] < 0) {
      scores[level] = score;
      return true;
    }
    return false;
  }

  void Load(const std::string& fname) {
    std::ifstream fin(fname, std::ios::binary);
    if (!fin) { return; }
    fin.read((char*)this, sizeof(Scores));
  }

  void Save(const std::string& fname) {
    std::ofstream fout(fname, std::ios::binary);
    if (!fout) { return; }
    fout.write((const char*)this, sizeof(Scores));
  }

  int scores[num_levels];
};
extern Scores high_scores;
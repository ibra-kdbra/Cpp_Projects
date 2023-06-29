#pragma once
#include <fstream>

class Settings {
public:
  Settings() :
    mute(false),
    mouse_sensitivity(0) {
  }

  void Load(const std::string& fname) {
    std::ifstream fin(fname, std::ios::binary);
    if (!fin) { return; }
    fin.read((char*)this, sizeof(this));
  }
  void Save(const std::string& fname) {
    std::ofstream fout(fname, std::ios::binary);
    if (!fout) { return; }
    fout.write((char*)this, sizeof(this));
  }

  bool   mute;
  int    mouse_sensitivity;
};
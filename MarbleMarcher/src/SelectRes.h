#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct Resolution {
  Resolution(int w, int h, const char* i) : width(w), height(h), info(i) {}
  int width;
  int height;
  const char* info;
};
static const int num_resolutions = 7;
const extern Resolution all_resolutions[num_resolutions];

class SelectRes {
public:
  SelectRes(const sf::Font* _font);

  bool FullScreen() const { return is_fullscreen; }

  int Select(const sf::Vector2i& mouse_pos);
  void Draw(sf::RenderWindow& window, const sf::Vector2i& mouse_pos);
  sf::Text MakeText(const char* str, float x, float y, int size, bool selected=true, bool centered=true) const;
  const Resolution* Run();

private:
  const sf::Font* font;

  bool is_fullscreen;

  sf::Sound sound_hover;
  sf::SoundBuffer buff_hover;
};

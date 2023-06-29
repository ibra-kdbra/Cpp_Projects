#pragma once
#include "Settings.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

extern Settings game_settings;

class Overlays {
public:
  enum Texts {
    TITLE,
    PLAY,
    LEVELS,
    CONTROLS,
    SCREEN_SAVER,
    EXIT,
    CREDITS,
    PAUSED,
    CONTINUE,
    RESTART,
    QUIT,
    MUSIC,
    MOUSE,
    CONTROLS_L,
    CONTROLS_R,
    BACK,
    L0, L1, L2, L3, L4,
    L5, L6, L7, L8, L9,
    L10, L11, L12, L13, L14,
    NEXT,
    PREV,
    BACK2,
    NUM_TEXTS
  };
  static const int LEVELS_PER_PAGE = 15;

  Overlays(const sf::Font* _font, const sf::Font* _font_mono);

  //Relative to 1280x720
  void SetScale(float scale) { draw_scale = scale; }

  Texts GetOption(Texts from, Texts to);
  int& GetLevelPage() { return level_page; }

  void UpdateMenu(float mouse_x, float mouse_y);
  void UpdateControls(float mouse_x, float mouse_y);
  void UpdateLevels(float mouse_x, float mouse_y);
  void UpdatePaused(float mouse_x, float mouse_y);

  void DrawMenu(sf::RenderWindow& window);
  void DrawControls(sf::RenderWindow& window);
  void DrawTimer(sf::RenderWindow& window, int t, bool is_high_score);
  void DrawLevelDesc(sf::RenderWindow& window, int level);
  void DrawFPS(sf::RenderWindow& window, int fps);
  void DrawPaused(sf::RenderWindow& window);
  void DrawArrow(sf::RenderWindow& window, const sf::Vector3f& v3);
  void DrawCredits(sf::RenderWindow& window, bool fullrun, int t);
  void DrawMidPoint(sf::RenderWindow& window, bool fullrun, int t);
  void DrawLevels(sf::RenderWindow& window);
  void DrawSumTime(sf::RenderWindow& window, int t);
  void DrawCheatsEnabled(sf::RenderWindow& window);
  void DrawCheats(sf::RenderWindow& window);

protected:
  void MakeText(const char* str, float x, float y, float size, const sf::Color& color, sf::Text& text, bool mono=false);
  void MakeTime(int t, float x, float y, float size, const sf::Color& color, sf::Text& text);
  void UpdateHover(Texts from, Texts to, float mouse_x, float mouse_y);

private:
  sf::Text all_text[NUM_TEXTS];
  bool all_hover[NUM_TEXTS];

  sf::Sound sound_hover;
  sf::SoundBuffer buff_hover;
  sf::Sound sound_click;
  sf::SoundBuffer buff_click;
  sf::Sound sound_count;
  sf::SoundBuffer buff_count;
  sf::Sound sound_go;
  sf::SoundBuffer buff_go;

  sf::Texture arrow_tex;
  sf::Sprite arrow_spr;

  float draw_scale;
  bool top_level;
  int level_page;

  const sf::Font* font;
  const sf::Font* font_mono;
};

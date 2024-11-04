#ifndef SS_HEADER_FOOD_H_
#define SS_HEADER_FOOD_H_

#include "engine.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
namespace game {
/*
Food: represents the Food object snake eats.
It is represented by a sf::RectangleShape
*/

class Food {
public:
  Food(sf::RenderWindow *, sf::Vector2f loc);
  sf::RectangleShape getFood();
  void drawFood();

private:
  sf::Color color;
  sf::Vector2f location;
  sf::RectangleShape food;
  sf::RenderWindow *screen;
};

} // namespace game

#endif // !SS_HEADER_FOOD_H_

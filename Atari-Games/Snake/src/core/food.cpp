#include "food.h"

game::Food::Food(sf::RenderWindow *w, sf::Vector2f loc) {
  location = loc;
  screen = w;
  color = sf::Color::Red;
  food = getRectangleAt(location, color);
}

sf::RectangleShape game::Food::getFood() { return food; }

void game::Food::drawFood() { screen->draw(food); }

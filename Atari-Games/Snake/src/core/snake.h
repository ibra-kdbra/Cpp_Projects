#ifndef SS_HEADER_SNAKE_H_
#define SS_HEADER_SNAKE_H_

#include <SFML/Graphics.hpp>
#include <list>
#include <random>

#include "food.h"

#define SNAKE_MAX_LENGTH 2000

namespace game {

class Random {
  /* A utility class that will be used to generate random numbers */
  std::random_device rd;
  std::mt19937 gen;

public:
  Random() { gen.seed(rd()); }

  int getRandomInt(int l, int u) {
    std::uniform_int_distribution<int> dist(l, u);
    return dist(gen);
  }
};

class Snake {
  /*
  This class represents the snake.
  It is represented as a Vector of sf::RectangleShape objects.
  */
public:
  Snake(sf::RenderWindow *);
  void drawSnake();
  bool died();
  bool ateFood(Food *fd);
  void moveSnake(sf::Vector2<int> direction);

  sf::Vector2f getNextFoodLocation();

private:
  sf::RenderWindow *screen;
  Random rand;

  /* Used to determine whether or not to increment length of snake */
  bool updateLegth;

  /* The rate of movement of snake */
  float movementScale;

  /* Snake parameters */
  int snake_length;
  std::list<sf::Vector2<int>> snake_direction_list;
  sf::Vector2<int> lastDirection;
  std::vector<sf::RectangleShape> body;

  /* Load from options */
  sf::Color colorBody;
  sf::Color colorHead;
};

} // namespace game

#endif // !SS_HEADER_SNAKE_H_

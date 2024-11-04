#ifndef SS_HEADER_ENGINE_H_
#define SS_HEADER_ENGINE_H_

#include <SFML/Graphics.hpp>

namespace game {
/*
Contains functions that will be used to perform certain
actions that will be needed that will be needed by game

*/

bool checkCollision(const sf::RectangleShape &, const sf::RectangleShape &);

/* Returns a rectangle at the specified coordinates */
sf::RectangleShape getRectangleAt(sf::Vector2f location, sf::Color);

} // namespace game

#endif // !SS_HEADER_ENGINE_H_

#ifndef SS_GAME_H
#define SS_GAME_H

#include "food.h"
#include "snake.h"

#include <SFML/Graphics.hpp>
#include <list>
#include <random>

#define BOX_SIZE 20

namespace game {
/*
GameController is the central interface of the game.
This class will monitor the working of game:
*	Movement of snake
*	Checking collisions
*	When to play sound etc
*/

const std::string MenuStrings[5] = {"Game Over!", "Do you wish to continue?",
                                    "Yes", "No", "Exit"}; // menu strings

enum MenuText {
  GameOver,
  Continue,
  Yes,
  No,
  Exit,
}; // menuText

enum Fonts {

};

/*==================================================*
 *			The main game controller *
 *===================================================*/
class GameController {
public:
  /* Constructors */
  GameController(sf::RenderWindow *w);

  /* Public methods */
  void start();
  /* methods to access the resources */
  sf::Font *getFont(Fonts font); // no definition yet

private:
  void gameLoop();
  void setupScene();
  void gameOver();      // no definition yet
  void loadResources(); // no definition yet

  Snake snake;
  sf::RenderWindow *screen;
  float scale;
  int score;

  /* Resources that the controller retrieves from Disk */
  sf::Font fontList[3];

}; // class game

} // namespace game

#endif // !SS_GAME_H

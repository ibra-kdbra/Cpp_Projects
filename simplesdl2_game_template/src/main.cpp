#include "gamehandler.h"

int main() {

    GameHandler game;

    while(!game.isGameOver) {
        game.input();
        game.update();
        game.render();
    }

    return 0;
}

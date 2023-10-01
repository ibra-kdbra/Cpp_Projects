#include "input.h"

class GameHandler final {
private:
        SDL_Window *sdlWindow_;
        SDL_Renderer *sdlRenderer_;

        Input user_action;
        GameManager game_process;

public:
        GameHandler();
        ~GameHandler();
        bool isGameOver = false;
        void input();
        void update();
        void render();
};


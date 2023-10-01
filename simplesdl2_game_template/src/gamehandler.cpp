#include "gamehandler.h"

GameHandler::GameHandler() {   
    SDL_Init(SDL_INIT_VIDEO);
    sdlWindow_ = SDL_CreateWindow(
            "Game_test",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_SHOWN);    
    sdlRenderer_ = SDL_CreateRenderer(sdlWindow_, -1, 0);
}

GameHandler::~GameHandler() {
    SDL_DestroyRenderer(sdlRenderer_);
    SDL_DestroyWindow(sdlWindow_);
    SDL_Quit();
}

void GameHandler::input() {
    isGameOver = user_action.update(game_process);
}

void GameHandler::update() {
    game_process.process_updates();

    switch (user_action.action_type) {
        case Input::action_type::NO_ACTIONS:
            break;
        case Input::action_type::STOP_ACTIONS:
            game_process.stop_dragging();
            game_process.end_select_area();
            user_action.clear_data();
            user_action.action_type = Input::action_type::NO_ACTIONS;
            break;
        case Input::action_type::NEW_OBJ:
            game_process.add_obj();
            user_action.action_type = Input::action_type::NO_ACTIONS;
            break;
        case Input::action_type::SELECT_OBJ:
            if (!user_action.getObjIDclicked()) break;
            game_process.target_obj(user_action.getObjIDclicked());
            user_action.clear_data();
            break;
        case Input::action_type::SELECTION:
            if (!game_process.selection_process())    {
            game_process.clear_selection();
            game_process.start_select_area(user_action.getPoint());
            } else if (user_action.getPoint() != game_process.get_selection_endPoint()) {
                game_process.set_selection_endPoint(user_action.getPoint());
                game_process.select_obj();
                }
            break;
        case Input::action_type::DRAG_OBJ:
            game_process.try_drag_obj(user_action.getPoint());
            break;
        case Input::action_type::SET_POINT_TO_MOVE:
            if (game_process.isInsideObj(user_action.getPoint())) {
                user_action.clear_data();
                user_action.action_type = Input::action_type::NO_ACTIONS;
                break;
            }
            game_process.find_place_for_obj(user_action.getPoint());
            user_action.clear_data();
            user_action.action_type = Input::action_type::NO_ACTIONS;
            break;
    }
}

void GameHandler::render() {
    SDL_SetRenderDrawColor(sdlRenderer_, 255, 255, 255, 255);
    SDL_RenderClear(sdlRenderer_);
    game_process.render(sdlRenderer_);
    SDL_RenderPresent(sdlRenderer_);
    SDL_Delay(5);
}

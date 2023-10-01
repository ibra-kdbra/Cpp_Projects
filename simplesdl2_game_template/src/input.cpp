#include "input.h"

bool Input::update(const GameManager& game) {
    SDL_PollEvent(&event);

    switch (event.type) {

    case SDL_QUIT:
        return true;

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
            point = {event.button.x, event.button.y};
            auto PointInsideObj = game.isInsideObj(point);
            if (PointInsideObj) {
                action_type = SELECT_OBJ;
                clickedonObjID = PointInsideObj.value();
                break;
            }
            action_type = SELECTION;
            break;
        }
        if (event.button.button == SDL_BUTTON_RIGHT && game.isAnyObjSelected()) {
            point = {event.button.x, event.button.y};
            action_type = SET_POINT_TO_MOVE;
        }
        break;

    case SDL_MOUSEMOTION:

        if (game.Targeted_obj()) {
            point = {event.button.x, event.button.y};
            action_type = DRAG_OBJ;
            break;
        }
        if (game.selection_process()) {
            point = {event.button.x, event.button.y};
            break;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        action_type = STOP_ACTIONS;
        break;

    case SDL_KEYDOWN:
        action_type = NEW_OBJ;
        break;
    }
    return false;
}

void Input::clear_data() {
    point = {-1, -1};
    clickedonObjID = 0;
}

#include "gamemanager.h"

class Input {
    uint16_t clickedonObjID;
    SDL_Event event;
    SDL_Point point;
public:
    bool update(const GameManager&);
    SDL_Point getPoint () const {return point;}
    uint16_t getObjIDclicked() const {return clickedonObjID;}
    void clear_data();
    enum action_type {
        NO_ACTIONS,
        STOP_ACTIONS,
        NEW_OBJ,
        SELECTION,
        SELECT_OBJ,
        SET_POINT_TO_MOVE,
        DRAG_OBJ,
         } action_type;
};

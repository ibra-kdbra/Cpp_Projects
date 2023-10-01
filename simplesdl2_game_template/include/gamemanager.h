#include "object.h"
#include "tasker.h"


class GameManager final
{
    std::vector<Object> objs;
    uint16_t targeted_objID = 0;
    uint16_t dragging_objID = 0;

    struct selection {
    std::optional<SDL_Rect> selected_area;
    SDL_Point selection_start;
    SDL_Point selection_end;
    bool isSelectionProcess = false;
    } selection;

    Tasker move_processing;

public:
    GameManager() = default;
    std::optional<uint16_t> isInsideObj(const SDL_Point) const;
    bool isAnyObjSelected() const;
    bool Targeted_obj() const {return targeted_objID;}
    void add_obj ();
    void render(SDL_Renderer*) const;
    void target_obj (const uint16_t);
    void try_drag_obj (const SDL_Point);
    void stop_dragging();
    void clear_selection();
    bool selection_process() const {return selection.isSelectionProcess;}
    void start_select_area(const SDL_Point);
    SDL_Point get_selection_endPoint() const {return selection.selection_end;}
    void set_selection_endPoint(const SDL_Point);
    void select_obj ();
    void end_select_area();
    void find_place_for_obj(const SDL_Point);
    void process_updates();
};


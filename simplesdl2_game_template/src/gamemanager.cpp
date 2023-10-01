#include "gamemanager.h"

std::optional<uint16_t> GameManager::isInsideObj(SDL_Point point) const {
    for (auto& obj : objs)
        if (SDL_PointInRect(&point, &obj.rect)) return obj.getID();
    return {};
}

bool GameManager::isAnyObjSelected() const {
    return std::find_if(objs.begin(), objs.end(), [](const Object& o){return o.isSelected;}) != objs.end() ? true : false;
}

void GameManager::add_obj() {
    objs.emplace_back(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
}

void GameManager::render(SDL_Renderer* renderer) const {
    for (auto& obj : objs) {
        SDL_SetRenderDrawColor(renderer, obj.color.r, obj.color.g, obj.color.b, obj.color.a);
        SDL_RenderFillRect(renderer, &obj.rect);
        if (obj.isSelected) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            char w = 4;
            SDL_Rect up_b{obj.rect.x, obj.rect.y, obj.rect.w, w};
            SDL_Rect right_b{obj.rect.x+obj.rect.w-w, obj.rect.y+w, w, obj.rect.h-w};
            SDL_Rect left_b{obj.rect.x, obj.rect.y+w, w, obj.rect.h-w};
            SDL_Rect down_b{obj.rect.x+w, obj.rect.y+obj.rect.h-w, obj.rect.w-2*w, w};
            SDL_RenderFillRect(renderer, &up_b);
            SDL_RenderFillRect(renderer, &right_b);
            SDL_RenderFillRect(renderer, &left_b);
            SDL_RenderFillRect(renderer, &down_b);
        }
        if (obj.isSelected && obj.current_status == Object::MOVING) {
            SDL_SetRenderDrawColor(renderer, obj.color.r, obj.color.g, obj.color.b, obj.color.a);
            SDL_RenderDrawLine(renderer, obj.getPos().x,
                                         obj.getPos().y,
                                         obj.getPathSegmnets().at(obj.getCurrentSegment()).second.x,
                                         obj.getPathSegmnets().at(obj.getCurrentSegment()).second.y);
            for (int i = obj.getCurrentSegment()+1; i< obj.getPathSegmnets().size(); i++)
                SDL_RenderDrawLine(renderer, obj.getPathSegmnets().at(i).first.x,
                                             obj.getPathSegmnets().at(i).first.y,
                                             obj.getPathSegmnets().at(i).second.x,
                                             obj.getPathSegmnets().at(i).second.y);
        }
    }
    if (selection_process() && selection.selected_area) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, selection.selected_area.value().x, selection.selected_area.value().y,
                selection.selected_area.value().x+selection.selected_area.value().w, selection.selected_area.value().y);
        SDL_RenderDrawLine(renderer, selection.selected_area.value().x+selection.selected_area.value().w, selection.selected_area.value().y,
                selection.selected_area.value().x+selection.selected_area.value().w, selection.selected_area.value().y+selection.selected_area.value().h);
        SDL_RenderDrawLine(renderer, selection.selected_area.value().x+selection.selected_area.value().w, selection.selected_area.value().y+selection.selected_area.value().h,
                selection.selected_area.value().x, selection.selected_area.value().y+selection.selected_area.value().h);
        SDL_RenderDrawLine(renderer, selection.selected_area.value().x, selection.selected_area.value().y+selection.selected_area.value().h,
                selection.selected_area.value().x, selection.selected_area.value().y);
    }
}

void GameManager::target_obj (uint16_t id) {
   targeted_objID = id;
   clear_selection();
   auto obj_it = std::find_if (objs.begin(), objs.end(), [id](const Object& o){return o.getID() == id;});
   obj_it->isTargeted = true;
   obj_it->isSelected = true;
}

void GameManager::try_drag_obj(SDL_Point point) {
    auto current_obj_it =
            std::find_if (objs.begin(), objs.end(), [this](const Object& o){return o.getID() == targeted_objID;});
    if (current_obj_it == objs.end()) return;

    SDL_Rect temp_rect
    ({point.x-current_obj_it->rect.w/2, point.y-current_obj_it->rect.h/2, current_obj_it->rect.w, current_obj_it->rect.h});

    for (auto& obj : objs) {
        if (obj == *current_obj_it) continue;
        if(SDL_HasIntersection(&temp_rect, &obj.rect)) return;
    }

    if (temp_rect.x < 0 || (temp_rect.x + temp_rect.w) > WINDOW_WIDTH ||
        temp_rect.y < 0 || (temp_rect.y + temp_rect.h) > WINDOW_HEIGHT )
            return;

    current_obj_it->isDragging = true;
    dragging_objID = targeted_objID;
    current_obj_it->rect.x = point.x - current_obj_it->rect.w/2;
    current_obj_it->rect.y = point.y - current_obj_it->rect.h/2;
}

void GameManager::stop_dragging() {
    targeted_objID = 0;
    dragging_objID = 0;
    std::for_each(objs.begin(), objs.end(), [](Object& o){o.isDragging = false;});
}

void GameManager::clear_selection() {
    std::for_each(objs.begin(), objs.end(), [](Object& o){o.isSelected = false;});
}

void GameManager::start_select_area(SDL_Point point) {
    selection.isSelectionProcess = true;
    selection.selection_start = point;
    selection.selection_end = point;
}

void GameManager::set_selection_endPoint(SDL_Point point) {
    selection.selection_end = point;
    SDL_Rect temp_rect;
    if (point.x>=selection.selection_start.x && point.y>=selection.selection_start.y) {
        temp_rect.x = selection.selection_start.x;
        temp_rect.y = selection.selection_start.y;
        temp_rect.w = point.x-selection.selection_start.x;
        temp_rect.h = point.y-selection.selection_start.y;
    } else if (point.x>=selection.selection_start.x && point.y<selection.selection_start.y) {
        temp_rect.x = selection.selection_start.x;
        temp_rect.y = point.y;
        temp_rect.w = point.x-selection.selection_start.x;
        temp_rect.h = selection.selection_start.y-point.y;
    } else if (point.x<selection.selection_start.x && point.y>=selection.selection_start.y) {
        temp_rect.x = point.x;
        temp_rect.y = selection.selection_start.y;
        temp_rect.w = selection.selection_start.x-point.x;
        temp_rect.h = point.y-selection.selection_start.y;
    } else if (point.x<selection.selection_start.x && point.y<selection.selection_start.y) {
        temp_rect.x = point.x;
        temp_rect.y = point.y;
        temp_rect.w = selection.selection_start.x-point.x;
        temp_rect.h = selection.selection_start.y-point.y;
    }

    if (temp_rect.x < 0) {temp_rect.x = 0; temp_rect.w = selection.selection_start.x;}
    if ((temp_rect.x + temp_rect.w) > WINDOW_WIDTH) temp_rect.w = WINDOW_WIDTH-temp_rect.x;
    if (temp_rect.y < 0) {temp_rect.y = 0; temp_rect.h = selection.selection_start.y;}
    if ((temp_rect.y + temp_rect.h) > WINDOW_HEIGHT) temp_rect.h = WINDOW_HEIGHT-temp_rect.y;

    selection.selected_area.emplace(std::move(temp_rect));
}

void GameManager::select_obj () {
    std::for_each(objs.begin(), objs.end(),
        [this](Object& o){o.isSelected = SDL_HasIntersection(&selection.selected_area.value(), &o.rect) ? true : false;});

}

void GameManager::end_select_area() {
    selection.selection_start = {-1,-1};
    selection.selection_end = {-1,-1};
    selection.isSelectionProcess = false;
    selection.selected_area = {};
}

void GameManager::find_place_for_obj(SDL_Point point) {
    for (auto& obj : objs) {
        if (obj.isSelected) {obj.setGoal(point); obj.current_status = Object::NEED_TO_BUILD_PATH;}
    }

    if (objs.size() == 1) {
        int available_x = point.x;
        int available_y = point.y;

        if (point.x<objs[0].rect.w/2) available_x = objs[0].rect.w/2;
        if (point.y<objs[0].rect.h/2) available_y = objs[0].rect.h/2;
        if (point.x>WINDOW_WIDTH-objs[0].rect.w/2) available_x = WINDOW_WIDTH-objs[0].rect.w/2;
        if (point.y>WINDOW_HEIGHT-objs[0].rect.h/2) available_y = WINDOW_HEIGHT-objs[0].rect.h/2;

        objs[0].setDest({available_x, available_y});
        objs[0].setGoal({available_x, available_y});

        objs[0].buildPath(objs);
        return;
    }

    auto res = move_processing.find_where_to_place(objs, point);

    for (const auto& [id, mask_point] : res)
        std::for_each(objs.begin(), objs.end(), [&id, &mask_point](auto& o){if (o.getID() == id) o.setDest(mask_point);});

    std::for_each(std::execution::par, objs.begin(), objs.end(), [this](auto& obj){obj.buildPath(objs);});
}

void GameManager::process_updates() {

/////////////////need_to_find_alt_dest
    if (std::any_of(objs.begin(), objs.end(), [](const auto& o){return o.current_status == Object::NEED_TO_FIND_ALT_DEST;})) {
        move_processing.update_map(objs);
        std::for_each(std::execution::par, objs.begin(), objs.end(), [this](Object& obj){obj.flood_fill(move_processing.getMap());});

        std::vector<SDL_Rect> temp_masks {};
        for (const auto& obj : objs) if (obj.getDestination().has_value() && obj.current_status != Object::NEED_TO_FIND_ALT_DEST)
                    temp_masks.push_back(obj.rect_dest_mask);

        bool no_need_to_move {false};
        for (auto& obj : objs) {
                if (obj.current_status != Object::NEED_TO_FIND_ALT_DEST) continue;
                if (!temp_masks.empty())
                    obj.add_new_mask(temp_masks);
                if (!obj.find_alt_dest()) {
                    auto hasIntersection = [&obj](const auto& o){return SDL_HasIntersection(&o.rect, &obj.rect_dest_mask);};
                    if (std::any_of(objs.begin(), objs.end(), hasIntersection)) {
                        obj.current_status = Object::STUCK;
                        break;
                    }
                    no_need_to_move = true;
                    break;
                };
                temp_masks.push_back(obj.rect_dest_mask);
            }
        temp_masks.clear();

        if (no_need_to_move) {
            std::for_each(objs.begin(), objs.end(),
                          [this](auto& obj){
                              if (obj.current_status == Object::NEED_TO_BUILD_PATH_FOR_ALT_DEST ||
                                  obj.current_status == Object::NEED_TO_BUILD_PATH ||
                                  obj.current_status == Object::NEED_TO_FIND_ALT_DEST) {
                                        obj.current_status = Object::DEST_ACHIEVED;}
                          });

        }
    }
/////////////////need_to_build_path_for_alt_dest
    if (std::any_of(objs.begin(), objs.end(), [](const auto& o){return o.current_status == Object::NEED_TO_BUILD_PATH_FOR_ALT_DEST;}))
            std::for_each(std::execution::par, objs.begin(), objs.end(), [this](auto& obj){obj.buildPath(objs);});

/////////////////moving
    if (std::any_of(objs.begin(), objs.end(), [](const auto& o){return o.current_status == Object::MOVING;})) {
        for (auto& obj : objs) {
            if (obj.current_status != Object::MOVING) continue;
            SDL_Point point = move_processing.calc_objs_pos(obj);
            SDL_Rect temp_rect {point.x-obj.rect.w/2, point.y-obj.rect.h/2, obj.rect.w, obj.rect.h};
            bool point_avaiable = true;
            for (const auto& obj_check : objs) {
                if (obj_check.current_status == Object::STATIC && SDL_HasIntersection(&temp_rect, &obj_check.rect)) {
                        obj.setDest({-1,-1});
                        obj.current_status = Object::NEED_TO_BUILD_PATH;                      
                        auto res = move_processing.find_where_to_place(objs, obj.getGoalPoint());

                        for (const auto& [id, mask_point] : res)
                            std::for_each(objs.begin(), objs.end(), [&id, &mask_point](auto& o)
                                          {if (o.getID() == id) o.setDest(mask_point);});
                        obj.buildPath(objs);
                        point_avaiable = false;
                        break;
                }
            }
            if (point_avaiable) {
                obj.setPos(point);
                if (point == obj.getPathSegmnets().at(obj.getCurrentSegment()).second) obj.process_next_segment();
            }
            if (obj.getDestination().has_value() && obj.getPos() == obj.getDestination().value())
                obj.current_status = Object::DEST_ACHIEVED;
        }
    }
/////////////////transform_to_static
    if (std::any_of(objs.begin(), objs.end(), [](const auto& o){return o.current_status == Object::DEST_ACHIEVED;})) {
        if (objs.size() == 1 ) {objs[0].current_status = Object::STATIC;} else {
            for (auto& arrived_obj : objs) {
                if (arrived_obj.current_status != Object::DEST_ACHIEVED) continue;

                auto hasIntersection = [&arrived_obj](const auto& o) -> bool {
                    if (arrived_obj.getID() ==  o.getID()) return false;
                    return SDL_HasIntersection (&arrived_obj.rect, &o.rect);
                };
                auto isStuck = [&arrived_obj](const auto& o) -> bool {
                    if ( !(o.current_status == Object::DEST_ACHIEVED || o.current_status == Object::STUCK)
                        || arrived_obj.getID() ==  o.getID()) return false;
                    return SDL_HasIntersection (&arrived_obj.rect, &o.rect);
                };

                if (std::none_of(objs.begin(), objs.end(), hasIntersection)) {
                        arrived_obj.current_status = Object::STATIC;
                        arrived_obj.setGoal({-1,-1});
                        arrived_obj.setDest({-1,-1});
                } else if(std::any_of(objs.begin(), objs.end(), isStuck))
                            arrived_obj.current_status = Object::STUCK;
            }
        }
    }

/////////////////process_stuck_obj
    if (std::any_of(objs.begin(), objs.end(), [](const auto& o){return o.current_status == Object::STUCK;})) {
    //#TODO: need to figure out what to do with stuck objects
    }
}

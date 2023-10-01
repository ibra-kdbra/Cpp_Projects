#include "tasker.h"

Tasker::Tasker () noexcept
{
    map.resize(WINDOW_WIDTH*WINDOW_HEIGHT, false);
}

void Tasker::update_map(const std::vector<Object>& objs) {
    std::fill(map.begin(), map.end(), false);
    for (const auto& obj : objs) {
        if (obj.current_status != Object::STATIC) continue;
        int x = obj.getDestination().has_value() ? obj.rect_dest_mask.x : obj.rect.x;
        int y = obj.getDestination().has_value() ? obj.rect_dest_mask.y : obj.rect.y;
        for (int i=0; i<obj.rect.w; i++) {
            map[WINDOW_WIDTH*y+x+i] = true;
            map[WINDOW_WIDTH*(y+obj.rect.h-1)+x+i] = true;
        }
        for (int i=0; i<obj.rect.h; i++) {
            map[WINDOW_WIDTH*(y+i)+x] = true;
            map[WINDOW_WIDTH*(y+i)+x+obj.rect.w-1] = true;
        }
    }
}

std::unordered_map<uint16_t, SDL_Point> Tasker::find_where_to_place (const std::vector<Object>& objs, SDL_Point goal_point) const {
    std::unordered_map<uint16_t, SDL_Point> masks;

    auto find_for_one = [this, &goal_point, &objs, &masks, min_r = 0] (const Object& current_obj) mutable {
        if (current_obj.current_status != Object::NEED_TO_BUILD_PATH) return;
        auto isDestinationAvailable = [this, &objs, &current_obj, &masks] (SDL_Point point) -> bool {

            auto IntersectionWithStaticObj = [&point, &current_obj](const Object& obj) -> bool {
                if (obj.getID() == current_obj.getID()) return false;
                SDL_Rect t_rect {point.x-current_obj.rect.w/2, point.y-current_obj.rect.h/2, current_obj.rect.w, current_obj.rect.h};
                return obj.current_status == Object::STATIC && SDL_HasIntersection(&t_rect, &obj.rect);
            };

            auto IntersectionWithCurrentMasks = [&point, &current_obj, &objs](const auto& el) -> bool {
                SDL_Rect t_rect {point.x-current_obj.rect.w/2, point.y-current_obj.rect.h/2, current_obj.rect.w, current_obj.rect.h};
                SDL_Rect m_rect;
                std::for_each(objs.begin(), objs.end(),
                              [&el, &m_rect](const auto& o){if (o.getID() == el.first) m_rect=
                                {el.second.x-o.rect.w/2, el.second.y-o.rect.h/2, o.rect.w, o.rect.h};});
                return SDL_HasIntersection(&t_rect, &m_rect);
            };

            auto IntersectionWithOldMasks = [&point, &current_obj, &objs](const auto& obj) -> bool {
                if (obj.getID() == current_obj.getID() || !obj.getDestination().has_value()) return false;
                SDL_Rect t_rect {point.x-current_obj.rect.w/2, point.y-current_obj.rect.h/2, current_obj.rect.w, current_obj.rect.h};
                return SDL_HasIntersection(&t_rect, &obj.rect_dest_mask);
            };


            return std::none_of(objs.begin(), objs.end(), IntersectionWithStaticObj)
                   && std::none_of(masks.begin(), masks.end(), IntersectionWithCurrentMasks)
                   && std::none_of(objs.begin(), objs.end(), IntersectionWithOldMasks);
        };

        int point_index = WINDOW_WIDTH*goal_point.y+goal_point.x;
        SDL_Rect window_border = {current_obj.rect.w/2,
                                  current_obj.rect.h/2,
                                  WINDOW_WIDTH-current_obj.rect.w,
                                  WINDOW_HEIGHT-current_obj.rect.h,
                                  };

        if (min_r == 0 && SDL_PointInRect(&goal_point, &window_border) && isDestinationAvailable(goal_point)) {
            masks.insert({current_obj.getID(), goal_point});
            min_r = current_obj.rect.h > current_obj.rect.w ? current_obj.rect.w : current_obj.rect.h;
            return;
        }

        std::set<int> var_r, min_var_r {min_r};
        int r = std::max(std::max(goal_point.x, goal_point.y), std::max(WINDOW_WIDTH-goal_point.x, WINDOW_HEIGHT-goal_point.y));
        var_r.insert(r);
        r = r/2;

        SDL_Point temp_point;
        int final_x, final_y;

        while (r) {
            bool found_place = false;

            float i = 0.0f;
            while (i < 2*std::numbers::pi) {
                temp_point = {goal_point.x + static_cast<int>(r * cos(i)), goal_point.y + static_cast<int>(r * sin(i))};

                if (SDL_PointInRect(&temp_point, &window_border) && isDestinationAvailable(temp_point)) {
                    final_x=temp_point.x; final_y=temp_point.y;
                    found_place = true;
                    break;
                }
                i += 4*std::numbers::pi/r;
            }

            if (found_place) {
                var_r.insert(r);
                r = (*var_r.begin() +  *(min_var_r.rbegin()))/ 2;
                if ((*var_r.begin() - *min_var_r.rbegin()) == 1) break;
                continue;
            }
            min_var_r.insert(r);
            r = (((r+*(var_r.begin()))/2) <= r) ? r+1 : ((r+*(var_r.begin()))/2);
            if ((*var_r.begin() - *min_var_r.rbegin()) == 1) break;
        }
        masks.insert({current_obj.getID(), {final_x, final_y}});
        min_r = r;
    };

    std::for_each(objs.begin(), objs.end(), find_for_one);
    return masks;
}

SDL_Point Tasker::calc_objs_pos(const Object& obj) {

    SDL_Point start {obj.getPathSegmnets()[obj.getCurrentSegment()].first.x, obj.getPathSegmnets()[obj.getCurrentSegment()].first.y};
    SDL_Point end {obj.getPathSegmnets()[obj.getCurrentSegment()].second.x, obj.getPathSegmnets()[obj.getCurrentSegment()].second.y};

    Uint64 currTicks = SDL_GetPerformanceCounter();
    float deltaTime = (currTicks - obj.getPrevTicks()) / 1000000000.0f;
    float distance = deltaTime * obj.getSpeed();

    float dirX = end.x - start.x;
    float dirY = end.y - start.y;

    float magnitude = std::sqrt(dirX * dirX + dirY * dirY);
    float unitDirX = dirX / magnitude;
    float unitDirY = dirY / magnitude;

    int currX = static_cast<int>(distance * unitDirX);
    int currY = static_cast<int>(distance * unitDirY);

    if ((start.x < end.x && start.x+currX >= end.x) || (start.y < end.y && start.y+currY >= end.y) ||
        (start.x > end.x && start.x+currX <= end.x) || (start.y > end.y && start.y+currY <= end.y) )
            return {end.x, end.y};
    return {start.x+currX, start.y+currY};
}

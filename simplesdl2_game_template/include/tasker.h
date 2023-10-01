#include "object.h"

class Tasker final
{
private:
    std::vector<bool> map;

public:
    Tasker() noexcept;
    void update_map(const std::vector<Object>&);
    std::vector<bool> getMap () const {return map;}
    std::unordered_map<uint16_t, SDL_Point> find_where_to_place (const std::vector<Object>&, SDL_Point) const;
    SDL_Point calc_objs_pos(const Object&);
};

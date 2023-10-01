#pragma once
#include "common.h"

class Object
{
    static uint16_t current_id;
    inline static std::mutex mu;
    uint16_t id;

    uint16_t speed = 200;

    struct path {
        SDL_Point start_p = {-1, -1};
        SDL_Point goal_p = {-1, -1};
        SDL_Point destination_p = {-1, -1};
        std::vector<std::pair<SDL_Point, SDL_Point>> segments;
        Uint64 prevTicks;
        uint16_t current_segment;
        std::map<float, int> distances;
    } path;

    std::vector<bool> border_mask;
    std::vector<bool> area_available;
    std::shared_ptr<pathfinder::navmesh::NavmeshInterface> navmesh;

    void BFS_alg_fill_area (int16_t, int16_t);
    void createNavmesh(const std::vector<Object>&);
    void createMask (const std::vector<bool>&);
    void writePoly(const auto&, std::string);

public:
    SDL_Rect rect;
    SDL_Color color;
    SDL_Rect rect_dest_mask;
    bool isSelected = false;
    bool isTargeted = false;
    bool isDragging = false;
    bool isMoving = false;
    bool hasPath = false;


    enum status {STATIC,
                 NEED_TO_BUILD_PATH,
                 NEED_TO_BUILD_PATH_FOR_ALT_DEST,
                 NEED_TO_FIND_ALT_DEST,
                 MOVING,
                 DEST_ACHIEVED,
                 STUCK} current_status;

    Object(int x, int y, int w = 50, int h = 50, SDL_Color color = {255, 0, 0, 255});

    SDL_Color getColor() const {return color;}
    uint16_t getID() const {return id;}

    std::optional<SDL_Point> getDestination () const;
    std::vector<std::pair<SDL_Point, SDL_Point>>  getPathSegmnets ()  const {return path.segments;}
    SDL_Point getGoalPoint () const {return path.goal_p;}

    bool operator == (const Object&) const;

    SDL_Point getPos() const {return {rect.x+rect.w/2, rect.y+rect.h/2};}

    void setDest(const SDL_Point);
    void setPos (const SDL_Point);
    void setGoal (const SDL_Point);
    void buildPath (const std::vector<Object>&);
    void flood_fill (const std::vector<bool>&);
    bool find_alt_dest();
    void add_new_mask (const std::vector<SDL_Rect>&);
    Uint64 getPrevTicks () const {return path.prevTicks;}
    uint16_t getSpeed() const {return speed;}
    uint16_t getCurrentSegment() const {return path.current_segment;}
    void process_next_segment();
};


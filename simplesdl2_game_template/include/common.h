#pragma once
#include <iostream>
#include <optional>
#include <vector>
#include <algorithm>
#include <numbers>
#include <set>
#include <unordered_map>
#include <utility>
#include <ranges>
#include <chrono>
#include <queue>
#include <thread>
#include <mutex>
#include <execution>
#include <future>

#include <SDL2/SDL.h>

#include "Pathfinder/triangle_lib_navmesh.h"
#include "Pathfinder/behaviorBuilder.h"
#include "Pathfinder/navmesh_interface.h"
#include "Pathfinder/pathfinder.h"
#include "Pathfinder/vector.h"

inline constexpr uint16_t WINDOW_WIDTH = 1000;
inline constexpr uint16_t WINDOW_HEIGHT = 1000;

inline bool operator==(SDL_Point const &a, SDL_Point const &b)
{
    return a.x == b.x && a.y == b.y;
}

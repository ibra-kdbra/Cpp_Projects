#include "object.h"
#include <fstream>

uint16_t Object::current_id{1};

Object::Object(int x, int y, int w, int h, SDL_Color color) : color(color), id(current_id++), current_status(status::STATIC) {
    rect.w = w;
    rect.h = h;
    rect.x = x-w/2;
    rect.y = y-h/2;
    border_mask.resize(WINDOW_HEIGHT*WINDOW_WIDTH, false);
    area_available.resize(WINDOW_HEIGHT*WINDOW_WIDTH, false);
}

////////////PRIVATE METHODS////////////

void Object::BFS_alg_fill_area (int16_t x, int16_t y) {
    int count {0};
    std::queue<std::pair<int16_t, int16_t>> q;
    area_available[WINDOW_WIDTH*y+x] = true;
    path.distances.clear();
    q.push(std::make_pair(x, y));
    while (!q.empty()) {
        auto p = q.front();
        q.pop();
        int16_t row = p.second;
        int16_t col = p.first;

        if ( (row < WINDOW_HEIGHT-rect.h/2-1) &&
            !area_available[WINDOW_WIDTH*(row+1)+col] &&
            !border_mask[WINDOW_WIDTH*(row+1)+col] )

        {   q.push(std::make_pair(col, row+1));
            int index =  WINDOW_WIDTH*(row+1)+col;
            area_available[index] = true; count++;
            float dist = static_cast<float>(std::sqrt(std::pow(path.goal_p.x - col, 2) + std::pow(path.goal_p.y - row+1, 2)));
            path.distances.insert({dist, index});
        }

        if ((row > rect.h/2) &&
            !area_available[WINDOW_WIDTH*(row-1)+col] &&
            !border_mask[WINDOW_WIDTH*(row-1)+col])

        {   q.push(std::make_pair(col, row-1));
            int index =  WINDOW_WIDTH*(row-1)+col;
            area_available[index] = true; count++;
            float dist = static_cast<float>(std::sqrt(std::pow(path.goal_p.x - col, 2) + std::pow(path.goal_p.y - row-1, 2)));
            path.distances.insert({dist, index});
        }

        if ((col < WINDOW_WIDTH-rect.w/2-1) &&
            !area_available[WINDOW_WIDTH*row+col+1] &&
            !border_mask[WINDOW_WIDTH*row+col+1])

        {   q.push(std::make_pair(col+1, row));
            int index =  WINDOW_WIDTH*row+col+1;
            area_available[index] = true; count++;
            float dist = static_cast<float>(std::sqrt(std::pow(path.goal_p.x - col+1, 2) + std::pow(path.goal_p.y - row, 2)));
            path.distances.insert({dist, index});
        }

        if ((col > rect.w/2) &&
            !area_available[WINDOW_WIDTH*row+col-1] &&
            !border_mask[WINDOW_WIDTH*row+col-1])

        {   q.push(std::make_pair(col-1, row));
            int index =  WINDOW_WIDTH*row+col-1;
            area_available[index] = true;count++;
            float dist = static_cast<float>(std::sqrt(std::pow(path.goal_p.x - col-1, 2) + std::pow(path.goal_p.y - row, 2)));
            path.distances.insert({dist, index});
        }
    }
}

void Object::createNavmesh (const std::vector<Object>& objs) {
    navmesh.reset();
    triangle::triangleio inputData;
    triangle::triangle_initialize_triangleio(&inputData);

    int static_obj_and_masks{0};
    int v_id{0};
    for (const auto& obj : objs)
        if (obj.current_status == STATIC) static_obj_and_masks++;
    int total_vertex = (static_obj_and_masks+2) * 4;
    inputData.numberofpoints = total_vertex;
    inputData.numberofsegments = total_vertex;
    inputData.pointlist = new double[2*total_vertex];
    inputData.segmentlist = new int[2*total_vertex];
    inputData.segmentmarkerlist = new int[total_vertex];

    for (int i = 0; i < total_vertex; i++) inputData.segmentmarkerlist[i] = 3;

    int w = rect.w/2;
    int h = rect.h/2;

    inputData.pointlist[0] = w-0.2;
    inputData.pointlist[1] = h-0.2;
    inputData.pointlist[2] = static_cast<double>(WINDOW_WIDTH)+0.2-w;
    inputData.pointlist[3] = h-0.2;
    inputData.pointlist[4] = static_cast<double>(WINDOW_WIDTH)+0.2-w;
    inputData.pointlist[5] = static_cast<double>(WINDOW_HEIGHT)+0.2-h;
    inputData.pointlist[6] = w-0.2;
    inputData.pointlist[7] = static_cast<double>(WINDOW_HEIGHT)+0.2-h;
    inputData.pointlist[8] = 0;
    inputData.pointlist[9] = 0;
    inputData.pointlist[10] = static_cast<double>(WINDOW_WIDTH);
    inputData.pointlist[11] = 0;
    inputData.pointlist[12] = static_cast<double>(WINDOW_WIDTH);
    inputData.pointlist[13] = static_cast<double>(WINDOW_HEIGHT);
    inputData.pointlist[14] = 0;
    inputData.pointlist[15] = static_cast<double>(WINDOW_HEIGHT);

    inputData.segmentlist[0] = 0;
    inputData.segmentlist[1] = 1;
    inputData.segmentlist[2] = 1;
    inputData.segmentlist[3] = 2;
    inputData.segmentlist[4] = 2;
    inputData.segmentlist[5] = 3;
    inputData.segmentlist[6] = 3;
    inputData.segmentlist[7] = 0;
    inputData.segmentlist[8] = 4;
    inputData.segmentlist[9] = 5;
    inputData.segmentlist[10] = 5;
    inputData.segmentlist[11] = 6;
    inputData.segmentlist[12] = 6;
    inputData.segmentlist[13] = 7;
    inputData.segmentlist[14] = 7;
    inputData.segmentlist[15] = 4;

    int list_count = 15;

    for (const auto& obj : objs) {
        if (obj.current_status != STATIC) continue;

        int x = obj.rect.x;
        int y = obj.rect.y;

        inputData.pointlist[++list_count]       = (x-w) <= 0     ? 0.2 : x-w+0.2;
        inputData.pointlist[++list_count]       = (y-h) <= 0     ? 0.2 : y-h+0.2;
        inputData.pointlist[++list_count]       = (x+obj.rect.w+w) >= WINDOW_WIDTH ? WINDOW_WIDTH-0.2 : x+obj.rect.w+w-0.2;
        inputData.pointlist[++list_count]       = (y-h) <= 0     ? 0.2 : y-h+0.2;
        inputData.segmentlist[(list_count-3)]   = list_count/2-1;
        inputData.segmentlist[(list_count-2)]   = list_count/2;
        inputData.pointlist[++list_count]       = (x+obj.rect.w+w) >= WINDOW_WIDTH ? WINDOW_WIDTH-0.2 : x+obj.rect.w+w-0.2;
        inputData.pointlist[++list_count]       = (y+obj.rect.h+h) >= WINDOW_HEIGHT ? WINDOW_HEIGHT-0.2 : y+obj.rect.h+h-0.2;
        inputData.segmentlist[(list_count-3)]   = list_count/2-1;
        inputData.segmentlist[(list_count-2)]   = list_count/2;
        inputData.pointlist[++list_count]       = (x-w) <= 0    ? 0.2 : x-w+0.2;
        inputData.pointlist[++list_count]       = (y+obj.rect.h+h) >= WINDOW_HEIGHT ? WINDOW_HEIGHT-0.2 : y+obj.rect.h+h-0.2;
        inputData.segmentlist[(list_count-3)]   = list_count/2-1;
        inputData.segmentlist[(list_count-2)]   = list_count/2;
        inputData.segmentlist[(list_count-1)]   = list_count/2;
        inputData.segmentlist[(list_count)]     = list_count/2-3;
    }

    triangle::context *ctx;
    ctx = triangle::triangle_context_create();
    pathfinder::BehaviorBuilder behaviorBuilder_;
    *(ctx->b) = behaviorBuilder_.getBehavior();

    int mesh_create = triangle::triangle_mesh_create(ctx, &inputData);

    if(mesh_create != 0) {
        std::cout<<"Error creating navmesh "; // #TODO: ######Need to fix it somehow ???#####
        // TRI_SEG_INTERSECT (-6) library error: segmentintersection()
        // Topological inconsistency after splitting a segment.
        writePoly(inputData, "error_mesh.poly"); //for debug
        std::cout<<"Error: " << mesh_create <<std::endl;

        delete[] inputData.pointlist;
        delete[] inputData.segmentlist;
        delete[] inputData.segmentmarkerlist;
        triangle_context_destroy(ctx);
        return;
    };

    triangle::triangleio triangleData, triangleVoronoiData;
    triangle::triangle_initialize_triangleio(&triangleData);
    triangle::triangle_initialize_triangleio(&triangleVoronoiData);

    triangle::triangle_mesh_copy(ctx, &triangleData, 1, 1, &triangleVoronoiData);
    triangle_context_destroy(ctx);

    navmesh = std::shared_ptr<pathfinder::navmesh::NavmeshInterface>
        (new pathfinder::navmesh::TriangleLibNavmesh(triangleData, triangleVoronoiData));

    triangle_free_triangleio(&triangleData);
    triangle_free_triangleio(&triangleVoronoiData);

    delete[] inputData.pointlist;
    delete[] inputData.segmentlist;
    delete[] inputData.segmentmarkerlist;

}

void Object::createMask(const std::vector<bool>& map) {
    std::fill(area_available.begin(), area_available.end(), false);
    std::fill(border_mask.begin(), border_mask.end(), false);
    for (int i = 0; i < WINDOW_WIDTH*WINDOW_HEIGHT; i++) {
        if (!map[i]) continue;
        if (abs((i%WINDOW_WIDTH)-((i+1)%WINDOW_WIDTH)) == 1 &&
            ((i+WINDOW_WIDTH) < WINDOW_WIDTH*WINDOW_HEIGHT) &&
            map[i+1] && map[WINDOW_WIDTH+i] && !map[WINDOW_WIDTH+i+1]) {

            for (int n = 0; n <= rect.w/2; n++) {
                if ( ((i-rect.w/2+n)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i-(WINDOW_WIDTH*rect.h/2) > 0) )
                {   int index = (i-rect.w/2+n+1)-(WINDOW_WIDTH*(rect.h/2-1));
                    border_mask[index] = true;
                }
            }
            for (int m = 0; m <= rect.h/2-1; m++) {
                if ( ((i-rect.w/2)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i-(WINDOW_WIDTH*(rect.h/2-m)) > 0) )
                {   int index = (i-rect.w/2+1)-(WINDOW_WIDTH*(rect.h/2-m-1));
                    border_mask[index] = true;
                }
            }
        } else if (abs((i%WINDOW_WIDTH)-((i-1)%WINDOW_WIDTH)) == 1 &&
                   ((i+WINDOW_WIDTH) < WINDOW_WIDTH*WINDOW_HEIGHT) &&
                   map[i-1] && map[WINDOW_WIDTH+i] && !map[WINDOW_WIDTH+i-1]) {

            for (int n = 0; n <= rect.w/2; n++) {
                if ( ((i+n)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i-(WINDOW_WIDTH*rect.h/2) > 0) )
                {
                    int index = (i+n)-(WINDOW_WIDTH*(rect.h/2-1));
                    border_mask[index] = true;
                }
            }
            for (int m = 0; m <= rect.h/2-1; m++) {
                if ( ((i+rect.w/2)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i-(WINDOW_WIDTH*(rect.h/2-m)) > 0) )
                {
                    int index = (i+rect.w/2)-(WINDOW_WIDTH*(rect.h/2-m-1));
                    border_mask[index] = true;
                }
            }
        } else if (abs((i%WINDOW_WIDTH)-((i-1)%WINDOW_WIDTH)) == 1 &&
                   (i - WINDOW_WIDTH >= 0) &&
                   map[i-1] && map[i-WINDOW_WIDTH] && !map[i-WINDOW_WIDTH-1]) {

            for (int n = 0; n <= rect.w/2-1; n++) {
                if ( ((i+n)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i+(WINDOW_WIDTH*rect.h/2) < WINDOW_WIDTH*WINDOW_HEIGHT) )
                {
                    int index = (i+n)+(WINDOW_WIDTH*rect.h/2);
                    border_mask[index] = true;
                }
            }
            for (int m = 0; m <= rect.h/2; m++) {
                if ( ((i+rect.w/2)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i+(WINDOW_WIDTH*(rect.h/2-m)) > 0) )
                {
                int index = (i+rect.w/2)+(WINDOW_WIDTH*(rect.h/2-m));
                border_mask[index] = true;
                }
            }
        } else if (abs((i%WINDOW_WIDTH)-((i+1)%WINDOW_WIDTH)) == 1 &&
                   (i - WINDOW_WIDTH >= 0) &&
                   map[i+1] && map[i-WINDOW_WIDTH] && !map[i-WINDOW_WIDTH+1]) {

            for (int n = 0; n <= rect.w/2; n++) {
                if ( ((i-rect.w/2+n)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i+(WINDOW_WIDTH*rect.h/2) < WINDOW_WIDTH*WINDOW_HEIGHT) )
                {
                int index = (i-rect.w/2+n+1)+(WINDOW_WIDTH*rect.h/2);
                border_mask[index] = true;
                }
            }
            for (int m = 0; m <= rect.h/2; m++) {
                if ( ((i-rect.w/2)/WINDOW_WIDTH == i/WINDOW_WIDTH) && (i+(WINDOW_WIDTH*(rect.h/2-m)) < WINDOW_WIDTH*WINDOW_HEIGHT) )
                {
                int index = (i-rect.w/2+1)+(WINDOW_WIDTH*(rect.h/2-m));
                border_mask[index] = true;
                }
            }
        } else if ((WINDOW_WIDTH+i < WINDOW_WIDTH*WINDOW_HEIGHT) && (i-WINDOW_WIDTH >= 0) &&
                   (i-(rect.h/2*WINDOW_WIDTH) >= 0) && (i+(rect.h/2*WINDOW_WIDTH) < WINDOW_WIDTH*WINDOW_HEIGHT) &&
                   !map[WINDOW_WIDTH+i] && !map[i-WINDOW_WIDTH]) {
            int indexHorizUp = i-((rect.h/2-1)*WINDOW_WIDTH);
            border_mask[indexHorizUp] = true;
            int indexHorizDown = i+(rect.h/2*WINDOW_WIDTH);
            border_mask[indexHorizDown] = true;
        } else if (abs((i%WINDOW_WIDTH)-((i+1)%WINDOW_WIDTH)) == 1 && abs((i%WINDOW_WIDTH)-((i-1)%WINDOW_WIDTH)) == 1 &&
                   (i+rect.w/2)/WINDOW_WIDTH == (i-rect.w/2)/WINDOW_WIDTH &&
                   !map[i+1] && !map[i-1]) {
            int indexVertLeft = i-rect.w/2+1;
            border_mask[indexVertLeft] = true;
            int indexVertRight = i+rect.w/2;
            border_mask[indexVertRight] = true;
        }
    }

}

void Object::writePoly(const auto& inputData, std::string filename) {
    std::string res;
    res += std::to_string(inputData.numberofpoints);
    res += " 2 0 0\n";
    for (int i=0; i<inputData.numberofpoints; i++) {
        res += std::to_string(i);
        res += " ";
        res += std::to_string(inputData.pointlist[2*i]);
        res += " ";
        res += std::to_string(inputData.pointlist[2*i + 1]);
        res += '\n';
    }
    res += std::to_string(inputData.numberofpoints);
    res += " 1\n";
    for (int i=0; i<inputData.numberofsegments; i++) {
        res += std::to_string(i);
        res += " ";
        res += std::to_string(inputData.segmentlist[2*i]);
        res += " ";
        res += std::to_string(inputData.segmentlist[2*i + 1]);
        res += " ";
        res += std::to_string(inputData.segmentmarkerlist[i]);
        res += '\n';
    }
    res += "0";
    std::ofstream out(filename);
    out << res;
    out.close();
}

////////////PUBLIC METHODS////////////

bool Object::operator == (const Object& obj) const {
    return SDL_RectEquals(&this->rect, &obj.rect);
}

std::optional<SDL_Point> Object::getDestination () const {
    if (path.destination_p == SDL_Point{-1, -1}) return {};
    return path.destination_p;
}

void Object::setDest(const SDL_Point point) {
    path.destination_p = point;
    if (getDestination().has_value())
        rect_dest_mask = {point.x - rect.w/2, point.y - rect.h/2, rect.w, rect.h};
}

void Object::setGoal(const SDL_Point point) {
    path.goal_p = point;
    path.start_p = getPos();
}

void Object::setPos(const SDL_Point point) {
    rect.x = point.x-rect.w/2; rect.y = point.y-rect.h/2;
}

void Object::buildPath (const std::vector<Object>& objs) {
    if (!(current_status == NEED_TO_BUILD_PATH || current_status == NEED_TO_BUILD_PATH_FOR_ALT_DEST)) return;
    createNavmesh(objs);
    mu.lock();
    pathfinder::Pathfinder pathfinder(reinterpret_cast<pathfinder::navmesh::AStarNavmeshInterface&>(*navmesh.get()), 0);
    mu.unlock();
    std::optional<pathfinder::Vector> startPoint_{std::in_place, rect.x+rect.w/2, rect.y+rect.h/2};
    std::optional<pathfinder::Vector> goalPoint_{std::in_place, path.destination_p.x, path.destination_p.y};

    auto pathfindingResult_ = pathfinder.findShortestPath(*startPoint_, *goalPoint_);

    if (pathfindingResult_.shortestPath.empty()) {
        current_status = NEED_TO_FIND_ALT_DEST;
        //std::cout<<"Unable to build path from StartP "<<startPoint_.value().x()<<" "<<startPoint_.value().y()<<" to DestP "<<
        //    goalPoint_.value().x()<<" "<<goalPoint_.value().y()<<std::endl;
        setDest({-1,-1});
        return;
    }
    path.segments.clear();
    hasPath = true;
    for  (auto& segment : pathfindingResult_.shortestPath) {
        int seg_x_start = round(static_cast<pathfinder::StraightPathSegment*>(segment.get())->startPoint.x());
        int seg_y_start = round(static_cast<pathfinder::StraightPathSegment*>(segment.get())->startPoint.y());
        int seg_x_end = round(static_cast<pathfinder::StraightPathSegment*>(segment.get())->endPoint.x());
        int seg_y_end = round(static_cast<pathfinder::StraightPathSegment*>(segment.get())->endPoint.y());
        path.segments.emplace_back(std::make_pair<SDL_Point, SDL_Point>({seg_x_start, seg_y_start}, {seg_x_end, seg_y_end}));
    }
    current_status = MOVING;
    path.prevTicks = SDL_GetPerformanceCounter();
    path.current_segment = 0;
}

void Object::flood_fill(const std::vector<bool>& map) {
    if (current_status != NEED_TO_FIND_ALT_DEST) return;
    createMask(map);

    auto start = std::chrono::high_resolution_clock::now();
    BFS_alg_fill_area(getPos().x, getPos().y);
    auto stop = std::chrono::high_resolution_clock::now();
    std::cout << ">>time: BFS_alg_fill_area = " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << "[µs]" << std::endl;
}

bool Object::find_alt_dest() {
    if (current_status != NEED_TO_FIND_ALT_DEST) return true;

    auto min_available_dist_it =
        std::find_if(path.distances.begin(), path.distances.end(),[this](const auto& el){return area_available[el.second];});

    if (min_available_dist_it == path.distances.end()) return false;

    auto index = min_available_dist_it->second;

    setDest({index%WINDOW_WIDTH, index/WINDOW_WIDTH});
    current_status = NEED_TO_BUILD_PATH_FOR_ALT_DEST;
    if (getDestination().value() == getPos()) current_status = DEST_ACHIEVED;

    return true;
}

void Object::add_new_mask (const std::vector<SDL_Rect>& masks) {
    for (const auto& mask_rect : masks) {
        for (int n = 0; n < mask_rect.w+rect.w; n++) {
            for (int m = 0; m < mask_rect.h+rect.h; m++) {
                if ((mask_rect.y+m-rect.h/2) >= WINDOW_HEIGHT ||
                    (mask_rect.y+m-rect.h/2) < 0 ||
                    (mask_rect.x+n-rect.w/2) >= WINDOW_WIDTH ||
                    (mask_rect.x+n-rect.w/2) < 0 )
                        continue;
                int index = WINDOW_WIDTH*(mask_rect.y+m-rect.h/2)+(mask_rect.x+n-rect.w/2);
                if (area_available[index]) area_available[index] = false;
            }
        }
    }
}

void Object::process_next_segment() {
    path.current_segment++;
    path.prevTicks = SDL_GetPerformanceCounter();
    if (path.current_segment == path.segments.size()) {
        path.current_segment = 0;
        hasPath = false;
    }
}

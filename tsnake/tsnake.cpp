/* tsnake.cpp */

#include <iostream>
#include <string>
#include <deque>
#include <algorithm>
#include <stdexcept>
#include <cmath>

#include <curses.h>
#include <stdlib.h>
#include <time.h>

#include "InputParser.h"

/* defines */
#define VERSION     "0.1.5"

// number of mpas
#define N_MAPS      5

// characters
#define EMPTY  	    ' '
#define SNAKE       'o'
#define FOOD        '@'
#define WALL        '#'
#define WATER       '^'

// directions
#define UP          0
#define DOWN        1
#define RIGHT       2
#define LEFT        3

// initial lenght
#define START_LEN   4

// alignments
#define ALIGN_RIGHT 0
#define ALIGN_LEFT  1

// return codes
#define R_QUIT            0
#define R_RESTART_NEW     1
#define R_RESTART_SAME    2

// color locations
#define C_DEFAULT 1
#define C_FOOD    2
#define C_SNAKE   3
#define C_STATUS  4
#define C_BORDER  5
#define C_GREEN   6
#define C_WALL    7
#define C_SNAKE_H 8
#define C_WATER   9

/* structs */
struct point {
    int x, y;
};
struct pointf {
    float x, y;
};

struct game_state {
    // current position of the head of the snake
    point pos;
    // current direction
    int dir;
    // current score
    int score;
    // snake speed in seconds/cell
    float speed;
    // are we running?
    bool running;
    // game is paused
    bool paused;
    // snake queue
    std::deque<point> snake;
    // current food position
    point food;
    // current food relative position
    pointf food_rel;
    // flag that goes up when we eat
    bool f_eat;
    // timers
    clock_t curr, last;
    // game window
    WINDOW* gamew;
    // game window size
    int gw_w, gw_h;
};

/* function definitions */
void update(game_state* state, int newy, int newx);
void do_chdir(game_state* state, int newy, int newx, int newdir, int opposite_dir);
int out_of_boudns(game_state* state, int y, int x);
int collision_check(game_state* state, int y, int x);
void draw_map(game_state* state, int map);
void print_bottom(char* text);
void create_food(game_state* state);
void print_status(std::string status, int align, int col);
int ask_end();
void redraw_entities(game_state* state);
void term_size_check();
bool speed_scl(game_state* state, float scale);
bool speed_add(game_state* state, float add);
bool speed_up(game_state* state);
bool speed_down(game_state* state);
int start_game(int start_length, int map);

/* global variables */
bool cheat;

template <typename T>
T clip(const T& n, const T& lower, const T& upper) {
  return std::max(lower, std::min(n, upper));
}

int main(int argc, char** argv)
{
    InputParser ip(argc, argv);
    if(ip.exists("-h") || ip.exists("--help")){
        std::cout << "tsnake version " << VERSION << std::endl;
        std::cout << std::endl;
        std::cout << "tsnake is a simple terminal snake game written in C++ with ncurses." << std::endl;
        std::cout << "Move the snake with the arrows, [wasd] or [hjkl]." << std::endl;
        std::cout << "Use [p] to pause game, [r] to restart it with a new map and" << std::endl;
        std::cout << "[q] to quit." << std::endl;
        std::cout << "The game starts with a speed of 2 m/s and every 20 points the speed" << std::endl;
        std::cout << "is increased by one m/s until a maximum of 20 m/s." << std::endl;
        std::cout << "If cheat mode is enabled (--cheat), the speed can be increased and" << std::endl;
        std::cout << "decreased with [+] and [-]." << std::endl;
        std::cout << "The game has a few maps which can be chosen using the -m option." << std::endl;
        std::cout << "Otherwise, maps are cycled automatically when the game is restarted." << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: tsnake [OPTION]..." << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << " -h, --help";
        std::cout << "\t\tshows usage information" << std::endl;
        std::cout << " -v, --version";
        std::cout << "\t\tprints version and exit" << std::endl;
        std::cout << " -n [SIZE]";
        std::cout << "\tset inital size of snake, which defaults to " << START_LEN << std::endl;
        std::cout << " -m [MAP_NUM]";
        std::cout << "\tnumber of the first map to use as an integer, which is cycled\n\t\tusing (map \% nmaps)" << std::endl;
        std::cout << " -c";
        std::cout << "\t\tdeactivate colors" << std::endl;
        std::cout << " -x, --cheat";
        std::cout << "\tactivate cheat mode where speed can be increased and\n\t\tdecreased with '+' and '-'" << std::endl;
        return 0;
    }
    if(ip.exists("-v") || ip.exists("--version")){
        std::cout << "tsnake version " << VERSION << std::endl;
        return 0;
    }

    /* initial snake length */
    int start_length = START_LEN;
    if(ip.exists("-n")){
        try{
            start_length = ip.getInt("-n");
        }catch(const std::invalid_argument& ia){
            std::cout << "Bad argument: n = '" << ip.getStr("-n") << "', must be an integer" << std::endl;
            return 0;
        }
    }

    /* first map */
    int map_id = 0;
    if(ip.exists("-m")){
        try{
            map_id = ip.getInt("-m");
        }catch(const std::invalid_argument& ia){
            std::cout << "Bad argument: m = '" << ip.getStr("-m") << "', must be an integer" << std::endl;
            return 0;
        }
    }

    /* cheat mode */
    cheat = ip.exists("--cheat") || ip.exists("-x");

    /* colors */
    bool colors = !ip.exists("-c");

    /* init random */
    srand(time(NULL));

    /* initialize curses */
    initscr();
    if(colors){
        start_color();
        // use default colors to not break current color scheme
        use_default_colors();
    }
    keypad(stdscr, TRUE);
    cbreak();
    noecho();
    curs_set(0);
    short r, g, b;
    color_content(COLOR_BLACK, &r, &g, &b);
    // set bright black black
    init_color(8, r, g, b);
    init_pair(C_STATUS, COLOR_BLACK, COLOR_CYAN);
    init_pair(C_FOOD, COLOR_CYAN, COLOR_MAGENTA);
    init_pair(C_SNAKE, COLOR_RED, COLOR_GREEN);
    init_pair(C_SNAKE_H, COLOR_GREEN, COLOR_RED);
    init_pair(C_DEFAULT, COLOR_WHITE, COLOR_BLACK);
    init_pair(C_BORDER, COLOR_BLUE, COLOR_BLACK);
    init_pair(C_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(C_WALL, COLOR_BLACK, COLOR_RED);
    init_pair(C_WATER, COLOR_BLACK, COLOR_BLUE);

    clear();

    /* start the game */
    start_length = clip(start_length, 1, (COLS - 5) / 2);

    int ret = R_RESTART_SAME;
    while(ret != R_QUIT){
        if(ret == R_RESTART_SAME)
            ret = start_game(start_length, map_id);
        else if(ret == R_RESTART_NEW)
            ret = start_game(start_length, ++map_id);
    }

    /* clean up */
    endwin();
    exit(0);
}

void term_size_check(){
    if(LINES < 15 || COLS < 60){
        clear();
        endwin();
        std::cout << "Terminal too small! (min [60,15], current [" << COLS << "," << LINES << "]" << std::endl;
        exit(0);
    }
}

int start_game(int start_length, int map)
{
    /* new game state */
    game_state state;

    /* auxiliary variables */
    float secs;
    clock_t start;
    int ch;

    term_size_check();

    /* create window */
    state.gw_w = COLS;
    state.gw_h = LINES - 1;
    state.gamew = newwin(state.gw_h, state.gw_w, 0, 0);

    /* initialize the map if any */
    draw_map(&state, map);

    /* start player at [start_length,5] going right */
    state.pos.x = start_length + 1;
    state.pos.y = 2;
    state.dir = RIGHT;
    state.score = 0;
    start = clock();

    /* init snake */
    wattron(state.gamew, COLOR_PAIR(C_SNAKE));
    state.snake.clear();
    for(int i = 0; i < start_length; i++) {
        point p = {state.pos.x + 1 - start_length + i, state.pos.y};
        state.snake.push_front(p);
        mvwaddch(state.gamew, p.y, p.x, SNAKE);
    }
    wattroff(state.gamew, COLOR_PAIR(C_SNAKE));

    wattron(state.gamew, COLOR_PAIR(C_SNAKE_H));
    mvwaddch(state.gamew, state.snake.front().y, state.snake.front().x, SNAKE);
    wattroff(state.gamew, COLOR_PAIR(C_SNAKE_H));

    /* init food */
    create_food(&state);

    refresh();
    wrefresh(state.gamew);

    /* speed in m/s */
    state.speed = 2.0;

    /* clocks */
    state.last = 0;

    /* async char read */
    nodelay(stdscr, TRUE);

    /* run */
    state.running = true;
    state.paused = false;

    bool resize = false;

    do {
        resize = state.gw_w != COLS || state.gw_h != LINES - 1;
        if(resize){
            term_size_check();
            state.gw_w = COLS;
            state.gw_h = LINES - 1;
            wresize(state.gamew, state.gw_h, state.gw_w);

            draw_map(&state, map);
            redraw_entities(&state); 

            wrefresh(state.gamew);
        } else if(!state.paused){
            state.curr = clock();


            /* get char async, see nodelay() */
            ch = getch();

            /* test inputted key and determine direction */
            if(ch != ERR){
                switch (ch) {
                    case KEY_UP:
                    case 'w':
                    case 'k':
                        if(state.dir != DOWN)
                            do_chdir(&state, state.pos.y - 1, state.pos.x, UP, DOWN);
                        break;
                    case KEY_DOWN:
                    case 's':
                    case 'j':
                        if(state.dir != UP)
                            do_chdir(&state, state.pos.y + 1, state.pos.x, DOWN, UP);
                        break;
                    case KEY_LEFT:
                    case 'a':
                    case 'h':
                        if(state.dir != RIGHT)
                            do_chdir(&state, state.pos.y, state.pos.x - 1, LEFT, RIGHT);
                        break;
                    case KEY_RIGHT:
                    case 'd':
                    case 'l':
                        if(state.dir != LEFT)
                            do_chdir(&state, state.pos.y, state.pos.x + 1, RIGHT, LEFT);
                        break;
                    case 'q':
                        // show quit action
                        state.running = false;
                        state.curr = state.last = clock();
                        break;
                    case 'p':
                        // pause
                        state.paused = true;
                        break;
                    case 'r':
                        // restart
                        return R_RESTART_NEW;
                    case '+':
                        if(cheat && speed_up(&state))
                            state.curr = state.last = clock();
                        break;
                    case '-':
                        if(cheat && speed_down(&state))
                            state.curr = state.last = clock();
                        break;
                }
            }

            float dt = ((float)(state.curr - state.last) / CLOCKS_PER_SEC);
            float secs_per_cell = 1.0F / state.speed;
            if (dt > secs_per_cell) {
                state.last = state.curr;
                /* auto-move */
                switch (state.dir) {
                    case UP:
                        state.pos.y--;
                        break;
                    case DOWN:
                        state.pos.y++;
                        break;
                    case RIGHT:
                        state.pos.x++;
                        break;
                    case LEFT:
                        state.pos.x--;
                        break;
                }
                if(collision_check(&state, state.pos.y, state.pos.x)) {
                    /* end */
                    state.running = false;
                    break;
                } else {
                    /* update */
                    update(&state, state.pos.y, state.pos.x);
                }
            }

            /* chech food */
            if(state.food.x == state.pos.x && state.food.y == state.pos.y){
                create_food(&state);
                state.f_eat = true;
                state.score++;
                if (state.score % 20 == 0)
                    speed_up(&state);
            }



        }else{
            // paused
            ch = getch();
            if(ch == 'p'){
                state.paused = false;
            }    
        }

        /* status */
        attron(COLOR_PAIR(C_STATUS));
        mvhline(LINES - 1, 0, EMPTY, COLS);
        attroff(COLOR_PAIR(C_STATUS));

        secs = ((float)(state.curr - start) / CLOCKS_PER_SEC);
        std::string st = "Score: " + std::to_string(state.score) + " | " + std::to_string((int) secs) + " seconds | " + std::to_string((int) state.speed) + " m/s";

        print_status(" [p]Pause [r]Restart [q]Quit", ALIGN_LEFT, A_BOLD | COLOR_PAIR(C_STATUS));
        print_status(st, ALIGN_RIGHT, COLOR_PAIR(C_STATUS));

        
        /* title */
        wattron(state.gamew, COLOR_PAIR(C_BORDER));
        box(state.gamew, 0, 0);
        if(!state.paused) {
            std::string mapstr;
            mapstr.append(" TSNAKE ");
            mapstr.append(VERSION);
            mapstr.append(" - MAP " + std::to_string(map % N_MAPS  + 1) + "  (" + std::to_string(COLS) + "x" + std::to_string(LINES) + ") ");
            mvwaddstr(state.gamew, 0, 4, mapstr.c_str());
        }else{
            wattron(state.gamew, COLOR_PAIR(C_GREEN));
            mvwaddstr(state.gamew, 0, 4, " GAME PAUSED - [p]Continue ");
            wattroff(state.gamew, COLOR_PAIR(C_GREEN));
        }
        wattroff(state.gamew, COLOR_PAIR(C_BORDER));
        
        /* refresh */
        refresh();
        wrefresh(state.gamew);
    }
    while (state.running);

    /* done */
    std::string msg3 = "YOUR SCORE: " + std::to_string(state.score);
    std::string msg4 = "You lasted " + std::to_string((int) secs) + " seconds";
    std::string msg1 = "[r] Restart (new map)";
    std::string msg0 = "[s] Restart (same map)";
    std::string msg2 = "[q] Quit";
    int minl = msg1.size();
    int ew_w = clip(COLS / 2, minl, COLS);
    int ew_h = clip(LINES / 2, 4, LINES);
    WINDOW* endw = newwin(ew_h, ew_w, (LINES - ew_h) / 2, (COLS - ew_w) / 2);
    wbkgd(endw, COLOR_PAIR(C_STATUS));
    nodelay(stdscr, FALSE);
    box(endw, 0, 0);

    /* title */
    mvwaddstr(endw, 0, 2, " GAME FINISHED ");
    /* score and seconds in green */
    mvwaddstr(endw, ew_h / 2 - 3, ew_w / 2 - msg3.size() / 2, msg3.c_str());
    mvwaddstr(endw, ew_h / 2 - 2, ew_w / 2 - msg4.size() / 2, msg4.c_str());

    wattron(endw, A_BOLD);
    mvwaddstr(endw, ew_h / 2 + 1, ew_w / 2 - minl / 2, msg1.c_str());
    mvwaddstr(endw, ew_h / 2 + 2, ew_w / 2 - minl / 2, msg0.c_str());
    mvwaddstr(endw, ew_h / 2 + 3, ew_w / 2 - minl / 2, msg2.c_str());
    wattroff(endw, A_BOLD);    

    wrefresh(endw);

    return ask_end();
}

int ask_end()
{
    int opt = getch();

    switch(opt){
        case 'r':
            /* new game, new map */
            return R_RESTART_NEW;
        case 's':
            /* new game, same map */
            return R_RESTART_SAME;
        case 'q':
            /* quit */
            return R_QUIT;
        default:
            return ask_end();
    }
}

void print_status(std::string status, int align, int col)
{
    attron(col);
    switch(align){
        case ALIGN_LEFT:
            move(LINES - 1, 0);
            printw(status.c_str());
            break;
        case ALIGN_RIGHT:
            move(LINES - 1, COLS - status.size() - 1);
            printw(status.c_str());
            break;
    }
    attroff(col);
}

void redraw_entities(game_state* state){
    /* snake */
    
    // head
    wattron(state->gamew, COLOR_PAIR(C_SNAKE_H));
    mvwaddch(state->gamew, state->snake[0].y, state->snake[0].x, SNAKE);
    wattron(state->gamew, COLOR_PAIR(C_SNAKE_H));

    // body
    wattron(state->gamew, COLOR_PAIR(C_SNAKE));
    for(unsigned int i = 1; i < state->snake.size(); i++){
        mvwaddch(state->gamew, state->snake[i].y, state->snake[i].x, SNAKE);
    }
    wattron(state->gamew, COLOR_PAIR(C_SNAKE));


    /* food */
    
    // reposition
    state->food.x = (int) round((float) state->gw_w * state->food_rel.x);
    state->food.y = (int) round((float) state->gw_h * state->food_rel.y);

    // draw
    wattron(state->gamew, COLOR_PAIR(C_FOOD));
    mvwaddch(state->gamew, state->food.y, state->food.x, FOOD);
    wattroff(state->gamew, COLOR_PAIR(C_FOOD));
}

point rd(game_state* state)
{
    point c;
    while(1){
        c.x = rand() % (state->gw_w - 2) + 1;
        c.y = rand() % (state->gw_h - 2) + 1;

        if((mvwinch(state->gamew, c.y, c.x) & A_CHARTEXT) == EMPTY)
            return c;
    }
}

void create_food(game_state* state)
{
    point newp = rd(state);
    state->food.x = newp.x;
    state->food.y = newp.y;
    state->food_rel.x = (float) newp.x / (float) state->gw_w;
    state->food_rel.y = (float) newp.y / (float) state->gw_h;
    wattron(state->gamew, COLOR_PAIR(C_FOOD));
    mvwaddch(state->gamew, state->food.y, state->food.x, FOOD);
    wattroff(state->gamew, COLOR_PAIR(C_FOOD));
}

void update(game_state* state, int newy, int newx)
{
    point aux = state->snake.front();
    wattron(state->gamew, COLOR_PAIR(C_SNAKE));
    mvwaddch(state->gamew, aux.y, aux.x, SNAKE);
    wattroff(state->gamew, COLOR_PAIR(C_SNAKE));

    point newpoint = {newx, newy};
    state->snake.push_front(newpoint);

    wattron(state->gamew, COLOR_PAIR(C_SNAKE_H));
    mvwaddch(state->gamew, newy, newx, SNAKE);
    wattroff(state->gamew, COLOR_PAIR(C_SNAKE_H));
    if(!state->f_eat){
        point erase = state->snake.back();
        state->snake.pop_back();
        mvwaddch(state->gamew, erase.y, erase.x, EMPTY);
    }else{
        state->f_eat = false;
    }
    move(state->pos.y, state->pos.x);
}

void do_chdir(game_state* state, int newy, int newx, int newdir, int opposite_dir)
{
    if (!collision_check(state, newy, newx)) {
        update(state, newy, newx);
        state->pos.x = newx;
        state->pos.y = newy;
        state->dir = newdir;
        state->curr = state->last = clock();
    } else if (state->dir != opposite_dir){
        state->running = false;
    }
}

bool speed_up(game_state* state)
{
    return speed_add(state, 1.0);
}

bool speed_down(game_state* state)
{
    return speed_add(state, -1.0);
}

bool speed_scl(game_state* state, float scale)
{
    float cpy = state->speed;
    state->speed = clip(state->speed * scale, 1.0F, 20.0F);
    return cpy != state->speed;
}

bool speed_add(game_state* state, float add)
{
    float cpy = state->speed;
    state->speed = clip(state->speed + add, 1.0F, 20.0F);
    return cpy != state->speed;
}

void print_bottom(const char* text)
{
    move(LINES - 1, 0);
    printw(text);
}

int out_of_bounds(game_state* state, int y, int x)
{
    return y <= 0 || x <= 0 || y >= state->gw_h - 1 || x >= state->gw_w - 1;
}

int collision_check(game_state* state, int y, int x)
{
    int testch = mvwinch(state->gamew, y, x) & A_CHARTEXT;
    return (testch != EMPTY && testch != FOOD) || out_of_bounds(state, y, x);
}

void draw_map(game_state* state, int map)
{
    map = map % N_MAPS;
    int y;
    for (y = 0; y < LINES; y++) {
        mvwhline(state->gamew, y, 0, EMPTY, state->gw_w);
    }

    /* actual map */
    wattron(state->gamew, COLOR_PAIR(C_WALL));
    switch(map){
        case 0:
            {
            /* small pool with a fence */
            
            // pool
            wattroff(state->gamew, COLOR_PAIR(C_WALL));
            wattron(state->gamew, COLOR_PAIR(C_WATER));
            for(int y = state->gw_h * 0.4; y <= state->gw_h * 0.6; y++){
                mvwhline(state->gamew, y, state->gw_w / 3, WATER, state->gw_w / 3); 
            }
            wattroff(state->gamew, COLOR_PAIR(C_WATER));
            wattron(state->gamew, COLOR_PAIR(C_WALL));

            // 5 fences
            int tx = state->gw_w * 0.2; 
            int ty = state->gw_h * 0.2;
            int bx = state->gw_w * 0.8;
            int by = state->gw_h * 0.8;

            mvwvline(state->gamew, ty, tx, WALL, by - ty);
            mvwvline(state->gamew, ty, bx, WALL, by - ty);

            mvwhline(state->gamew, by, tx, WALL, bx - tx + 1);
            mvwhline(state->gamew, ty, tx, WALL, state->gw_w * 0.21);
            mvwhline(state->gamew, ty, state->gw_w * 0.6, WALL, state->gw_w * 0.21);
            
            break;
            }
        case 1:
            {
            /* two walls with a pool in the middle */

            // pool
            wattroff(state->gamew, COLOR_PAIR(C_WALL));
            wattron(state->gamew, COLOR_PAIR(C_WATER));
            for(int y = state->gw_h / 3; y <= state->gw_h * 2 / 3; y++){
                mvwhline(state->gamew, y, state->gw_w / 3, WATER, state->gw_w / 3); 
            }
            wattroff(state->gamew, COLOR_PAIR(C_WATER));
            wattron(state->gamew, COLOR_PAIR(C_WALL));

            // 2 walls
            mvwhline(state->gamew, state->gw_h * 0.2, state->gw_w * 0.3, WALL, state->gw_w * 0.7); 
            mvwhline(state->gamew, state->gw_h * 0.8, 0, WALL, state->gw_w * 0.7); 

            break;
            }
        case 2:
            {
            /* just one vertical wall */
            mvwvline(state->gamew, 0, state->gw_w / 2, WALL, state->gw_h * 0.7); 
            break;
            }
        case 3:
            {
            /* three walls */
            mvwvline(state->gamew, 0, state->gw_w / 2, WALL, state->gw_h * 0.7); 
            mvwhline(state->gamew, state->gw_h / 2, 0, WALL, state->gw_w * 0.3333); 
            mvwhline(state->gamew, state->gw_h / 2, state->gw_w * 0.6666, WALL, state->gw_w * 0.3333 + 1); 
            break;
            }
        case 4:
            {
            /* still three walls, centered */
            int hl = state->gw_w * 0.7;
            int vl = state->gw_h * 0.8;
            mvwvline(state->gamew, (state->gw_h - vl) / 2, state->gw_w / 2, WALL, vl); 
            mvwhline(state->gamew, state->gw_h * 0.3333, (state->gw_w - hl) / 2, WALL, hl); 
            mvwhline(state->gamew, state->gw_h * 0.6666, (state->gw_w - hl) / 2, WALL, hl); 
            break;
            }
    }
    wattroff(state->gamew, COLOR_PAIR(C_WALL));
}


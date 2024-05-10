#pragma once

#include <ncurses.h>

struct NcursesApp
{
 public:
   NcursesApp()
   {
      initscr();
      noecho();
      keypad(stdscr, 1);
      curs_set(0);
   }
   ~NcursesApp()
   {
      endwin();
   }
   void refresh_app()
   {
      refresh();
   }
};

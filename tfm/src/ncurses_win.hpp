#pragma once

#include <ncurses.h>

struct NcursesWin
{
 public:
   WINDOW* win;

 public:
   NcursesWin() : win{ nullptr }
   {
   }
   NcursesWin(WINDOW* parent, int nlines, int ncols, int beginy, int beginx)
   {
      win = subwin(parent, nlines, ncols, beginy, beginx);
   }
   void set(WINDOW* parent, int nlines, int ncols, int beginy, int beginx)
   {
      destroy();
      win = subwin(parent, nlines, ncols, beginy, beginx);
   }
   void destroy()
   {
      if (win != nullptr)
         delwin(win);
   }
   ~NcursesWin()
   {
      destroy();
   }
   WINDOW* getwin()
   {
      return win;
   }
   void refresh_win()
   {
      wrefresh(win);
   }
   void erase_win()
   {
      werase(win);
   }
};

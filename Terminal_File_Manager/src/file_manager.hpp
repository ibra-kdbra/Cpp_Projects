#pragma once

#include <ncurses.h>
#include <string>

#include "./ncurses_win.hpp"

struct FileManager
{
 public:
   NcursesWin title_win{};
   NcursesWin prev_win{};
   NcursesWin cwd_win{};
   NcursesWin content_win{};
   NcursesWin cmd_win{};

 public:
   FileManager(WINDOW* container)
   {
      resize(container);
   }
   void resize(WINDOW* container)
   {
      int maxy, maxx;
      getmaxyx(container, maxy, maxx);

      title_win.set(container, 1, maxx, 0, 0);
      prev_win.set(container, maxy - 2, maxx / 3, 1, 0);
      cwd_win.set(container, maxy - 2, maxx / 3, 1, maxx / 3);
      content_win.set(container, maxy - 2, maxx / 3, 1, 2 * maxx / 3);
      cmd_win.set(container, 1, maxx, maxy - 1, 0);
   }
   void display_err(const std::string& err)
   {
      wattron(cmd_win.getwin(), COLOR_PAIR(3) | A_BOLD);
      cmd_win.erase_win();
      wprintw(cmd_win.getwin(), "%s", err.c_str());
      cmd_win.refresh_win();
      wattroff(cmd_win.getwin(), COLOR_PAIR(3) | A_BOLD);
   }
   void refresh_fm()
   {
      title_win.refresh_win();
      prev_win.refresh_win();
      cwd_win.refresh_win();
      content_win.refresh_win();
      cmd_win.refresh_win();
   }
   void erase_fm()
   {
      title_win.erase_win();
      prev_win.erase_win();
      cwd_win.erase_win();
      content_win.erase_win();
      cmd_win.erase_win();
   }
};

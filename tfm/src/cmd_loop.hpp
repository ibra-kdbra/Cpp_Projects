#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <ncurses.h>

#include "./str_utils.hpp"

using std::string;
using std::vector;

namespace fs = std::filesystem;

enum class Command
{
   DeleteFile,
   RenameFile,
   Quit,
   None,
   Unknown,
   MissingArgs,
   PermissionDenied
};

/**
 * Parses input, executes the command, and returns the command executed
 * or the type of error if there was any (see enum Command).
 * Supported commands:
 *  - q, quit, exit: exit the program
 *  - rm, remove: delete the selected file/directory
 *  - rename NEW_NAME: rename the selected file/directory to NEW_NAME
 */
Command exec_cmd(const string cmd, const string& selected_file)
{
   vector<string> tokens{ split(cmd, " ") };
   if (tokens.size() == 1 && tokens[0] == "")
      return Command::None;

   if (tokens[0] == "q" or tokens[0] == "quit" or tokens[0] == "exit")
   {
      return Command::Quit;
   }
   else if (tokens[0] == "rm" or tokens[0] == "remove")
   {
      try
      {
         fs::remove_all(selected_file);
      }
      catch (fs::filesystem_error& e)
      {
         return Command::PermissionDenied;
      }
      return Command::DeleteFile;
   }
   else if (tokens[0] == "rename")
   {
      if (tokens.size() < 2)
         return Command::MissingArgs;
      else
      {
         try
         {
            fs::rename(selected_file, tokens[1]);
         }
         catch (fs::filesystem_error& e)
         {
            return Command::PermissionDenied;
         }
      }
      return Command::RenameFile;
   }
   return Command::Unknown;
}

std::string cmd_loop(WINDOW* cmd_win)
{
   constexpr int ESC_KEY = 27;
   std::string   s{};
   while (true)
   {
      const int input = getch();
      if (input == ESC_KEY)
         return "";
      if (input == '\n')
      {
         break;
      }
      else if (input == KEY_BACKSPACE && s.size() > 0)
      {
         // Move left
         wmove(cmd_win, getcury(cmd_win), getcurx(cmd_win) - 1);
         wdelch(cmd_win);
         s.pop_back();
         wrefresh(cmd_win);
      }
      else if ((isalpha(input) || input == ' '))
      {
         waddch(cmd_win, unsigned(input));
         wrefresh(cmd_win);
         s += char(input);
      }
   }
   return s;
}

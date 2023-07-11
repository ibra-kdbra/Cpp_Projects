#include <filesystem>
#include <fstream>
#include <locale>
#include <ncurses.h>

#include "./cmd_loop.hpp"
#include "./file_manager.hpp"
#include "./fs_utils.hpp"
#include "./ncurses_app.hpp"

namespace fs = std::filesystem;

int main()
{
   setlocale(LC_ALL, "");

   NcursesApp app{};
   start_color();
   init_pair(1, COLOR_CYAN, 0);  // color for directories
   init_pair(2, COLOR_GREEN, 0); // color for links
   init_pair(3, COLOR_RED, 0);   // color for errors

   FileManager fm{ stdscr };
   // Vectors to hold the content of the previous, current and selected directory
   vector<FileItem> prev_dir_files{};
   vector<FileItem> dir_files{};
   vector<FileItem> selected_dir_files{};

   int selected = 0;
   int begin    = 0;

   while (true)
   {
      // Get content of previous directory
      Err prev_dir_e = get_dir_content(prev_dir_files, "..");
      if (prev_dir_e == Err::PermissionDenied)
         wprintw(fm.prev_win.getwin(), "Permission denied");
      else
         print_dir_content(prev_dir_files, fm.prev_win.getwin());

      // Get content of current directory
      Err err = get_dir_content(dir_files);
      if (err == Err::PermissionDenied)
         wprintw(fm.cwd_win.getwin(), "Permission denied");
      else
         print_dir_content(dir_files, fm.cwd_win.getwin(), selected, begin);

      // Get content of selected directory or file
      if (dir_files.size() == 0)
      {
         wattron(fm.cwd_win.getwin(), COLOR_PAIR(3));
         wprintw(fm.cwd_win.getwin(), "Empty");
         wattroff(fm.cwd_win.getwin(), COLOR_PAIR(3));
      }
      else if (dir_files[size_t(selected)].type == FileType::Directory)
      {
         Err selected_err = get_dir_content(selected_dir_files, dir_files[size_t(selected)].name);
         if (selected_err == Err::PermissionDenied)
         {
            wattron(fm.content_win.getwin(), COLOR_PAIR(3));
            wprintw(fm.content_win.getwin(), "Permission denied");
            wattroff(fm.content_win.getwin(), COLOR_PAIR(3));
         }
         else if (selected_dir_files.size() == 0)
         {
            wattron(fm.content_win.getwin(), COLOR_PAIR(3));
            wprintw(fm.content_win.getwin(), "Empty");
            wattroff(fm.content_win.getwin(), COLOR_PAIR(3));
         }
         else
            print_dir_content(selected_dir_files, fm.content_win.getwin());
      }
      else if (dir_files[size_t(selected)].type == FileType::RegularFile)
      {
         std::ifstream file{ dir_files[size_t(selected)].name };
         if (file.rdstate() == std::ios_base::failbit)
            fm.display_err("Couldn't read the file");
         else if (file.peek() == std::ifstream::traits_type::eof())
         {
            wattron(fm.content_win.getwin(), COLOR_PAIR(3));
            wprintw(fm.content_win.getwin(), "Empty");
            wattroff(fm.content_win.getwin(), COLOR_PAIR(3));
         }
         else
         {
            string line{};
            while (std::getline(file, line))
               wprintw(fm.content_win.getwin(), "%s\n", line.c_str());
         }
      }

      refresh();
      fm.refresh_fm();

      const int input = getch();
      if (input == 'q')
         break;
      else if (input == ':')
      {
         // Erase anything that might be in the cmd_win
         werase(fm.cmd_win.getwin());
         // Display the prompt
         wprintw(fm.cmd_win.getwin(), ": ");
         wrefresh(fm.cmd_win.getwin());
         // Get input
         std::string cmd_str = cmd_loop(fm.cmd_win.getwin());
         // Execute
         Command cmd = exec_cmd(cmd_str, dir_files[size_t(selected)].name);
         fm.erase_fm();
         // Display err if necessary
         if (cmd == Command::Quit)
            break;
         else if (cmd == Command::MissingArgs)
            fm.display_err("Missing arguments");
         else if (cmd == Command::PermissionDenied)
            fm.display_err("Permission Denied");
         else if (cmd == Command::Unknown)
            fm.display_err("Unknown command");
      }
      else if (input == KEY_RESIZE)
         fm.resize(stdscr);
      // Trying to open when no directory exists in current location
      // separated to reduce nesting
      else if (input == KEY_RIGHT && dir_files.size() == 0)
      {
         fm.erase_fm();
         fm.display_err("No directory to open");
      }
      else if (input == KEY_RIGHT)
      {
         fm.erase_fm();
         if (dir_files[size_t(selected)].type == FileType::Directory)
         {
            try
            {
               fs::current_path(dir_files[size_t(selected)].name);
               selected = 0;
               begin    = 0;
            }
            catch (fs::filesystem_error& e)
            {
               fm.display_err("Error: Permission denied");
            }
         }
         else
         {
            fm.display_err("Error: Not a directory");
         }
      }
      else if (input == KEY_LEFT)
      {
         fm.erase_fm();
         fs::current_path("..");
         selected = 0;
         begin    = 0;
      }
      else if (input == KEY_DOWN)
      {
         fm.erase_fm();
         if (size_t(selected) < dir_files.size() - 1)
         {
            int maxy = getmaxy(fm.cwd_win.getwin());
            ++selected;
            if (selected - begin >= maxy)
               ++begin;
         }
      }
      else if (input == KEY_UP)
      {
         fm.erase_fm();
         if (size_t(selected) > 0)
         {
            --selected;
            if (selected - begin < 0)
               --begin;
         }
      }
      else
      {
         fm.erase_fm();
      }
   }
}

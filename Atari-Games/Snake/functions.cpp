#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <ncurses.h>
#endif

void gotoxy(int x, int y)
{
#ifdef _WIN32
    COORD pos;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (INVALID_HANDLE_VALUE != hConsole)
    {
        pos.X = x;
        pos.Y = y;
        SetConsoleCursorPosition(hConsole, pos);
    }
#else
    move(y, x);
#endif
}

void set_color(int number)
{
#ifdef _WIN32
    HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(color, number);
#else
    attron(COLOR_PAIR(number));
#endif
}

int main()
{
    int x = 10, y = 10;
    int color = 1;

#ifdef _WIN32
    system("cls");
#else
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
#endif

    gotoxy(x, y);
    set_color(color);
    printf("Hello, World!");

#ifdef _WIN32
    getch();
#else
    getch();
    endwin();
#endif

    return 0;
}
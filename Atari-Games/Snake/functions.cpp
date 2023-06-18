#include <conio.h>
#include <windows.h>

void gotoxy(int x, int y)
{
    COORD pos;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (INVALID_HANDLE_VALUE != hConsole)
    {
        pos.X = x;
        pos.Y = y;
        SetConsoleCursorPosition(hConsole, pos);
    }
}

void set_color(int number)
{
    HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(color, number);
}

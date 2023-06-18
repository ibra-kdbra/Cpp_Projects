#include <iostream>
#include "snake.h"

#define body1 (char)2
#define body2 (char)4

using namespace std;

Snake::Snake()
{
    length = 5;
    for (int i = 0; i < length; i++)
    {
        pos[i].x = i + 40;
        pos[i].y = 12;
    }
}

void Snake::print()
{
    set_color(11);
    gotoxy(pos[0].x, pos[0].y);
    cout << body1;

    for (int i = 0; i < length; i++)
    {
        if (i % 2 == 0)
            set_color(13);
        else
            set_color(11);
        gotoxy(pre_pos[i].x, pre_pos[i].y);
        cout << body2;
    }
    gotoxy(pre_pos[length - 1].x, pre_pos[length - 1].y);
    cout << " ";
}

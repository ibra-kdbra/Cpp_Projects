#include <iostream>
#include "food.h"

using namespace std;

Food::Food()
{
    pos.x = rand() % 70 + 5;
    pos.y = rand() % 20 + 3;
}

void Food::print()
{
    set_color(10);
    gotoxy(pos.x, pos.y);
    cout << shape;
    set_color(11);
}

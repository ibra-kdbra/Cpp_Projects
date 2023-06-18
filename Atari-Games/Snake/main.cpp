#include <iostream>
#include <iomanip>
#include <time.h>
#include "functions.cpp"
#include "snake.cpp"
#include "food.cpp"

using namespace std;

void print_wall()
{
	for (int i = 0; i < 79; i++)
	{
		gotoxy(i, 0);
		cout << (char)219;
		gotoxy(i, 24);
		cout << (char)219;
	}
	for (int i = 24; i >= 0; i--)
	{
		gotoxy(0, i);
		cout << (char)219;
		gotoxy(79, i);
		cout << (char)219;
	}
}

void gameover()
{
	set_color(12);
	gotoxy(36, 12);
	cout << "Game Over!";
	_getch();
}

void play()
{
	char ch = 75, ch1 = 75;
	bool sw;

	Snake snake;
	Food food;

	print_wall();
	food.print();

	while (1)
	{
		set_color(13);
		gotoxy(37, 24);
		cout << "Score: " << snake.length - 5;

		if (snake.pos[0].x == food.pos.x && snake.pos[0].y == food.pos.y)
		{
			snake.length++;
			food.print();
		}

		for (int i = 0; i < snake.length; i++)
		{
			snake.pre_pos[i].x = snake.pos[i].x;
			snake.pre_pos[i].y = snake.pos[i].y;
		}

		if (_kbhit())
		{
			ch1 = ch;
			_getch();
			ch = _getch();
			if ((ch == 77 && ch1 == 75) || (ch == 75 && ch1 == 77) || (ch == 72 && ch1 == 80) || (ch == 80 && ch1 == 72))
			{
				ch = ch1;
			}
		}
		if (ch == 77) // right
		{
			snake.pos[0].x++;
		}
		if (ch == 72) // up
		{
			snake.pos[0].y--;
		}
		if (ch == 75) // left
		{
			snake.pos[0].x--;
		}
		if (ch == 80) // down
		{
			snake.pos[0].y++;
		}
		if (snake.pos[0].x == 80 || snake.pos[0].x == 0 || snake.pos[0].y == 25 || snake.pos[0].y == 0)
		{
			gameover();
			return;
		}

		sw = true;
		for (int i = 1; i < snake.length; i++)
		{
			if (snake.pos[0].x == snake.pos[i].x && snake.pos[0].y == snake.pos[i].y)
			{
				sw = false;
			}
		}
		if (sw == false)
		{
			gameover();
			return;
		}

		if (snake.length == 32)
		{
			set_color(10);
			gotoxy(36, 12);
			cout << "You Win!";
			_getch();
			return;
		}

		for (int i = 0; i < snake.length; i++)
		{
			snake.pos[i + 1].x = snake.pre_pos[i].x;
			snake.pos[i + 1].y = snake.pre_pos[i].y;
		}

		snake.print();
		Sleep(100);
	}
}

int main()
{
	srand(time(0));
	system("color 0b");
	play();
}

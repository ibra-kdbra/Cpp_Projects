#include <iostream>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <time.h>

using namespace std;

#define block (char)254
#define wall (char)219

int screen[80][25];
int shape[3][3], next_shape[3][3];
int xs = 37, ys = 0;
int score = -1;
int color;

void menu();
void init();

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

void print_shape()
{
	set_color(color);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			gotoxy(xs + j, ys + i);

			if (shape[i][j] == 1)
				cout << block;
		}
	}
}

void make_shape()
{
	int r;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			shape[i][j] = next_shape[i][j];
		}
	}
	r = rand() % 6;
	switch (r)
	{
	case 0:
		next_shape[0][0] = next_shape[1][0] = next_shape[1][0] = next_shape[1][1] = next_shape[2][0] = 0;
		next_shape[0][2] = next_shape[1][2] = next_shape[2][2] = next_shape[2][1] = 1;
		color = 10;
		break;
	case 1:
		next_shape[0][0] = next_shape[0][1] = next_shape[0][2] = next_shape[1][0] = next_shape[1][2] = 0;
		next_shape[1][1] = next_shape[2][0] = next_shape[2][1] = next_shape[2][2] = 1;
		color = 11;
		break;
	case 2:
		next_shape[0][0] = next_shape[0][1] = next_shape[1][0] = next_shape[1][1] = next_shape[2][0] = next_shape[2][1] = 0;
		next_shape[0][2] = next_shape[1][2] = next_shape[2][2] = 1;
		color = 12;
		break;
	case 3:
		next_shape[0][0] = next_shape[0][1] = next_shape[0][2] = next_shape[1][0] = next_shape[2][0] = 0;
		next_shape[1][1] = next_shape[2][1] = next_shape[1][2] = next_shape[2][2] = 1;
		color = 13;
		break;
	case 4:
		next_shape[0][0] = next_shape[1][0] = next_shape[2][0] = next_shape[2][1] = next_shape[0][2] = 0;
		next_shape[0][1] = next_shape[1][1] = next_shape[1][2] = next_shape[2][2] = 1;
		color = 14;
		break;
	case 5:
		next_shape[0][0] = next_shape[0][1] = next_shape[0][2] = next_shape[2][1] = 0;
		next_shape[1][0] = next_shape[1][1] = next_shape[1][2] = next_shape[2][0] = next_shape[2][2] = 1;
		color = 15;
		break;
	}
}

void rotate()
{
	int temp[3][3];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			temp[i][j] = shape[j][2 - i];
		}
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			shape[i][j] = temp[i][j];
		}
	}
}

void move_shape()
{
	bool swr = true, swl = true;

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (screen[xs + j + 1][ys + i] == 1 && shape[i][j] == 1)
			{
				swr = false;
			}
		}
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (screen[xs + j - 1][ys + i] == 1 && shape[i][j] == 1)
			{
				swl = false;
			}
		}
	}

	if (GetAsyncKeyState(VK_RIGHT) && swr == true)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (shape[i][j] == 1)
				{
					gotoxy(xs + j, ys + i);
					cout << " ";
				}
			}
		}
		xs++;
		print_shape();
	}
	else if (GetAsyncKeyState(VK_LEFT) && swl == true)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (shape[i][j] == 1)
				{
					gotoxy(xs + j, ys + i);
					cout << " ";
				}
			}
		}
		xs--;
		print_shape();
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (shape[i][j] == 1)
				{
					gotoxy(xs + j, ys + i);
					cout << " ";
				}
			}
		}
		rotate();
		print_shape();
	}
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (shape[i][j] == 1)
				{
					gotoxy(xs + j, ys + i);
					cout << " ";
				}
			}
		}
		menu();
	}
}

void print_wall()
{
	set_color(11);
	for (int i = 0; i < 79; i++)
	{
		gotoxy(i, 0);
		cout << wall;
		gotoxy(i, 23);
		cout << wall;
		gotoxy(i, 24);
		cout << wall;
	}
	for (int i = 24; i >= 0; i--)
	{
		gotoxy(0, i);
		cout << wall;
		gotoxy(25, i);
		cout << wall;
		gotoxy(50, i);
		cout << wall;
		gotoxy(79, i);
		cout << wall;
	}
}

void show_score_and_next_shape()
{
	set_color(15);
	gotoxy(9, 15);
	cout << "Score: " << score;
	gotoxy(5, 18);
	cout << "Press space to rotate";
	gotoxy(8, 5);
	cout << "Next:";
	for (int i = 0; i < 3; i++)
	{
		gotoxy(11, 7 + i);
		cout << "   ";
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			gotoxy(11 + j, 7 + i);

			if (next_shape[i][j] == 1)
				cout << block;
		}
	}
}

void play()
{
	bool sw = true;
	long r = 0;

	xs = 37;
	ys = 1;
	score++;

	make_shape();
	show_score_and_next_shape();

	while (1)
	{
		sw = true;

		move_shape();
		r++;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (screen[xs + j][ys + i + 1] == 1 && shape[i][j] == 1)
				{
					sw = false;
				}
			}
		}
		if (r % 2 == 0)
		{
			if (sw == true)
			{
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (shape[i][j] == 1)
						{
							gotoxy(xs + j, ys + i);
							cout << " ";
						}
					}
				}
				ys++;
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						if (shape[i][j] == 1)
							screen[xs + j][ys + i] = 1;
					}
				}
				play();
			}
		}
		for (int i = 26; i < 50; i++)
		{
			if (screen[i][2] == 1)
			{
				set_color(12);
				gotoxy(32, 12);
				cout << "Game Over!";
				Sleep(2000);
				_getch();
				init();
			}
		}

		print_shape();
		Sleep(200);
	}
}

void clear_half_screen()
{
	for (int i = 4; i < 20; i++)
	{
		gotoxy(51, i);
		cout << "                           ";
	}
}

void about()
{
	clear_half_screen();

	gotoxy(54, 10);
	cout << "TETRIS v1.1";
	gotoxy(54, 12);
	cout << "Programmer: Sajjad Aemmi";

	_getch();
	clear_half_screen();
}

void menu()
{
	char ch;
	int select = 1;

	while (1)
	{
		set_color(10);
		gotoxy(62, 8);
		cout << "TETRIS";

		if (select == 1)
			set_color(11);
		else
			set_color(15);

		gotoxy(62, 12);
		cout << "Play";

		if (select == 2)
			set_color(11);
		else
			set_color(15);

		gotoxy(62, 14);
		cout << "About";

		if (select == 3)
			set_color(11);
		else
			set_color(15);

		gotoxy(62, 16);
		cout << "Exit";

		ch = _getch();
		switch (ch)
		{
		case 72:
			select--;
			if (select == 0)
				select = 3;
			break;
		case 80:
			select++;
			if (select == 4)
				select = 1;
			break;
		case 13:
			if (select == 1)
				play();
			if (select == 2)
				about();
			if (select == 3)
				exit(0);
		}
	}
}

void init()
{
	system("cls");

	for (int i = 25; i < 50; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			screen[i][j] = 0;
		}
	}
	for (int i = 25; i < 50; i++)
	{
		screen[i][24] = 1;
		screen[i][23] = 1;
	}
	for (int i = 0; i < 25; i++)
	{
		screen[25][i] = 1;
		screen[50][i] = 1;
	}
	for (int i = 1; i < 25; i++)
	{
		gotoxy(26, i);
		cout << "                        ";
	}

	print_wall();
	make_shape();
	srand(time(0));
	menu();
}

int main()
{
	system("color 0b");
	init();
}

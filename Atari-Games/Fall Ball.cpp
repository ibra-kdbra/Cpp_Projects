
#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<iomanip>
#include<fstream>
#include<time.h>

using namespace std;
using std::cin;
using std::cout;

#define ball (char)15
#define wall (char)219
#define sheet (char)223

int screen[80][25];
int xb = 20, yb = 1, ys[4] = { 5, 11, 17, 23 };
int score = 0;
int r[4];

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

///////////////////////////////////////
void color(int number)
{
	HANDLE color;
	color = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(color, number);
}

///////////////////////////////////////
void move_ball()
{
	if (GetAsyncKeyState(VK_RIGHT) && xb + 2 != 40)
	{
		gotoxy(xb, yb);
		cout << " ";
		xb += 2;
	}
	else if (GetAsyncKeyState(VK_LEFT) && xb - 2 != 0)
	{
		gotoxy(xb, yb);
		cout << " ";
		xb -= 2;
	}
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		menu();
	}
}

void print_sheet()
{
	color(10);
	for (int i = 0; i < 4; i++)
	{
		for (int j = 1; j < 39; j++)
		{
			screen[j][ys[i]] = 0;
		}
		gotoxy(1, ys[i]);
		cout << "                                      ";
	}

	for (int i = 0; i < 4; i++)
	{
		ys[i]--;
	}

	if (ys[0] == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			ys[i] = ys[i + 1];
			r[i] = r[i + 1];
		}
		r[3] = rand() % 34 + 1;
		ys[3] = 23;
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 1; j < 39; j++)
		{
			screen[j][ys[i]] = 1;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = r[i]; j < r[i] + 4; j++)
		{
			screen[j][ys[i]] = 0;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		for (int j = 1; j < 39; j++)
		{
			if (screen[j][ys[i]] == 1)
			{
				gotoxy(j, ys[i]);
				cout << sheet;
			}
		}
	}
}

///////////////////////////////////////
void print_wall()
{
	color(11);
	for (int i = 0; i < 79; i++)
	{
		gotoxy(i, 0);
		cout << wall;
		gotoxy(i, 24);
		cout << wall;
	}
	for (int i = 24; i >= 0; i--)
	{
		gotoxy(0, i);
		cout << wall;
		gotoxy(39, i);
		cout << wall;
		gotoxy(79, i);
		cout << wall;
	}
}

///////////////////////////////////////
void play()
{
	long q = 0;
	xb = 20;
	yb = 16;
	for (int i = 0; i < 4; i++)
	{
		r[i] = rand() % 34 + 1;
	}
	while (1)
	{
		gotoxy(61, 18);
		cout << "   ";
		gotoxy(56, 18);
		cout << "Score: " << score;

		move_ball();
		if (q % 2 == 0)
		{
			print_sheet();
		}
		color(15);
		if (screen[xb][yb] == 1)
		{
			yb--;
		}
		if (yb == 0)
		{
			color(12);
			gotoxy(17, 12);
			cout << "Game Over!";
			color(15);
			gotoxy(14, 14);
			cout << "Your Score Is: " << score;
			Sleep(3000);
			_getch();
			init();
		}
		if (yb + 1 != 24 && screen[xb][yb + 1] == 0)
		{
			if (screen[xb][yb + 2] == 1)
			{
				score++;
			}
			gotoxy(xb, yb);
			cout << " ";
			yb++;
			gotoxy(xb, yb);
			cout << ball;
		}
		gotoxy(xb, yb);
		cout << ball;
		q++;
		if (q == 10000)
			q = 0;
		Sleep(40);
	}
}

///////////////////////////////////////
void claerhalfscreen()
{
	for (int i = 4; i < 20; i++)
	{
		gotoxy(46, i);
		cout << "                             ";
	}
}

///////////////////////////////////////
void about()
{
	claerhalfscreen();

	gotoxy(47, 10);
	cout << "Fall Ball v 1.0";
	gotoxy(47, 12);
	cout << "Programmer: Sajjad Aemmi";
	gotoxy(47, 14);
	cout << "All Rights Reserved!";

	_getch();
	claerhalfscreen();
}

///////////////////////////////////////
void menu()
{
	char ch;
	int select = 1;

	print_wall();

	while (1)
	{
		color(10);
		gotoxy(56, 6);
		cout << "Fall Ball";

		if (select == 1)
			color(11);
		else
			color(15);

		gotoxy(58, 10);
		cout << "Play";

		if (select == 2)
			color(11);
		else
			color(15);

		gotoxy(58, 12);
		cout << "About";

		if (select == 3)
			color(11);
		else
			color(15);

		gotoxy(58, 14);
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

///////////////////////////////////////
void init()
{
	system("cls");

	print_wall();

	for (int i = 0; i < 39; i++)
	{
		screen[i][24] = 1;
	}
	score = 0;
	srand(time(0));
	menu();
}

///////////////////////////////////////
void main()
{
	system("color 0b");

	init();
}
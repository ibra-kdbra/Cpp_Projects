//Sajjad Aemmi

#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<iomanip>
#include<fstream>
#include<time.h>

using namespace std;
using std::cin;
using std::cout;

#define bird (char)2
#define wall (char)219
#define sheet (char)176

int screen[80][25];
int score = 0;
int xb, yb, xs[2] = { 18, 38 };
int r[2];

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
void print_sheet()
{
	color(10);
	for (int i = 0; i < 2; i++)
	{
		for (int j = 1; j < 24; j++)
		{
			screen[xs[i]][j] = 0;
			gotoxy(xs[i], j);
			cout << " ";
		}
	}

	for (int i = 0; i < 2; i++)
	{
		xs[i]--;
	}

	if (xs[0] == 0)
	{
		for (int i = 0; i < 2; i++)
		{
			xs[i] = xs[i + 1];
			r[i] = r[i + 1];
		}
		r[1] = rand() % 10 + 5;
		xs[1] = 38;
	}

	for (int i = 0; i < 2; i++)
	{
		for (int j = 1; j < 24; j++)
		{
			screen[xs[i]][j] = 1;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = r[i]; j < r[i] + 7; j++)
		{
			screen[xs[i]][j] = 0;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 1; j < 24; j++)
		{
			if (screen[xs[i]][j] == 1)
			{
				gotoxy(xs[i], j);
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
	int upc = 2, frontc = 2;
	long q = 0;
	bool sw = true;

	for (int i = 0; i < 2; i++)
	{
		r[i] = rand() % 10 + 5;
	}

	while (1)
	{
		sw = true;
		if (q % 2 == 0)
		{
			print_sheet();
			if (screen[18][2] == 1)
			{
				score++;
				Beep(880, 70);
				sw = false;
			}
		}
		color(11);
		if (GetAsyncKeyState(VK_SPACE))
		{
			upc = 0;
			frontc = 0;
		}
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			menu();
		}
		if (upc < 3)
		{
			yb--;
			gotoxy(xb, yb);
			cout << bird;
			upc++;
		}
		else if (frontc < 4)
		{
			gotoxy(xb, yb);
			cout << bird;
			frontc++;
		}
		else if (yb < 23)
		{
			yb++;
			gotoxy(xb, yb);
			cout << bird;
		}
		if (screen[xb][yb] == 1 || yb == 23)
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

		gotoxy(17, 0);
		cout << "Score: " << score;
		q++;
		if (q == 10000)
			q = 0;
		if (sw == true)
			Sleep(30);
		gotoxy(xb, yb);
		cout << " ";
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
	cout << "Flappy Bird v 1.0";
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
		gotoxy(55, 6);
		cout << "Flappy Bird";

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
	xb = 20;
	yb = 18;
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
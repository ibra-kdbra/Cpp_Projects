
#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<iomanip>
#include<fstream>
#include<time.h>

using namespace std;
using std::cin;
using std::cout;

#define rocket (char)186
#define rocket_nok (char)186
#define rocket_uc (char)202
#define rocket_ur (char)187
#define rocket_ul (char)201
#define rocket_dc (char)205
#define rocket_dr (char)206
#define tir (char)30
#define enemy (char)254

int a[40][25];
int score = 0;
int xr = 19;
int tir_count = 0;

struct post
{
	int x;
	int y;
};
post tir_post[50];

void init();
void menu();

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
		gotoxy(40, i);
		cout << (char)219;
		gotoxy(79, i);
		cout << (char)219;
	}
}

///////////////////////////////////////
void print_rocket()
{
	color(15);

	gotoxy(xr, 23);
	cout << rocket_dr;
	gotoxy(xr + 1, 23);
	cout << rocket_dc;
	gotoxy(xr + 2, 23);
	cout << rocket_dc;
	gotoxy(xr + 3, 23);
	cout << rocket_dc;
	gotoxy(xr + 4, 23);
	cout << rocket_dr;
	gotoxy(xr, 22);
	cout << rocket_ul;
	gotoxy(xr + 1, 22);
	cout << rocket_dc;
	gotoxy(xr + 2, 22);
	cout << rocket_uc;
	gotoxy(xr + 3, 22);
	cout << rocket_dc;
	gotoxy(xr + 4, 22);
	cout << rocket_ur;

	color(11);
}

///////////////////////////////////////
void move_rocket()
{
	if (GetAsyncKeyState(VK_RIGHT) && xr + 4 != 39)
	{
		gotoxy(xr, 23);
		cout << "     ";
		gotoxy(xr, 22);
		cout << "     ";
		xr++;
		print_rocket();
	}
	else if (GetAsyncKeyState(VK_LEFT) && xr - 1 != 0)
	{
		gotoxy(xr, 23);
		cout << "     ";
		gotoxy(xr, 22);
		cout << "     ";
		xr--;
		print_rocket();
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		tir_post[tir_count].x = xr + 2;
		tir_post[tir_count].y = 21;
		gotoxy(tir_post[tir_count].x, tir_post[tir_count].y);
		cout << tir;
		tir_count++;
	}
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		menu();
	}
}

///////////////////////////////////////
void print_enemy()
{
	int x, y = 0;

	color(11);
	x = (rand() % 70 + 3);

	if (x >= 3 && x <= 37)
	{

		for (int j = 1; j < 25; j++)
		{
			if (a[x][j] == 0)
			{
				y = j;
				a[x][y] = 1;
				break;
			}
		}
		gotoxy(x, y);
		cout << enemy;
	}
	Sleep(60);
}

///////////////////////////////////////
void print_tir()
{
	color(14);
	for (int i = 0; i < tir_count; i++)
	{
		if (tir_post[i].y != 1)
		{
			gotoxy(tir_post[i].x, tir_post[i].y);
			cout << " ";
			tir_post[i].y--;
			gotoxy(tir_post[i].x, tir_post[i].y);
			cout << tir;
		}
		if (tir_post[i].y == 1)
		{
			gotoxy(tir_post[i].x, tir_post[i].y);
			cout << " ";

			for (int j = i; j < tir_count; j++)
			{
				tir_post[j] = tir_post[j + 1];
			}
			tir_count--;
		}
	}
	color(11);
}

///////////////////////////////////////
void play()
{
	print_wall();
	print_rocket();

	while (1)
	{
		move_rocket();
		print_enemy();
		print_tir();
		gotoxy(58, 18);
		cout << "Score: " << score;

		for (int i = 0; i < 40; i++)
		{
			for (int j = 0; j < 25; j++)
			{
				for (int k = 0; k < tir_count; k++)
				{
					if (a[i][j] == 1 && i == tir_post[k].x && j == tir_post[k].y)
					{
						a[i][j] = 0;
						gotoxy(i, j);
						cout << " ";
						for (int w = k; w < tir_count; w++)
						{
							tir_post[w] = tir_post[w + 1];
						}
						tir_count--;
						score++;
					}
				}
			}
		}
		for (int i = 0; i < 40; i++)
		{
			for (int j = 22; j < 25; j++)
			{
				if (a[i][j] == 1)
				{
					color(12);
					gotoxy(36, 12);
					cout << "Game Over!";
					Sleep(2000);
					_getch();
					init();
				}
			}
		}
	}
}

///////////////////////////////////////
void claerhalfscreen()
{
	for (int i = 4; i < 20; i++)
	{
		gotoxy(50, i);
		cout << "                         ";
	}
}
///////////////////////////////////////
void about()
{
	claerhalfscreen();

	gotoxy(50, 8);
	cout << "SILVER SPACECRAFT v 1.0";
	gotoxy(50, 10);
	cout << "Programmer: Sajjad Aemmi";
	gotoxy(50, 12);
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
		gotoxy(52, 4);
		cout << "SILVER SPACECRAFT";

		if (select == 1)
			color(11);
		else
			color(15);

		gotoxy(58, 8);
		cout << "Play";

		if (select == 2)
			color(11);
		else
			color(15);

		gotoxy(58, 10);
		cout << "About";

		if (select == 3)
			color(11);
		else
			color(15);

		gotoxy(58, 12);
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

	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			a[i][j] = 0;
		}
	}
	score = 0;
	xr = 19;
	tir_count = 0;

	for (int i = 0; i < 50; i++)
	{
		tir_post[i].x = 0;
		tir_post[i].y = 0;
	}

	srand(time(0));
	menu();
}

///////////////////////////////////////
void main()
{
	system("color 0b");

	init();
}
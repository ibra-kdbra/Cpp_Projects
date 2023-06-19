
#include<iostream>
#include<conio.h>
#include<Windows.h>
#include<iomanip>
#include<fstream>
#include<time.h>

using namespace std;

#define ball '*'
#define rocket (char)186

int sleep1 = 40;
int bar = 6;
int yr = 12;

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

void harkate_rocket()
{
	if (GetAsyncKeyState(VK_DOWN) && yr + 4 != 25)
	{
		gotoxy(5, yr);
		cout << " ";
		gotoxy(5, yr + 5);
		cout << rocket;
		yr++;
		Sleep(10);
	}
	else if (GetAsyncKeyState(VK_UP) && yr - 1 != -1)
	{
		gotoxy(5, yr + 4);
		cout << " ";
		gotoxy(5, yr - 1);
		cout << rocket;
		yr--;
		Sleep(10);
	}
}
///////////////////////////////////////
void play()
{
	int nw = 0, ne = 1, sw = 0, se = 0;
	int x = 40, y = 12;
	char ch;

	color(11);
	for (int i = yr; i < yr + 5; i++)
	{
		gotoxy(5, i);
		cout << rocket;
	}
	while (1)
	{
		if (_kbhit())
		{
			ch = _getch();
			if (ch == 27)
				return;
		}

		harkate_rocket();

		if (x == 0)
		{
			color(12);
			gotoxy(36, 12);
			cout << "Game Over!";
			Sleep(1000);
			ch = _getch();
			return;
		}
		if (x == 6 && (y == yr || y == yr + 1 || y == yr + 2 || y == yr + 3 || y == yr + 4))
		{
			if (nw == 1)
			{
				nw = 0;
				ne = 1;
			}
			if (sw == 1)
			{
				sw = 0;
				se = 1;
			}
		}
		if (y == 0)
		{
			if (ne == 1)
			{
				ne = 0;
				se = 1;
			}
			if (nw == 1)
			{
				nw = 0;
				sw = 1;
			}
		}
		if (y == 24)
		{
			if (se == 1)
			{
				ne = 1;
				se = 0;
			}
			if (sw == 1)
			{
				nw = 1;
				sw = 0;
			}
		}
		if (x == 79)
		{
			if (ne == 1)
			{
				ne = 0;
				nw = 1;
			}
			if (se == 1)
			{
				se = 0;
				sw = 1;
			}
		}
		gotoxy(x, y);
		cout << " ";
		if (ne == 1)
		{
			x++;
			y--;
		}
		if (se == 1)
		{
			x++;
			y++;
		}
		if (nw == 1)
		{
			x--;
			y--;
		}
		if (sw == 1)
		{
			x--;
			y++;
		}

		gotoxy(x, y);
		cout << ball;

		Sleep(sleep1);
	}
}

///////////////////////////////////////
void option()
{
	char ch;

	gotoxy(28, 12);
	cout << "Speed:";
	gotoxy(35, 11);
	cout << (char)218;
	gotoxy(35, 13);
	cout << (char)192;
	gotoxy(47, 11);
	cout << (char)191;
	gotoxy(47, 13);
	cout << (char)217;
	gotoxy(35, 12);
	cout << (char)179;
	gotoxy(47, 12);
	cout << (char)179;
	gotoxy(36, 11);
	for (int i = 0; i <= 10; i++)
	{
		cout << (char)196;
	}
	gotoxy(36, 13);
	for (int i = 0; i <= 10; i++)
	{
		cout << (char)196;
	}
	while (1)
	{
		gotoxy(36, 12);
		for (int i = 0; i <= 10; i++)
		{
			cout << " ";
		}
		gotoxy(36, 12);
		for (int i = 0; i < bar; i++)
		{
			cout << (char)219;
		}

		ch = _getch();
		switch (ch)
		{
		case 77:
			sleep1 -= 10;

			if (sleep1 == 0)
				sleep1 = 10;
			bar++;
			if (bar == 12)
				bar = 11;
			break;
		case 75:
			sleep1 += 10;
			if (sleep1 == 110)
				sleep1 = 100;
			bar--;
			if (bar == 0)
				bar = 1;
			break;
		case 13:
			system("cls");
			return;
		}
	}
}
///////////////////////////////////////
void about()
{
	gotoxy(30, 8);
	cout << "Squash v 1.01";
	gotoxy(30, 10);
	cout << "Programmer: Sajjad Aemmi";
	gotoxy(30, 12);
	cout << "All Rights Reserved!";
	_getch();
}

///////////////////////////////////////
void menu()
{
	char ch;
	int select = 1;

	color(10);
	gotoxy(36, 4);
	cout << "SQUASH";
	while (1)
	{
		if (select == 1)
			color(11);
		else
			color(15);

		gotoxy(36, 8);
		cout << "Start";

		if (select == 2)
			color(11);
		else
			color(15);

		gotoxy(36, 10);
		cout << "Option";

		if (select == 3)
			color(11);
		else
			color(15);

		gotoxy(36, 12);
		cout << "About";

		if (select == 4)
			color(11);
		else
			color(15);

		gotoxy(36, 14);
		cout << "Exit";

		ch = _getch();
		switch (ch)
		{
		case 72:
			select--;
			if (select == 0)
				select = 4;
			break;
		case 80:
			select++;
			if (select == 5)
				select = 1;
			break;
		case 13:
			system("cls");
			if (select == 1)
				play();
			if (select == 2)
				option();
			if (select == 3)
				about();
			if (select == 4)
				exit(0);
			system("cls");
		}
	}
}

///////////////////////////////////////
void main()
{
	menu();
}
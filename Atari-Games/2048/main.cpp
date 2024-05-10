/*
This was made by ibra-kdbra 
https://github.com/ibra-kdbra
I'm trying to build this with all programming languages I konw.
*/

#include <iostream>
#include <conio.h>
#include <windows.h>
#include <iomanip>
#include <time.h>

using namespace std;

int a[4][4];
int score = 0;
char ch = 'w';

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

void show()
{
	system("cls");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (a[i][j] == 2)
				set_color(10);
			else if (a[i][j] == 4)
				set_color(14);
			else if (a[i][j] == 8)
				set_color(12);
			else if (a[i][j] == 16)
				set_color(13);
			else if (a[i][j] == 32)
				set_color(11);
			else if (a[i][j] == 64)
				set_color(07);
			else if (a[i][j] == 128)
				set_color(06);
			else if (a[i][j] == 256)
				set_color(10);
			else if (a[i][j] == 512)
				set_color(14);
			else if (a[i][j] == 1024)
				set_color(12);
			else if (a[i][j] == 2048)
				set_color(13);
			else
				set_color(15);
			cout << a[i][j];
			cout << "\t";
		}
		cout << "\n\n\n";
	}
	set_color(15);
	gotoxy(36, 4);
	cout << "Score:" << score;
	gotoxy(0, 0);
}

bool move()
{
	bool sw = true;

	_getch();
	ch = _getch();

	for (int count = 0; count < 4; count++)
	{
		if (ch == 72) // up
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (a[i][j] != 0 && a[i][j] == a[i - 1][j] && a[i][j] == a[i - 2][j] && a[i][j] != a[i - 3][j] && i - 1 != -1 && i - 2 != -1 && i - 3 != -1)
					{
						a[i - 2][j] *= 2;
						a[i][j] = 0;
						score += a[i - 2][j];
						sw = false;
					}
					if (a[i][j] != 0 && a[i - 1][j] == 0 && i - 1 != -1)
					{
						a[i - 1][j] = a[i][j];
						a[i][j] = 0;
						sw = false;
					}
					if (a[i][j] == a[i - 1][j] && a[i][j] != 0 && i - 1 != -1)
					{
						a[i - 1][j] *= 2;
						a[i][j] = 0;
						score += a[i - 1][j];
						sw = false;
					}
				}
			}
		}
		else if (ch == 80) // down
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (a[i][j] != 0 && a[i][j] == a[i + 1][j] && a[i][j] == a[i + 2][j] && a[i][j] != a[i + 3][j] && i + 1 != 4 && i + 2 != 4 && i + 3 != 4)
					{
						a[i + 2][j] *= 2;
						a[i][j] = 0;
						score += a[i + 2][j];
						sw = false;
					}
					if (a[i][j] != 0 && a[i + 1][j] == 0 && i + 1 != 4)
					{
						a[i + 1][j] = a[i][j];
						a[i][j] = 0;
						sw = false;
					}
					if (a[i][j] == a[i + 1][j] && a[i][j] != 0 && i + 1 != 4)
					{
						a[i + 1][j] *= 2;
						a[i][j] = 0;
						score += a[i + 1][j];
						sw = false;
					}
				}
			}
		}
		else if (ch == 75) // left
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (a[i][j] != 0 && a[i][j] == a[i][j - 1] && a[i][j] == a[i][j - 2] && a[i][j] != a[i][j - 3] && j - 1 != -1 && j - 2 != -1 && j - 3 != -1)
					{
						a[i][j - 2] *= 2;
						a[i][j] = 0;
						score += a[i][j - 2];
						sw = false;
					}
					if (a[i][j] != 0 && a[i][j - 1] == 0 && j - 1 != -1)
					{
						a[i][j - 1] = a[i][j];
						a[i][j] = 0;
						sw = false;
					}
					if (a[i][j] == a[i][j - 1] && a[i][j] != 0 && j - 1 != -1)
					{
						a[i][j - 1] *= 2;
						a[i][j] = 0;
						score += a[i][j - 1];
						sw = false;
					}
				}
			}
		}
		else if (ch == 77) // right
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (a[i][j] != 0 && a[i][j] == a[i][j + 1] && a[i][j] == a[i][j + 2] && a[i][j] != a[i][j + 3] && j + 1 != 4 && j + 2 != 4 && j + 3 != 4)
					{
						a[i][j + 2] *= 2;
						a[i][j] = 0;
						score += a[i][j + 2];
						sw = false;
					}
					if (a[i][j] != 0 && a[i][j + 1] == 0 && j + 1 != 4)
					{
						a[i][j + 1] = a[i][j];
						a[i][j] = 0;
						sw = false;
					}
					if (a[i][j] == a[i][j + 1] && a[i][j] != 0 && j + 1 != 4)
					{
						a[i][j + 1] *= 2;
						a[i][j] = 0;
						score += a[i][j + 1];
						sw = false;
					}
				}
			}
		}
	}
	if (sw == true)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (a[i][j] == 0)
					return false;
			}
		}

		gotoxy(36, 12);
		cout << "Game Over!";
		_getch();
		exit(0);
	}
	return true;
}

void play()
{
	int x, y, z;
	bool sw = true, sw1 = true;

	srand(time(0));

	while (1)
	{
		show();
		sw1 = move();

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (a[i][j] == 2048)
				{
					gotoxy(36, 6);
					set_color(10);
					cout << "You Win!";
					gotoxy(0, 0);
				}
			}
		}
		sw = true;
		while (sw == true)
		{
			x = rand() % 4;
			y = rand() % 4;
			if (a[x][y] == 0 && ((ch == 72 && x == 3) || (ch == 80 && x == 0) || (ch == 75 && y == 3) || (ch == 77 && y == 0)))
				sw = false;
		}
		if (sw1 == true)
		{
			z = rand() % 3;
			if (z == 0)
				a[x][y] = 4;
			if (z == 1 || z == 2)
				a[x][y] = 2;
		}
	}
}

void about()
{
	gotoxy(30, 8);
	cout << "2048 v1.5";
	gotoxy(30, 10);
	cout << "Programmer: ibra-kdbra";

	_getch();
}

void menu()
{
	char ch;
	int select = 1;

	set_color(10);
	gotoxy(36, 4);
	cout << "2048";
	while (1)
	{
		if (select == 1)
			set_color(11);
		else
			set_color(15);

		gotoxy(36, 8);
		cout << "Play";

		if (select == 2)
			set_color(11);
		else
			set_color(15);

		gotoxy(36, 10);
		cout << "About";

		if (select == 3)
			set_color(11);
		else
			set_color(15);

		gotoxy(36, 12);
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
			system("cls");
			if (select == 1)
				play();
			if (select == 2)
				about();
			if (select == 3)
				exit(0);
			system("cls");
		}
	}
}

void init()
{
	bool sw;
	int x, y, z;

	srand(time(0));

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			a[i][j] = 0;
		}
	}
	for (int count = 0; count < 2; count++)
	{
		sw = true;
		while (sw == true)
		{
			x = rand() % 4;
			y = rand() % 4;
			if (a[x][y] == 0 && ((ch == 'w' && x == 3) || (ch == 's' && x == 0) || (ch == 'a' && y == 3) || (ch == 'd' && y == 0)))
				sw = false;
		}

		z = rand() % 3;
		if (z == 0)
			a[x][y] = 4;
		if (z == 1 || z == 2)
			a[x][y] = 2;
	}
}

int main()
{
	init();
	menu();
}

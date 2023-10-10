void print_range()
{
	int i, j;
	for (i = 1; i <= 50; i++)
	{
		for (j = 1; j <= 59; j++)
		{
			if (i == 1 || i == 50)
			{
				if (j != 1)
					printf("-");
				else
					printf("  ");

			}
			else
			{
				if (j == 1 || j == 59)
					printf("∥");
				else
					printf(" ");
			}
		}
		printf("\n");
	}
}
void weapon1(int i)
{
	if (i == 0)
		printf("   。");
	else if (i == 1)
		printf("   ||");
	else if (i == 2)
		printf("  ||||");
	else if (i == 3)
		printf("  ●●");
	else if (i == 4)
		printf("|||●|||");
	else
		printf("  *◎*");
}
void weapon2(int i)
{
	if (i == 0)
		printf("   ↑");
	else if (i == 1)
		printf("   ||");
	else if (i == 2)
		printf("。|｜|。");
	else if (i == 3)
		printf("↑|█|↑");
	else if (i == 4)
		printf("|▅●▅|");
	else
		printf(" ︻┴︻");
}
void weapon3(int i)
{
	if (i == 0)
		printf(" 。。。");
	else if (i == 1)
		printf(" ▉▉▉");
	else if (i == 2)
		printf(" ▉▉▉");
	else if (i == 3)
		printf("|▅●▅|");
	else if (i == 4)
		printf(" └▇┘");
	else
		printf(" ︻┴︻");
}
int PRINT_explosion(struct explosion* first_animation, int line_status, int status)
{
	COORD xy = first_animation->pos;
	if (line_status == 3)
	{
		if (status == 2)
		{
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
			printf("  ※  ");
			xy.Y -= 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
			printf("※※※");
			xy.Y -= 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
			printf("  ※  ");
			return 1;
		}
		if (status == 1)
		{
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
			printf("      ");
			xy.Y -= 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
			printf("  ※  ");
			xy.Y -= 1;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
			printf("      ");
			return 2;
		}
		if (status == 0)
		{
			return 2;
		}
		xy = first_animation->pos;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("╰※╯");
		xy.Y -= 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("※※※");
		xy.Y -= 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("╭※╮");
		xy.Y -= 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("      ");
		return 1;
	}
	else if (line_status == 2)
	{
		xy = first_animation->pos;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("      ");
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("  ※  ");
		xy.Y -= 1;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("      ");
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("※※※");
		return 1;
	}
	else if (line_status == 1)
	{
		xy = first_animation->pos;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("      ");
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
		printf("  ※  ");
		return 1;
	}
	else
		return 0;

}

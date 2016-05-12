#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void MakeAGuess(int &guess)
{
	printf("Make a guess 0-9>");
	char str[256];
	fgets(str,255,stdin);
	guess=atoi(str);
}


int main(void)
{
	srand((int)time(nullptr));

	const int r=rand()%10;


	for(;;)
	{
		int guess;
		MakeAGuess(guess);

		if(guess==r)
		{
			printf("Correct!\n");
			break;
		}
		else
		{
			if(guess<r)
			{
				printf("Too low\n");
			}
			else
			{
				printf("Too high\n");
			}
		}
	}

	return 0;
}


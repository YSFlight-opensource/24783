#include <stdio.h>

int CountWord(const char str[])
{
	int state=0,wordCount=0;
	for(int i=0; str[i]!=0; ++i)
	{
		switch(state)
		{
		case 0:
			if(' '!=str[i] && '\t'!=str[i])
			{
				state=1;
				++wordCount;
			}
			break;
		case 1:
			if(' '==str[i] || '\t'==str[i])
			{
				state=0;
			}
			break;
		}
	}
	return wordCount;
}

int main(void)
{
	printf(">");
	char str[256];
	fgets(str,255,stdin);
	printf("%d words\n",CountWord(str));
	return 0;
}

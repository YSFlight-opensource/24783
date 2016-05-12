#include <stdio.h>

int main(void)
{
	printf("Enter string>");

	char str[256];
	fgets(str,255,stdin);

	printf("You entered %s\n",str);

	return 0;
}


#include <stdio.h>


void PrintDump(const char fn[])
{
	FILE *fp=fopen(fn,"rb");
	if(nullptr!=fp)
	{
		long long int offset=0,readSize;
		unsigned char buf[256];
		while(0<(readSize=fread(buf,1,256,fp)))
		{
			for(int i=0; i<256; i+=16)
			{
				printf("%08llx|",offset+i);
				for(int j=0; j<16; ++j)
				{
					if(readSize<=i+j)
					{
						break;
					}
					printf(" %02x",buf[i+j]);
				}
				printf("\n");
			}
			printf("\n");
			offset+=256;
		}
		fclose(fp);
	}
	else
	{
		printf("Cannot open file.\n");
	}
}

int main(int argc,char *argv[])
{
	if(2==argc)
	{
		PrintDump(argv[1]);
	}
	else
	{
		printf("Usage: dump.exe <filename>\n");
		return 0;
	}
}

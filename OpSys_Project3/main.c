#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "parser_help.h"


int main(int argc, char **argv)
{
	struct BPB b;
	int fin = 0;

	if(argc == 2)
	{
		// Proper number of arguments (FAT32 and fat32 image file)
		if(access(argv[1], F_OK) != -1)
		{
			// File Exists, parses boot sector
			parseBS(&b, argv[1]);
			while(fin == 0)
			{
				//
				parseInput(&b, argv[1]);
				fin = 1;
			}
		}
		else
		{
			// File Doesnt Exist
			printf("File Does not Exist!\n");
		}
	}
	else
	{
		printf("Check Number of arguments\n");
	}
	return 0;
}

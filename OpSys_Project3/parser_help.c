//COP4610
//Project 1 Starter Code
//example code for initial parsing

//*** if any problems are found with this code,
//*** please report them to the TA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "parser_help.h"


void parseInput(struct BPB* b, char* fileName) {
	char* token = NULL;
	char* temp = NULL;
	int fin = 0;

	instruction instr;
	struct LocationStack loc;
	struct currDir cwd;
	//struct rootData root;
	struct direcEnt direc;
	instr.tokens = NULL;
	instr.numTokens = 0;

	while (fin == 0) {
		printf("Please enter an instruction: ");

		// loop reads character sequences separated by whitespace
		do {
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms", &token);
			temp = (char*)malloc((strlen(token) + 1) * sizeof(char));

			int i;
			int start = 0;
			for (i = 0; i < strlen(token); i++) {
				//pull out special characters and make them into a separate token in the instruction
				if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&') {
					if (i-start > 0) {
						memcpy(temp, token + start, i - start);
						temp[i-start] = '\0';
						addToken(&instr, temp);
					}

					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';

					addToken(&instr,specialChar);

					start = i + 1;
				}
			}

			if (start < strlen(token)) {
				memcpy(temp, token + start, strlen(token) - start);
				temp[i-start] = '\0';
				addToken(&instr, temp);
			}

			//free and reset variables
			free(token);
			free(temp);

			token = NULL;
			temp = NULL;
		} while ('\n' != getchar());    //until end of line is reached

		addNull(&instr);
		//printTokens(&instr);
		if(loc.top == 0)
			getRootDirectory(b, &loc);
		checkTokens(&instr, b, &direc, &loc, fileName, &cwd);
		if(strcmp(instr.tokens[0], "exit") == 0)
			fin = 1;
		clearInstruction(&instr);
	}
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction* instr_ptr, char* tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**) malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**) realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok)+1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
}

void addNull(instruction* instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char**)malloc(sizeof(char*));
	else
		instr_ptr->tokens = (char**)realloc(instr_ptr->tokens, (instr_ptr->numTokens+1) * sizeof(char*));

	instr_ptr->tokens[instr_ptr->numTokens] = (char*) NULL;
	instr_ptr->numTokens++;
}

void printTokens(instruction* instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr_ptr->numTokens; i++) {
		if ((instr_ptr->tokens)[i] != NULL)
			printf("%s\n", (instr_ptr->tokens)[i]);
	}
}

void clearInstruction(instruction* instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);

	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}

// Functions I created

// Prints menu
void printMenu(void)
{
	printf("\nBelow is the list of possible commands:\n");
	printf("\t1. size FILENAME\n");
	printf("\t2. ls DIRNAME\n");
	printf("\t3. cd DIRNAME\n");
	printf("\t4. creat FILENAME\n");
	printf("\t5. mkdir DIRNAME\n");
	printf("\t6. open FILENAME MODE\n");
	printf("\t7. close FILENAME\n");
	printf("\t8. read FILENAME OFFSET SIZE\n");
	printf("\t9. write FILENAME OFFSET SIZE STRING\n");
	printf("\t10. rm FILENAME\n");
	printf("\t11. rmdir DIRNAME\n");
	printf("\t12. info\n");
	printf("\t12. menu\n");
	printf("\t13. exit\n");
}

// Prints Boot Sector Information
void printInfo(struct BPB* b)
{
	printf("Boot Sector Info\n");
	printf("Bytes Per Sector:\t%d\n", b->BPB_BytsPerSec);
	printf("Sectors Per Cluster:\t%d\n", b->BPB_SecPerClus);
	printf("Reserved Sector Count:\t%d\n", b->BPB_RsvdSecCnt);
	printf("Number of FATs:\t\t%d\n", b->BPB_NumFATs);
	printf("Total Sectors:\t\t%d\n", b->BPB_TotSec32);
	printf("FAT Size:\t\t%d\n", b->BPB_FATSz32);
	printf("Root Clusters:\t\t%d\n", b->BPB_RootClus);
}

// Used to check if the user's tokens correlate to real commands
void checkTokens(instruction *instr_ptr, struct BPB* b, struct direcEnt* direc, struct LocationStack* loc, char* fileName, struct currDir* cwd)
{
	if(strcmp(instr_ptr->tokens[0], "size") == 0) // Prints size of given file
		getSize(b, loc, fileName, direc, instr_ptr, cwd);
	else if(strcmp(instr_ptr->tokens[0], "ls") == 0) // Prints contents of listed directory
		getLS(loc, direc, instr_ptr, cwd,  fileName);
	else if(strcmp(instr_ptr->tokens[0], "cd") == 0)
		changeDir(instr_ptr, fileName, loc, direc, cwd);
	else if(strcmp(instr_ptr->tokens[0], "creat") == 0)
		creatFile(instr_ptr, fileName, loc, direc, cwd);
	else if(strcmp(instr_ptr->tokens[0], "mkdir") == 0)
		creatDir(instr_ptr, fileName, loc, direc, cwd);
	else if(strcmp(instr_ptr->tokens[0], "open") == 0)
		printf("Command Exists\n");
	else if(strcmp(instr_ptr->tokens[0], "close") == 0)
		printf("Command Exists\n");
	else if(strcmp(instr_ptr->tokens[0], "read") == 0)
		printf("Command Exists\n");
	else if(strcmp(instr_ptr->tokens[0], "write") == 0)
		printf("Command Exists\n");
	else if(strcmp(instr_ptr->tokens[0], "rm") == 0)
		printf("Command Exists\n");
	else if(strcmp(instr_ptr->tokens[0], "rmdir") == 0)
		printf("Command Exists\n");
	else if(strcmp(instr_ptr->tokens[0], "menu") == 0)
		printMenu();
	else if(strcmp(instr_ptr->tokens[0], "info") == 0)
		printInfo(b);
	else if(strcmp(instr_ptr->tokens[0], "exit") == 0)
		printf("Exiting... \n");
	else
		printf("Command Does Not Exist, Try typing 'menu' for help!\n");
}

// Parses Boot Sector
void parseBS(struct BPB* b, char* fileName)
{
	FILE* in = fopen(fileName, "rb");
	fread(b, sizeof(struct BPB), 1, in);
	fclose(in);
}

// Find Root Directory
void getRootDirectory(struct BPB* b, struct LocationStack* loc)
{
	uint32_t /*RootDirSectors, FirstDataSector, FirstSecOfCluster,*/ RootSector;

	/*RootDirSectors	 	= ((b->BPB_RootEntCnt * 32) + (b->BPB_BytsPerSec - 1)) / b->BPB_BytsPerSec;
	FirstDataSector 	= b->BPB_RsvdSecCnt + (b->BPB_NumFATs * b->BPB_FATSz32) + RootDirSectors;
	FirstSecOfCluster 	= ((b->BPB_RootClus - 2) * b->BPB_SecPerClus) + FirstDataSector;
	RootSector = FirstSecOfCluster * b->BPB_BytsPerSec;*/
	RootSector = b->BPB_RootClus;
	push(loc, RootSector);
}

// ls command
void getLS(struct LocationStack * loc, struct direcEnt* direc, instruction* instr_ptr, struct currDir* cwd, char* fileName)
{
	if((instr_ptr->tokens[1] == NULL) || (strcmp(instr_ptr->tokens[1], ".") == 0))
	{
		getClusters(fileName, loc, cwd);
		printEnt(instr_ptr, fileName, loc, direc, cwd);
	}
	else
	{
		if (ifDir(instr_ptr, fileName, loc, direc, cwd))
		{
			push(loc, (direc->DIR_FstClusLO + direc->DIR_FstClusHI));
			getClusters(fileName, loc, cwd);
			pop(loc);
			printEnt(instr_ptr, fileName, loc, direc, cwd);
		}
	}
}

// Gets the size of the given file
void getSize(struct BPB* b, struct LocationStack* loc, char* fileName, struct direcEnt* direc, instruction* instr_ptr, struct currDir* cwd)
{
	if (instr_ptr->tokens[1] == NULL)
	{
		printf("Must give filename.\n");
		return;
	}
	if (ifFile(instr_ptr, fileName, loc, direc, cwd))
	{
		printf("%s: %d bytes\n", direc->DIR_Name, direc->DIR_FileSize);
	}
	else
	{
		printf("No such File: %s\n", instr_ptr->tokens[1]);
	}
}

void changeDir(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	if(strcmp(instr_ptr->tokens[1], "..") == 0)
	{
		pop(loc);
		return;
	}
	if(ifDir(instr_ptr, fileName, loc, direc, cwd))
	{
		push(loc, (direc->DIR_FstClusHI+direc->DIR_FstClusLO));
	}
}

void creatFile(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{

	if(ifFile(instr_ptr, fileName, loc, direc, cwd))
	{
		puts("A file with that name already exists.");
		return;
	}
	getClusters(fileName, loc, cwd);
	if (isClusFull(loc, cwd, fileName))
		enlargeDir(instr_ptr, fileName, loc, direc, cwd);
	writeNewFile(instr_ptr, fileName, loc, direc, cwd);

	/*puts("creatFile running...");
	FILE *out = fopen(fileName, "rb+");
	fseek(out, 0x4940-4, SEEK_SET);
	uint32_t BUFFER;
	fread(&BUFFER, 4, 1, out);
	printf("%d\n", BUFFER);

	int endofclus = 30;
	fwrite(&endofclus, sizeof(int), 1, out);
	fclose(out);*/
}

void creatDir(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	if(ifDir(instr_ptr, fileName, loc, direc, cwd))
	{
		puts("A file with that name already exists.");
		return;
	}
	getClusters(fileName, loc, cwd);
	if (isClusFull(loc, cwd, fileName))
		enlargeDir(instr_ptr, fileName, loc, direc, cwd);
	writeNewDir(instr_ptr, fileName, loc, direc, cwd);

	/*puts("creatFile running...");
	FILE *out = fopen(fileName, "rb+");
	fseek(out, 0x4940-4, SEEK_SET);
	uint32_t BUFFER;
	fread(&BUFFER, 4, 1, out);
	printf("%d\n", BUFFER);

	int endofclus = 30;
	fwrite(&endofclus, sizeof(int), 1, out);
	fclose(out);*/
}

void enlargeDir(instruction *instr_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	FILE *out = fopen(fileName, "rb+");

	// find free cluster
	int freeFAT = getFreeFAT(fileName);
	// insert new clus num into previous, last end clus
	fseek(out, getFATOffset(cwd->ClusMap[cwd->cmSize - 1]), SEEK_SET);
	fwrite(&freeFAT, sizeof(int), 1, out);
	// fill location of new end clus with 0FFFFFFF
	fseek(out, getFATOffset(freeFAT), SEEK_SET);
	int endOfClus = 268435448;
	fwrite(&endOfClus, sizeof(int), 1, out);
	// append new cluster to ClusMap?
	cwd->ClusMap[cwd->cmSize] = freeFAT;
	cwd->cmSize++;

	fclose(out);
}

int isClusFull(struct LocationStack* loc, struct currDir* cwd, char* fileName)
{
	int i;
	uint8_t direcBUFF[32];
	FILE *in = fopen(fileName, "rb");
	fseek(in, getDATAOffset(cwd->ClusMap[cwd->cmSize-1]), SEEK_SET);
	for(i = 0; i < 16; i++)
	{
		fread(direcBUFF, 32, 1, in);
		if(direcBUFF[0] == 00)
		{
			fclose(in);
			return 0;
		}
	}
	fclose(in);
	return 1;
}

void writeNewFile(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	int i;
	uint8_t direcBUFF[32];
	FILE *out = fopen(fileName, "rb+");
	fseek(out, getDATAOffset(cwd->ClusMap[cwd->cmSize-1]), SEEK_SET);
	for(i = 0; i < 16; i++)
	{
		fread(direcBUFF, 32, 1, out);

		//printf("%d\n", direcBUFF[0]);
		if(direcBUFF[0] == 00)
		{
			// writing the name of the file
			fseek(out, -32, SEEK_CUR);
			fwrite(inst_ptr->tokens[1], sizeof(char), strlen(inst_ptr->tokens[1]), out);
			// writing the attributes of the file
			fseek(out, 11-strlen(inst_ptr->tokens[1]), SEEK_CUR);
			char attr = 0x20;
			fwrite(&attr, sizeof(char), 1, out);
			// finding a FAT cluster for the file and populating it with the end of cluster number
			int freeFAT = getFreeFAT(fileName);
			fseek(out, getFATOffset(freeFAT), SEEK_SET);
			int endOfClus = 268435448;
			fwrite(&endOfClus, sizeof(int), 1, out);
			// writing the HI and LO bits of the file

			fseek(out, 14, SEEK_SET);
			fwrite(&freeFAT, 2, 1, out);
			//fwrite(&freeFATChar[1], sizeof(char), 1, out);
			//printf("%x %x\n", freeFATChar[1], freeFATChar[0]);

			fclose(out);
			return;
		}
	}
	fclose(out);
}

void writeNewDir(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	int i;
	uint8_t direcBUFF[32];
	FILE *out = fopen(fileName, "rb+");
	fseek(out, getDATAOffset(cwd->ClusMap[cwd->cmSize-1]), SEEK_SET);
	for(i = 0; i < 16; i++)
	{
		fread(direcBUFF, 32, 1, out);

		//printf("%d\n", direcBUFF[0]);
		if(direcBUFF[0] == 00)
		{
			// writing the name of the file
			fseek(out, -32, SEEK_CUR);
			fwrite(inst_ptr->tokens[1], sizeof(char), strlen(inst_ptr->tokens[1]), out);
			// writing the attributes of the file
			fseek(out, 11-strlen(inst_ptr->tokens[1]), SEEK_CUR);
			char attr = 0x10;
			fwrite(&attr, sizeof(char), 1, out);
			// finding a FAT cluster for the file and populating it with the end of cluster number
			int freeFAT = getFreeFAT(fileName);
			fseek(out, getFATOffset(freeFAT), SEEK_SET);
			int endOfClus = 268435448;
			fwrite(&endOfClus, sizeof(int), 1, out);
			// writing the HI and LO bits of the file

			fseek(out, 14, SEEK_SET);
			fwrite(&freeFAT, 2, 1, out);
			//fwrite(&freeFATChar[1], sizeof(char), 1, out);
			//printf("%x %x\n", freeFATChar[1], freeFATChar[0]);

			fclose(out);
			return;
		}
	}
	fclose(out);
}


int ifDir(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	getClusters(fileName, loc, cwd);
	FILE *in = fopen(fileName, "rb");

	int i;
	for (i=0; i < cwd->cmSize; i++)
	{
		fseek(in, getDATAOffset(cwd->ClusMap[i]), SEEK_SET);
		int j;
		for (j=0; j < 16; j++)
		{
			fread(direc, sizeof(struct direcEnt), 1, in);

			if (loc->top == 1 && strcmp(inst_ptr->tokens[1], "..") == 0)
			{
				puts("In root directory, cannot go back.");
				return 0;
			}
			else if ((strcmp(inst_ptr->tokens[1], "..") == 0) || (strcmp(inst_ptr->tokens[1], ".") == 0))
				return 1;
			char *dName = strtok(direc->DIR_Name, " ");
			if (direc->DIR_Name[0] == 0x00)
				break;
			if ((strcmp(inst_ptr->tokens[1], dName) == 0) && (direc->DIR_Attr == 16)) // if entry is a directory
				return 1;
		}
	}
	printf("No such Directory: %s\n", inst_ptr->tokens[1]);
	fclose(in);
	return 0;
}

// checks to see if its a file
int ifFile(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	getClusters(fileName, loc, cwd);
	FILE *in = fopen(fileName, "rb");

	int i;
	for (i=0; i < cwd->cmSize; i++)
	{
		fseek(in, getDATAOffset(cwd->ClusMap[i]), SEEK_SET);
		int j;
		for (j=0; j < 16; j++)
		{
			fread(direc, sizeof(struct direcEnt), 1, in);

			char *dName = strtok(direc->DIR_Name, " ");
			if (direc->DIR_Name[0] == 0x00)
				break;
			if ((strcmp(inst_ptr->tokens[1], dName) == 0) && (direc->DIR_Attr == 32)) // if entry is a directory
				return 1;
		}
	}
	fclose(in);
	return 0;
}


void printEnt(instruction *inst_ptr, char *fileName, struct LocationStack *loc, struct direcEnt *direc, struct currDir *cwd)
{
	//getClusters(fileName, loc, cwd);
	FILE *in = fopen(fileName, "rb");

	int i;
	for (i=0; i < cwd->cmSize; i++)
	{
		fseek(in, getDATAOffset(cwd->ClusMap[i]), SEEK_SET);
		int j;
		for (j=0; j < 16; j++)
		{
			fread(direc, sizeof(struct direcEnt), 1, in);

			if(direc->DIR_Attr == 16) // If it is a directory print with /
			{
				printf("%s/\n", direc->DIR_Name);
			}
			else if(direc->DIR_Attr == 32) // if its a file, print it
			{
				printf("%s\n", direc->DIR_Name);
			}
		}
	}
	fclose(in);
}

int getFreeFAT(char *fileName)
{
	uint32_t ClusBUFF;
	int index = 0;
	FILE * in = fopen(fileName, "rb");

	fseek(in, 0x4000, SEEK_SET);
	fread(&ClusBUFF, 4, 1, in);
	while(1)
	{
		if(ClusBUFF == 0x00000000)
		{
			fclose(in);
			return(index);
		}
		index++;
		fread(&ClusBUFF, 4, 1, in);
	}
	fclose(in);
}

void getClusters(char *fileName, struct LocationStack *loc, struct currDir* cwd)
{
	FILE *in = fopen(fileName, "rb");
	uint32_t BUFFER;
	cwd->cmSize = 0;
	cwd->ClusMap[cwd->cmSize] = loc->CurrentLoc[loc->top];
	cwd->cmSize++;
	fseek(in, getFATOffset(loc->CurrentLoc[loc->top]), SEEK_SET);
	fread(&BUFFER, 4, 1, in);
	while(1)
	{
		if(BUFFER == 0x0ffffff8)
			break;
		if(BUFFER == 0x0fffffff)
			break;
		cwd->ClusMap[cwd->cmSize] = BUFFER;
		cwd->cmSize++;
		fseek(in, getFATOffset(cwd->ClusMap[cwd->cmSize - 1]), SEEK_SET);
		fread(&BUFFER, 4, 1, in);
	}
	fclose(in);
}

int getFATOffset(int FATloc)
{
	return((FATloc * 4) + 0x4000);
}

int getDATAOffset(int FatClus)
{
	return(((FatClus-2) * 512) + (2050*512));
}

/*STACK FUNCTIONS BELOW*/

void push(struct LocationStack* loc, int location)
{
	loc->top++;
     loc->CurrentLoc[loc->top] = location;
}

void pop(struct LocationStack* loc)
{
	if(loc->top > 1)
		loc->top--;
	else
		puts("In Root Directory");
}

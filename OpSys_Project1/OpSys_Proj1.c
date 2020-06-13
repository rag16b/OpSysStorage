// COP4610
/*Group Members:
  Ryan Gutierrez
  Zachary Gutierrez
  Joshua Story*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct
{
	char **tokens;
	int numTokens;
} instruction;

typedef struct
{
	char *key;
	char *aInstr;
} alias;

void addToken(instruction *instr_ptr, char *tok);
void printTokens(instruction *instr_ptr);
int addAlias(alias *table, char *tok, int numAlias);
void printAliasTable(alias *table);
void clearInstruction(instruction *instr_ptr);
void addNull(instruction *instr_ptr);
char *resolvePathName(char *pathGiven);
char *relativePath(char *command);
int doesPathExist(char *pathGiven);
int doesFileExist(char *pathGiven);
int doesDirExist(char *pathGiven);
void execute(instruction *instr_ptr);
void IORedirect(instruction *instr_ptr);
//void pipe(instruction *instr_ptr);

int main()
{
	char *token = NULL;
	char *temp = NULL;
	char *path = NULL; // holds the full path of what was inputed via the return of resolvePathName
	//char **pathList; // holds every path in $PATH env variable
	int commandsExecuted = 0;

	instruction instr;
	instr.tokens = NULL;
	instr.numTokens = 0;

	// initializing the list of aliases
	alias aliasList[10];
	int i = 0;
	for (i; i < 10; i++)
	{
		aliasList[i].key = "";
		aliasList[i].aInstr = "";
	}
	int numAlias = 0;

	while (1)
	{
		// part 3:
		printf("%s@%s : %s > ", getenv("USER"), getenv("MACHINE"), getenv("PWD"));

		// loop reads character sequences separated by whitespace
		do
		{
			//scans for next token and allocates token var to size of scanned token
			scanf("%ms", &token);
			temp = (char *)malloc((strlen(token) + 1) * sizeof(char));

			int i;
			int start = 0;
			for (i = 0; i < strlen(token); i++)
			{
				//pull out special characters and make them into a separate token in the instruction
				if (token[i] == '|' || token[i] == '>' || token[i] == '<' || token[i] == '&')
				{
					if (i - start > 0)
					{
						memcpy(temp, token + start, i - start);
						temp[i - start] = '\0';
						addToken(&instr, temp);
					}

					char specialChar[2];
					specialChar[0] = token[i];
					specialChar[1] = '\0';

					addToken(&instr, specialChar);

					start = i + 1;
				}
			}

			if (start < strlen(token))
			{
				memcpy(temp, token + start, strlen(token) - start);
				temp[i - start] = '\0';
				addToken(&instr, temp);
			}

			//free and reset variables
			free(token);
			free(temp);

			token = NULL;
			temp = NULL;
		} while ('\n' != getchar()); //until end of line is reached

		// MIGHT NEED TO PUT ADD NULL BACK HERE /////////////////////

		/* ----------------------------------------------
		 * Beginning of built-in commands and command execution logic
		 * ----------------------------------------------
		 */

		commandsExecuted++; // MAYBE SHOULD ONLY INCREMENT WHEN COMMAND IS ACTUALLY EXECUTED
		// EXIT
		/* Exits out of the program
		 * -Doesn't wait for background processes to finish thus far
		 */
		if (strcmp(instr.tokens[0], "exit") == 0)
		{
			printf("Exiting...\n");
			printf("Commands Executed: %d\n", commandsExecuted);
			// once background processes are implemented we need to wait for them to finish here
			break;
		}
		// ALIAS
		/* Adds alias to list of ten alias's
		 */
		else if (strcmp(instr.tokens[0], "alias") == 0)
		{ // if the user types 'alias' and nothing else, it will show them a list of their aliases
			if (instr.numTokens == 1)
				printAliasTable(aliasList);
			// if the first token after 'alias' does not contain a "'" or a "=" OR the last token doesn't contian a "'" then there is a formatting error
			else if (strchr(instr.tokens[1], '\'') == NULL || strchr(instr.tokens[1], '=') == NULL || strchr(instr.tokens[instr.numTokens - 1], '\'') == NULL)
				printf("Error: Improper format. Expected: alias ALIAS_NAME='CMD'\n");
			else if (instr.numTokens >= 2)
			{ // creating a temporary string to store the CMD part of the instruction
				char *tempAlias = "";
				int i = 1;
				// iterates through all the tokens except the first and concatenates them together to create the final CMD
				for (i; i < instr.numTokens; i++)
				{
					if (i == 1)
					{
						tempAlias = (char *)malloc(strlen(instr.tokens[i]) + 2 * sizeof(char));
						strcpy(tempAlias, instr.tokens[i]);
					}
					else
					{
						tempAlias = (char *)realloc(tempAlias, strlen(instr.tokens[i]) + 2 * sizeof(char));
						strcat(tempAlias, instr.tokens[i]);
					}
					// adding a space after each concatenation
					if (i != (instr.numTokens - 1))
						strcat(tempAlias, " ");
				}
				// if addAlias returns true then we increment
				if (addAlias(aliasList, tempAlias, numAlias) == 1)
					numAlias++;

				// free and reset tempAlias
				free(tempAlias);
				tempAlias = NULL;
			}
		}
		// UNALIAS
		/*Removes given alias from list of 10 possible aliases
		 */
		else if (strcmp(instr.tokens[0], "unalias") == 0)
		{
			// if only 'unalias' is given throw error
			if (instr.numTokens == 1)
				printf("unalias: Too few rguments.\n");
			else if (instr.numTokens == 2)
			{
				int i = 0;
				for (i; i < 10; i++)
				{
					// ensure that the given alias exists and delete it
					if (strcmp(aliasList[i].key, instr.tokens[1]) == 0)
					{
						// If an alias that is in the middle of the array is removed, then shift
						//  each one up and remove the last.
						int k = i + 1;
						for (i; i < numAlias - 1; i++)
						{
							strcpy(aliasList[i].key, aliasList[k].key);
							strcpy(aliasList[i].aInstr, aliasList[k].aInstr);
							k++;
						}
						aliasList[numAlias - 1].key = "";
						aliasList[numAlias - 1].aInstr = "";
						numAlias--;
						break;
					}
				}
				if (i == 10) // if i == 10 then we went through the whole alias table and didn't find anything
				{
					printf("unalias: %s not found.\n", instr.tokens[1]);
				}
			}
		}
		// ECHO
		/* Prints the given words to stdout
		 * -if word starts with $, expand its environment variable
		 */
		else if (strcmp(instr.tokens[0], "echo") == 0)
		{
			int i = 1;
			for (i; i < instr.numTokens; i++)
			{
				// deal with possible environment variables
				if (strncmp(instr.tokens[i], "$", 1) == 0)
				{
					memmove(instr.tokens[i], instr.tokens[i] + 1, strlen(instr.tokens[i])); // remove '$'
					if (!getenv(instr.tokens[i]))
						printf(" ");
					else
						printf("%s", getenv(instr.tokens[i]));
				}
				// print tokens with no possibility of being environment variables
				else
					printf("%s", instr.tokens[i]);

				printf(" ");
			}
			printf("\n");
		}
		// CD
		/* Change directory to given path
		 * -setenve works properly everytime
		 * BUGS:
		 * -Only works for a few key directories
		 */
		else if (strcmp(instr.tokens[0], "cd") == 0)
		{
			if (doesDirExist(resolvePathName(instr.tokens[1])))
			{
				chdir(resolvePathName(instr.tokens[1]));
				setenv("PWD", resolvePathName(instr.tokens[1]), 1);
			}
			else
				printf("No such directory.\n");
		}
		// NOT BUILT-INS
		/*
		 */
		else
		{
			execute(&instr);
		}

		addNull(&instr);
		/* --------------------------------------------
	 	 * Freeing memory and resetting variables
	 	 * --------------------------------------------
	 	 */
		// couldn't figure out how to free this specific memory
		/*i = 0;
		for(i; i < *pathListLen; i++)
		{
			free(pathList[i]);
			pathList[i] = NULL;
		}*/
		//free(pathList);
		//pathList = NULL;

		free(path);
		path = NULL;

		clearInstruction(&instr);
	}

	/* --------------------------------------------
	 * Freeing memory and resetting variables
	 * --------------------------------------------
	 */
	i = 0;
	for (i; i < numAlias; i++)
	{
		free(aliasList[i].key);
		free(aliasList[i].aInstr);
		aliasList[i].key = NULL;
		aliasList[i].aInstr = NULL;
	}

	return 0;
}

//reallocates instruction array to hold another token
//allocates for new token within instruction array
void addToken(instruction *instr_ptr, char *tok)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char **)malloc(sizeof(char *));
	else
		instr_ptr->tokens = (char **)realloc(instr_ptr->tokens, (instr_ptr->numTokens + 1) * sizeof(char *));

	//allocate char array for new token in new slot
	instr_ptr->tokens[instr_ptr->numTokens] = (char *)malloc((strlen(tok) + 1) * sizeof(char));
	strcpy(instr_ptr->tokens[instr_ptr->numTokens], tok);

	instr_ptr->numTokens++;
}

// Adds the key and the instruction to the alias table (which is an array of structs with two strings).
// Returns 2 if it overwrites, 1 if it worked normally (added a new one), 0 if false
int addAlias(alias *table, char *tok, int numAlias)
{
	// using strtok() to parse through the given token and seperating into key and instruction
	char *p;
	p = strtok(tok, "='");

	// if the key already exists, then iterate through the list to find it and overwrite it
	int i = 0;
	int didOverWrite = 0;
	for (i; i < numAlias; i++)
	{
		if (strcmp(table[i].key, p) == 0)
		{
			strcpy(table[i].key, p);
			p = strtok(NULL,"'");
			strcpy(table[i].aInstr, p);
			return 2;
		}
	}
	// if number of aliases has not reached its maximum
	if (numAlias < 10)
	{
		// otherwise, just add the new one
		table[numAlias].key = (char *)malloc(strlen(p + 1) * sizeof(char));
		strcpy(table[numAlias].key, p);
		p = strtok(NULL, "'");
		table[numAlias].aInstr = (char *)malloc(strlen(p + 1) * sizeof(char));
		strcpy(table[numAlias].aInstr, p);
		return 1;
	}
	else
	{
		printf("Error: You have already reached your max number of Aliases. Please remove one if you would like to create another.\n");
		return 0;
	}
}

void printAliasTable(alias *table)
{
	int i = 0;
	for (i; i < 10; i++)
		if (strcmp(table[i].key, "") != 0)
			printf("%s : %s\n", table[i].key, table[i].aInstr);
}

void printTokens(instruction *instr_ptr)
{
	int i;
	printf("Tokens:\n");
	for (i = 0; i < instr_ptr->numTokens; i++)
	{
		if ((instr_ptr->tokens)[i] != NULL)
			printf("%s\n", (instr_ptr->tokens)[i]);
	}
}
//clears all tokens from instruction array
void clearInstruction(instruction *instr_ptr)
{
	int i;
	for (i = 0; i < instr_ptr->numTokens; i++)
		free(instr_ptr->tokens[i]);

	free(instr_ptr->tokens);

	instr_ptr->tokens = NULL;
	instr_ptr->numTokens = 0;
}

void addNull(instruction *instr_ptr)
{
	//extend token array to accomodate an additional token
	if (instr_ptr->numTokens == 0)
		instr_ptr->tokens = (char **)malloc(sizeof(char *));
	else
		instr_ptr->tokens = (char **)realloc(instr_ptr->tokens, (instr_ptr->numTokens + 1) * sizeof(char *));

	instr_ptr->tokens[instr_ptr->numTokens] = (char *)NULL;
	instr_ptr->numTokens++;
}

// path resolution
/* Resolves the given path to it's absolute path
 * BUGS:
 * -Don't know why but doesn't like to be subsequently called
 */
char *resolvePathName(char *pathGiven)
{
	int pathSize = strlen(getenv("PWD")) + strlen("path");
	char *absPath = (char *)malloc(pathSize);

	// resolving for parent directory
	if (strncmp(pathGiven, "..", 2) == 0)
	{
		// If .. is used on root then it would be an error
		if (strcmp(getenv("PWD"), "/") == 0)
		{
			printf("Error: Root does not have a parent directory.");
			return pathGiven;
		}
		strcpy(absPath, getenv("PWD"));
		// finds the last insance of a '/'
		char *temp = strrchr(absPath, '/');
		absPath[temp - absPath] = '\0';
		if (strlen(pathGiven) > 3)
			memmove(pathGiven, pathGiven + 2, strlen(pathGiven));
		else
			memmove(pathGiven, pathGiven + 3, strlen(pathGiven));
		strcat(absPath, pathGiven);
	}
	// resolving for current directory
	else if (strncmp(pathGiven, ".", 1) == 0)
	{
		// changes . to pwd and appends the rest of the given path
		strcpy(absPath, getenv("PWD"));
		memmove(pathGiven, pathGiven + 1, strlen(pathGiven));
		strcat(absPath, pathGiven);
	}
	// resolving for home directory
	else if (strncmp(pathGiven, "~", 1) == 0)
	{
		strcpy(absPath, getenv("HOME"));
		if (strlen(pathGiven) > 2)
		{
			memmove(pathGiven, pathGiven + 1, strlen(pathGiven));
			strcat(absPath, pathGiven);
		}
	}
	// resolving for root directory
	else if (strcmp(pathGiven, "/") == 0)
	{
		strcpy(absPath, pathGiven);
	}

	// checks if path exists
	if (!doesPathExist(absPath))
	{
		printf("File or Directory does not exist.\n");
		return "DNE";
	}

	return absPath;
}

/* This function splits up the PATH environment variable using ':' as a delimeter 
 *  and appends 'command' to the end of each token. It then iterates through the
 *  list and returns the path if it is found somewhere in the PATH environment
 *  variable.
 *  BUGS:
 *  -appends 'l' to the back of current directory
 */
char *relativePath(char *command)
{
	char **pathList = NULL; // c-string array
	char *tempPath = NULL;
	//int pathListLen = split(pathList, command, getenv("PATH"));

	tempPath = (char *)malloc(strlen(getenv("PATH")) * sizeof(char));
	strcpy(tempPath, getenv("PATH"));
	char *ptr = strtok(tempPath, ":");
	int i = 0;
	while (ptr != NULL)
	{
		// extend token array to accomodate an additional token
		if (i == 0)
			pathList = (char **)malloc(sizeof(char *));
		else
			pathList = (char **)realloc(pathList, (i + 1) * sizeof(char *));
		// allocate char array for new token in new slot
		pathList[i] = (char *)malloc((strlen(ptr) + 1) * sizeof(char));

		// append the command to each path
		char *temp = (char *)malloc(strlen(ptr));
		strcpy(temp, ptr);
		strcat(temp, "/");
		strcat(temp, command);
		// add the new paths to the list
		strcpy(pathList[i++], temp);

		// set ptr to next string in the 'path' variable
		ptr = strtok(NULL, ":");
	}

	// iterate through list
	int k = 0;
	for (k; k < i; k++)
	{
		// if found give pathname back
		if (doesPathExist(pathList[k]))
		{
			int h = k + 1;
			for (h; h < i; h++)
			{
				free(pathList[h]);
				pathList[h] = NULL;
			}
			return pathList[k];
		}
		free(pathList[k]);
		pathList[k] = NULL;
	}
	// if never found, return DNE and print "No such command"
	printf("Error: No such command.\n");
	return "DNE";
}

// Checks to see if the given pathname exists as a regular file or directory using the stat() function.
// https://www.tutorialspoint.com/unix_system_calls/stat.htm was used as a source to understand how to use stat().
int doesPathExist(char *pathGiven)
{
	struct stat buffer;
	return stat(pathGiven, &buffer) == 0 ? 1 : 0;
}

// Checks if file is REGULAR file
/* http://forum.codecall.net/topic/68935-how-to-test-if-file-or-directory/
was used as a reference to check if regular file */
int doesFileExist(char *pathGiven)
{
	if (doesPathExist(pathGiven))
	{
		struct stat buffer;
		stat(pathGiven, &buffer);
		return S_ISREG(buffer.st_mode);
	}
	return 0;
}

// Checks if file is DIRECTORY file
/* http://forum.codecall.net/topic/68935-how-to-test-if-file-or-directory/
was used as a reference to check if directory */
int doesDirExist(char *pathGiven)
{
	if (doesPathExist(pathGiven))
	{
		struct stat buffer;
		stat(pathGiven, &buffer);
		return S_ISDIR(buffer.st_mode);
	}
	return 0;
}

//Execute simple commands
void execute(instruction *instr_ptr)
{
	int status;
	//fork a child
	pid_t pid = fork();
	if (pid == -1) //error
		exit(1);
	if (pid == 0)
	{
		if (execv(relativePath(instr_ptr->tokens[0]), instr_ptr->tokens) < 0)
		{ //issues when calling resolvePathName
			//causes malloc() memory corruption
			printf("Command cannot be executed \n");
		}
		exit(0);
	}
	else
	{
		//parent
		waitpid(pid, &status, 0);
		return;
	}
}

void IORedirect(instruction *instr_ptr)
{
	int input = 0;  //flag for input
	int output = 0; //flag for output
	int i = 0;
	char in[128];  //hold input filename
	char out[128]; //hold output filename
	int status;
	//checks to see if special character is an input or output redirection
	//help with copying to new string with:
	//https://stackoverflow.com/questions/11515399/implementing-shell-in-c-and-need-help-handling-input-output-redirection
	while ((instr_ptr->tokens)[i] != NULL)
	{
		if (strcmp((instr_ptr->tokens)[i], "<") == 0)
		{
			input = 1;
			(instr_ptr->tokens)[i] = NULL;
			strcpy(in, (instr_ptr->tokens)[i + 1]);
		}
		if (strcmp((instr_ptr->tokens)[i], ">") == 0)
		{
			output = 1;
			(instr_ptr->tokens)[i] = NULL;
			strcpy(out, (instr_ptr->tokens)[i + 1]);
		}
		i++;
	}

	pid_t pid = fork();
	if (pid == -1)
	{ //error
		exit(1);
	}
	else if (pid == 0)
	{
		//child (help from project hint lecture)
		if (input == 1)
		{ //input
			open(resolvePathName(in), O_RDONLY);
			perror("Error Opening Input File");
			close(0);
			dup(3);
			close(3);
		}
		if (output == 1)
		{ //ouput
			open(resolvePathName(out), O_RDWR | O_CREAT | O_TRUNC);
			perror("Error Opening Output File");
			close(1);
			dup(3);
			close(3);
		}
		execv(relativePath((instr_ptr->tokens)[0]), instr_ptr->tokens);
		exit(1);
	}

	else
	{
		wait(pid, &status, 0);
	}
	return;
}

/*
void pipe(instruction *instr_ptr){
    //help from pipe example from book chapter 3
    pid_t pid;
    int fd[2];

    //create pipe
    if(pipe(fd)==-1){
        perror("Pipe Failed");
        return 1;
    }

    pid=fork();

    if(pid==-1){
        perror("Failed to Fork")
    }
    if(pid==1){//parent
        close(fd[0]);
        write(fd[1], (instr_ptr->tokens), (instr_ptr->numTokens)+1);
        close(fd[1]);
    }
    else{
        close(fd[1]);
        read(fd[0], (instr_ptr->tokens), (instr_ptr->numTokens)+1);
        close(fd[0]);
    }
    return 0;
}
*/
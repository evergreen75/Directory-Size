// Author: Joshua Bowen
// Class:  CSCI 3000
// Date:   03/22/2017

// This program calculates the total size of a directory, including subdirectories, files, etc., excluding symbolic links and
// current/parent directory links. Using this program requires a command line argument to be given at runtime of the name of
// the directory to calculate the size of.

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// getDirSize(path) returns the total size of a directory
// including the size of the files/subdirectories/hard links within it.
// Symbolic links and parent/current directory links are ignored.

int getDirSize(char* path)
{
	DIR* dirp = opendir(path);
	int total = 0;
	struct dirent* dp = readdir(dirp);
	struct stat statusInfo;
	int statResult;
	statResult = lstat(dp->d_name, &statusInfo);
	while (dp != NULL)
	{
		if (statResult == 0)
		{
			char *full_path = malloc(strlen(path) + strlen(dp->d_name) + 2);
			strcpy(full_path, path);
        	strcat(full_path, "/");
        	strcat(full_path, dp->d_name);
        	lstat(full_path, &statusInfo);
			if (strcmp(dp->d_name, "..") == 0 || strcmp(dp->d_name, ".") == 0)
			{
				printf("PARENT/CURRENT dir link ignored\n");
			}
			else if (S_ISLNK(statusInfo.st_mode))
			{
				printf("SYMBOLIC link %s ignored\n", dp->d_name);
			}
			else if (S_ISDIR(statusInfo.st_mode))
			{
				int temp = getDirSize(full_path);
				printf("DIR:     %-35s\tSIZE: %-10d bytes\n", dp->d_name, temp);
				total+= temp;
			}
			else
			{
				printf("FILE:    %-35s\tSIZE: %-10lld bytes\n", dp->d_name, statusInfo.st_size);
				total+= statusInfo.st_size;
			}
		}
		dp = readdir(dirp);
	}
	closedir(dirp);
	return total;
}

int fileExists (char* file)
{
  struct stat buffer;   
  return (stat (file, &buffer) == 0);
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("ERROR. Invalid number of arguments.\n");
		exit(0);
	}
	else if (!fileExists(argv[1]))
	{
		printf("ERROR. Directory %s does not exist.\n", argv[1]);
		exit(0);
	}
	printf("\nTotal size of directory %s is %d bytes\n\n", argv[1], getDirSize(argv[1]));
	return 0;
}
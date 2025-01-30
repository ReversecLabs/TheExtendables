#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#define __STDC_WANT_LIB_EXT1__ 1

//Defining functions specified in other files
void extensionRecon(int verbose);
void extensionInstall(char* extensionId, char* browser, char* allUsers);
void extendablesPersist(char* extensionId, char* extensionName, char* targetPath, char* browser, char* allUsers, char* description, int verbose);


#ifndef CRANKFILE_H_INCLUDED
#define CRANKFILE_H_INCLUDED

#include "crankMain.h"

int createFile(char* filePath);
int checkFile(char* filePath, int desiredLines);
int appendLine(char* filePath, char* stuff, bool addNewline);
int replaceLine(char* filePath, int lineNum, char* stuff, int maxLength, bool addNewline);
char* readLine(char* filePath, int lineNum, int maxLength, char** output);
#endif // CRANKFILE_H_INCLUDED

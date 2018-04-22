#ifndef CRANKSHAFTFILE_H_INCLUDED
#define CRANKSHAFTFILE_H_INCLUDED

#include "crankshaftMain.h"

int createFile(char* filePath);
int checkFile(char* filePath, int desiredLines);
int appendLine(char* filePath, char* stuff, bool addNewline);
int replaceLine(char* filePath, int lineNum, char* stuff, int maxLength, bool addNewline);
char* readLine(char* filePath, int lineNum, int maxLength, char** output);
#endif // CRANKSHAFTFILE_H_INCLUDED

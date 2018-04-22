#include "pistonMain.h"

int createFile(char* filePath);
int checkFile(char* filePath, int desiredLines);
int appendLine(char* filePath, char* stuff);
int replaceLine(char* filePath, int lineNum, char* stuff, int maxLength);
char* readLine(char* filePath, int lineNum, int maxLength, char** output);

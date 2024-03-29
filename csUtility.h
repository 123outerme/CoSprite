#ifndef CSUTILITY_H_INCLUDED
#define CSUTILITY_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#ifndef bool
    #define bool char
    #define false 0
    #define true 1
    #define boolToString(bool) (bool ? "true" : "false")
#endif // bool
#ifndef NULL
    #define NULL ((void*) 0)
#endif //NULL

int randInt(int low, int high, bool inclusive);
int digits(int num);
void* freeThisMem(void* x);
char* removeNewline(char* stuff, char replacement, int maxLength);
double getDistance(double x1, double y1, double x2, double y2);

#endif // CSUTILITY_H_INCLUDED

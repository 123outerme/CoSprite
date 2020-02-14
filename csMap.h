#ifndef CSMAP_H_INCLUDED
#define CSMAP_H_INCLUDED

#include <stdio.h>  //for console printing (debugging)
#include <stdlib.h>  //for calloc/free
#include <string.h>  //for str___() functions
#include <math.h>  //for log10()

//#defines:
#ifndef bool
    #define bool char
    #define false 0
    #define true 1
    #define boolToString(bool) (bool ? "true" : "false")
#endif // bool

#ifndef NULL
    #define NULL ((void*) 0)
#endif //NULL

typedef struct _csMap
{
    char** keys;
    char** values;
    int entries;
    struct _csMap** subMaps;
    bool* isSubMap;
} csMap;

void initCSMap(csMap* map, int numEntries, char* keys[], char* values[], csMap* subMaps[], bool* isSubMap[]);
void jsonToCSMap(csMap* map, char* json);
char* CSMapToJson(csMap map);
void stringToCSMap(csMap* map, char* str);
char* traverseCSMapByKey(csMap map, char* key);
csMap* traverseCSMapByKeyGetMap(csMap map, char* key);
void destroyCSMap(csMap* map);

#endif // CSMAP_H_INCLUDED

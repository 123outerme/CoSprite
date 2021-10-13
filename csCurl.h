#ifndef CSCURL_H_INCLUDED
#define CSCURL_H_INCLUDED

//#includes:
#include <stdio.h>
#include <stdlib.h>
#include "curl/curl.h"

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

//structs:
typedef struct _csCurl {
    CURL* handle;
    CURLcode retCode;
    bool online;
} csCurl;

//functions:
void initCoSpriteCurl(long flags, char* certPath, bool verbose);
void initCSCurl(csCurl* handle, char* certPath, bool verbose);
void csCurlPerformEasyGet(csCurl* handle, char* url, char* outputString);
void csCurlPerformEasyPost(csCurl* handle, char* url, char* data);
void csCurlPerformEasyPatch(csCurl* handle, char* url, char* data);
void csCurlPerformEasyPut(csCurl* handle, char* url, char* data);
void csCurlPerformEasyDelete(csCurl* handle, char* url);
size_t performEasyGetCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
void destroyCSCurl(csCurl* handle);
void closeCoSpriteCurl();

//globals:
extern csCurl globalCurl;

#endif // CSCURL_H_INCLUDED

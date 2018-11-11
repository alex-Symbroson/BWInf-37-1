
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef const char* cstr;

#define error(a, ...) \
    fprintf(stderr, "\033[0;33m" a "\033[0;37m\n", ##__VA_ARGS__)
#define werror(a, ...) \
    fwprintf(stderr, "\033[0;33m" a "\033[0;37m\n", ##__VA_ARGS__)

using namespace std;

void help(const char* cmd);

bool tryOpen(const char* name, FILE*& fp);

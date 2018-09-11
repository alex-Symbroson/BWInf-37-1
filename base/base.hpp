
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define error(a, ...) \
    fprintf(stderr, "\033[0;33m" a "\033[0;37m\n", ##__VA_ARGS__)

using namespace std;

void help(const char* cmd);

bool tryOpen(const char* name, FILE*& fp);

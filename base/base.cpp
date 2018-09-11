
#include "base.hpp"

extern const char* helpStr;

void help(const char* cmd) {
    printf(helpStr, cmd);
}

bool tryOpen(const char* name, FILE*& fp) {
    fp = fopen(name, "r");
    if (fp == NULL) {
        error("Error opening '%s'", name);
        return true;
    }
    return false;
}


/*
 *
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define error(a, ...) \
    fprintf(stderr, "\033[0;33m" a "\033[0;37m\n", ##__VA_ARGS__)

#define CHRCNT 12

// const char* biomes[] = {"Wald", "Wiese", "Häuser", "Wüste", "See", "Sumpf"};
char* watched[2];
uint16_t count, digitcnt, *digits;

bool tryOpen(const char* name, FILE*& fp) {
    fp = fopen(name, "r");
    if (fp == NULL) {
        error("Error opening '%s'", name);
        return true;
    }
    return false;
}

bool initBaywatch(FILE* fp) {
    char* line = NULL;
    ssize_t read;
    size_t len = 0;

    // Biome einlesen
    if ((read = getline(&line, &len, fp)) != -1)
        watched[0] = strndup(line, read - 1);
    else
        return true;

    if (getline(&line, &len, fp) != -1)
        watched[1] = strndup(line, read - 1);
    else {
        free(line);
        free(watched[0]);
        return true;
    }

    count = read;

    // speichere zu prüfende Zeichenpositionen
    digitcnt = 0;
    digits   = (uint16_t*)malloc(sizeof(uint16_t) * read / 2);

    for (len = 0; len < count; len++)
        if (isdigit(watched[1][len])) digits[digitcnt++] = len;

    if (digitcnt != read)
        digits = (uint16_t*)realloc(digits, sizeof(uint16_t) * digitcnt);

    free(line);

    if (!digitcnt) {
        error("no biome specializations found (only '?'?)");
        free(watched[0]);
        free(watched[1]);
        return true;
    }
    return false;
}

void freeBaywatch() {
    free(digits);
    free(watched[0]);
    free(watched[1]);
}

int main(int argc, const char* argv[]) {
    FILE* fp = NULL;

    if (argc > 1) tryOpen(argv[1], fp);
    if (fp == NULL && tryOpen("res/baywatch1.txt", fp)) {
        fclose(fp);
        return 1;
    }

    // Datei einlesen
    if (initBaywatch(fp)) {
        error("initialization failed");
        fclose(fp);
        return 1;
    }

    uint16_t i, d = 0;
    do {
        while (d < count &&
               watched[0][(d + *digits) % count] != watched[1][*digits])
            d++;

        if (d >= count) break;

        for (i = 1; i < digitcnt; i++) {
            if (watched[0][(d + digits[i]) % count] != watched[1][digits[i]])
                break;
        }

        if (i == digitcnt)
            printf(
                "found at +%3i:\n%s %.*s\n", d, watched[0] + d, d, watched[0]);

    } while (++d < count);

    printf("base:\n%s\n", watched[1]);

    freeBaywatch();
    fclose(fp);
    return 0;
}

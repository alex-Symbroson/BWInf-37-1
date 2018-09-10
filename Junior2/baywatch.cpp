
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

/*const char* biomes[] = {"Wald",  "Wiese", "Häuser",    "Wüste", "See",
                        "Sumpf", "Berge", "Flachland", "Weide"};
*/

char* watch[2];
uint16_t count, digitcnt, *digits;

bool tryOpen(const char* name, FILE*& fp) {
    fp = fopen(name, "r");
    if (fp == NULL) {
        error("Error opening '%s'", name);
        return true;
    }
    return false;
}

void freeBaywatch() {
    free(digits);
    free(watch[0]);
    free(watch[1]);
}

bool initBaywatch(FILE* fp) {
    char* line = NULL;
    ssize_t read;
    size_t len = 0;

    // George-Liste einlesen
    if ((read = getline(&line, &len, fp)) != -1)
        watch[0] = strndup(line, read - 1);
    else
        return true;

    // Longstock-Liste einlesen
    if ((read = getline(&line, &len, fp)) != -1)
        watch[1] = strndup(line, read - 1);
    else {
        free(line);
        free(watch[0]);
        return true;
    }

    count    = read;
    digitcnt = 0;
    digits   = (uint16_t*)malloc(sizeof(uint16_t) * read);

    // speichere zu prüfende Zeichenpositionen
    for (len = 0; len < count; len++)
        if (isdigit(watch[1][len])) digits[digitcnt++] = len;

    digits = (uint16_t*)realloc(digits, sizeof(uint16_t) * digitcnt);

    free(line);

    if (!digitcnt) {
        error("no biome specializations found (only '?'?)");
        freeBaywatch();
        return true;
    }
    return false;
}

int main(int argc, const char* argv[]) {
    FILE* fp = NULL;

    // Input oder Default Datei öffnen
    if (argc > 1) tryOpen(argv[1], fp);
    if (fp == NULL && tryOpen("res/baywatch1.txt", fp)) {
        fclose(fp);
        return 1;
    }

    // Datei Einlesen & Parsen
    if (initBaywatch(fp)) {
        error("initialization failed");
        fclose(fp);
        return 1;
    }

    uint16_t i, d = 0, n = 0;
    printf("base:\n%s\n\n", watch[1]);

    do {
        // Numerischen Buchstaben aus Longstock-Liste suchen
        while (d < count &&
               watch[0][(d + *digits) % count] != watch[1][*digits])
            d++;

        if (d >= count) break;

        // Mustervalidierung
        for (i = 1; i < digitcnt; i++)
            if (watch[0][(d + digits[i]) % count] != watch[1][digits[i]]) break;

        // Ausgabe
        if (i == digitcnt) {
            n++;
            printf(
                "gefunden bei +%3i:\n%s %.*s\n\n", d, watch[0] + d, d,
                watch[0]);
        }
    } while (++d < count);

    // Zusammenfassung
    if (n)
        printf("%i  Ergebnis%s gefunden!\n", n, n == 1 ? "" : "se");
    else
        error("keine Ergebnisse gefunden!\n");

    freeBaywatch();
    fclose(fp);
    return 0;
}

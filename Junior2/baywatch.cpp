
/*
 *
 */

// Includes
#include "../base/base.hpp"

const char* helpStr =
    "Usage: %s [FILE] \n"
    "Uses FILE as input (defaults to \"res/baywatch1.txt\")\n"
    "\nOptions:\n"
    "  --help        this help\n";



#define CHRCNT 12



/*const char* biomes[] = {"Wald",  "Wiese", "Häuser",    "Wüste", "See",
                        "Sumpf", "Berge", "Flachland", "Weide"};*/

char* watch[2];
uint count, digitcnt, *digits;



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
    digits   = (uint*)malloc(sizeof(uint) * read);

    // speichere zu prüfende Zeichenpositionen
    for (len = 0; len < count; len++)
        if (isdigit(watch[1][len])) digits[digitcnt++] = len;

    digits = (uint*)realloc(digits, sizeof(uint) * digitcnt);

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
    uint i;

    // Argumente einlesen
    for (i = 1; i < (uint)argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            help(*argv);
            return 0;

        } else if (*argv[i] == '-') {
            error("unknown option %s", argv[i]);
            help(*argv);
            return 1;

        } else if (!fp) {
            tryOpen(argv[i], fp);
        }
    }

    // Input oder Default Datei öffnen
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

    printf("base:\n%s\n\n", watch[1]);
    uint d = 0, n = 0;
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

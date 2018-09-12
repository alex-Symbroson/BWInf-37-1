
/*
 *
 */

// Includes
#include "../base/base.hpp"

#include <time.h>

#include <forward_list>

const char *helpStr =
    "Usage: %s [FILE] [OPTIONS]\n"
    "Uses FILE as input (defaults to \"res/superstar1.txt\")\n"
    "\nOptions:\n"
    "  --debug        show profit by each player\n"
    "  --random=[n]   generates n random player values"
    "instead of the input file\n"
    "  --help         this help\n";



forward_list<uint> player, tips;



void freeVolldaneben() {}

bool initVolldaneben(FILE *fp) {
    char line[1024];
    uint num;

    while (fgets(line, 1024, fp) != NULL)
        if (sscanf(line, "%u\n", &num) == 1) player.push_front(num);

    return false;
}



int main(int argc, const char *argv[]) {
    FILE *fp   = NULL;
    bool debug = false;
    uint i, random = 0;

    // Argumente einlesen
    for (i = 1; i < (uint)argc; i++) {
        if (!strncmp(argv[i], "--random=", 9)) {
            if (sscanf(argv[i] + 9, "%i", &random) != 1) {
                error("invalid syntax %s", argv[i]);
                help(*argv);
                return 1;
            }

        } else if (!strcmp(argv[i], "--debug")) {
            debug = true;

        } else if (!strcmp(argv[i], "--help")) {
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

    if (random) {
        // initialisiere Zufallsgenerator
        srand(time(NULL) + clock());

        // generiere Zufallszahlen
        for (i = 0; i < random; i++) player.push_front(1 + rand() % 1000);

    } else {
        // Datei öffnen
        if (fp == NULL && tryOpen("res/beispiel1.txt", fp)) return 1;

        // Datei einlesen
        if (initVolldaneben(fp)) {
            fclose(fp);
            error("initialization failed");
            return 1;
        }
        fclose(fp);
    }



    uint dmin, d, n;
    int sum;

    // initialisiere Zufallsgenerator
    srand(time(NULL) + clock());

    // 'berechne' Al's Tipp-Zahlen
    printf("\033[1;37mtips: ");
    for (i = 0; i < 10; i++) {
        tips.push_front(50 + 100 * i + rand() % 11);
        printf("%i ", tips.front());
    }

    // Berechne Gewinn
    printf("\033[0;37m\nplayer: ");
    n = sum = 0;
    for (uint &a: player) {
        n++;

        // finde Mindestabstand
        dmin = -1;
        for (uint &b: tips) {
            d = b < a ? a - b : b - a;
            if (dmin == (uint)-1 || d < dmin) dmin = d;
        }

        sum += 25 - dmin;

        // Spielerwert und mit --debug Differenz ausgeben
        if (debug) {
            printf(
                "(%5i,\033[0;3%cm%5i\033[0;37m)\t", a,
                "231"[1 + (dmin > 25) - (dmin < 25)], 25 - dmin);
        } else
            printf("%i ", a);
    }

    // Gesamtgewinn
    printf(
        "\n\033[1;3%cmsum: %i\033[0;37m\n", "231"[1 + (sum < 0) - (sum > 0)],
        sum);

    freeVolldaneben();
}

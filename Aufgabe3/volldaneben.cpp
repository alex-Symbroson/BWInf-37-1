
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
    "  --help        this help\n";


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
    FILE *fp = NULL;
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

    // Datei öffnen
    if (fp == NULL && tryOpen("res/beispiel1.txt", fp)) return 1;

    // Datei einlesen
    if (initVolldaneben(fp)) {
        fclose(fp);
        error("initialization failed");
        return 1;
    }



    uint dmin, d, n;
    int sum;

    // initialisiere Zufallsgenerator
    srand(time(NULL) + clock());

    printf("tips: ");
    for (i = 0; i < 10; i++) {
        tips.push_front(50 + 100 * i + rand() % 10);
        printf("%i ", tips.front());
    }


    printf("\nplayer:\n");
    n   = 0;
    sum = 0;
    for (uint &a: player) {
        n++;
        dmin = -1;
        for (uint &b: tips) {
            d = b < a ? a - b : b - a;
            if (dmin == (uint)-1)
                dmin = d;
            else if (d < dmin)
                dmin = d;
        }
        sum += 25 - dmin;
        printf(
            "(%5i,\033[0;3%cm%5i\033[0;37m)\t", a,
            "231"[1 + (dmin > 25) - (dmin < 25)], 25 - dmin);
    }

    printf("\nsum: %i\n", sum);

    tips.clear();
    freeVolldaneben();
    fclose(fp);
}

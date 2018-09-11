
/*
 *
 */

// Includes
#include "../base/base.hpp"

const char* helpStr =
    "Usage: %s [OPTIONS]\n"
    "Options:\n"
    "  --start=[i]   define custom start point (default: 0)\n"
    "  --help        this help\n";



#define LEDDER_COUNT 12
#define FIELD_COUNT 100
#define FIELD(p) fields[p - 1]



struct Field {
    bool touched = false; // Status: Feld betreten
    uint target  = 0;     // ggf. Ziel der Leiter
} fields[FIELD_COUNT + 1];



// Leitern
uint ledders[LEDDER_COUNT * 2] = {6,  27, 14, 19, 21, 53, 31, 42,
                                  33, 38, 46, 62, 51, 59, 57, 96,
                                  65, 85, 68, 80, 70, 76, 92, 98};



int main(int argc, const char* argv[]) {
    uint s = 0, // Startposition
        p,      // Position
        n,      // Würfelzahl
        c,      // Wurfanzahl
        i;      // Zähler

    // Argumente einlesen
    for (i = 1; i < (uint)argc; i++) {
        if (!strncmp(argv[i], "--start", 6)) {
            if (argv[i][7] != '=' || sscanf(argv[i] + 8, "%i", &s) != 1) {
                error("invalid syntax %s", argv[i]);
                help(*argv);
                return 1;
            }

        } else if (!strcmp(argv[i], "--help")) {
            help(*argv);
            return 0;

        } else if (*argv[i] == '-') {
            error("unknown option %s", argv[i]);
            help(*argv);
            return 1;
        }
    }

    // Wende Leiterliste an
    for (i = 0; i < LEDDER_COUNT * 2; i += 2) {
        FIELD(ledders[i]).target     = ledders[i + 1];
        FIELD(ledders[i + 1]).target = ledders[i];
    }

    printf(
        "Zahl | Ziel | Würfe\n"
        "-----+------+------\n");

    // alle Würfelzahlen durchprobieren
    for (n = 1; n <= 6; n++) {
        p = s;
        c = 0;

        // resette Felder
        for (i = 0; i < FIELD_COUNT; i++) fields[i].touched = false;

        // solange Ziel nicht erreicht
        while (p != FIELD_COUNT) {
            p += n;
            c++;

            // 100 überschritten
            if (p > FIELD_COUNT - 1) p = 2 * FIELD_COUNT - p;

            // ggf. Leiter benutzen
            if (FIELD(p).target) p = FIELD(p).target;

            // Feld bereits betreten? -> abbrechen
            if (FIELD(p).touched) break;

            FIELD(p).touched = true;
        }

        printf("%4i | %4s | %4i\n", n, p == FIELD_COUNT ? "ja" : "nein", c);
    }
}

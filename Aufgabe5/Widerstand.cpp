
// Includes
#include "../base/base.hpp"

#include <algorithm>
#include <vector>

#define KMAX 4

cstr helpStr =
    "Usage: %s [FILE]\n"
    "Uses FILE as input (defaults to \"res/widerstaende.txt\")\n";



vector<float> resistors; // Verfügbare Widerstände

cstr order,   // aktuelle Widerstandsreihenfolge
    best_o;   // beste Widerstandsreihenfolge
float best_r, // bester Widerstandswert
    best_d;   // bester Widerstandswertunterschied

uint len = 0,     // total r. count
    ck   = 1,     // current r. count
    cpos,         // current c. position
    srch_r,       // searched r. value
    perm[KMAX],   // current r. selection (k of n)
    best_p[KMAX]; // best r. permutation

char circuit[20], // aktuelle Schaltung
    best_c[20];   // beste Schaltung


bool initResistors(FILE* fp) {
    uint val;
    while (fscanf(fp, "%u", &val) != -1) resistors.push_back(val);
    len = resistors.size();
    return false;
}

// Speichert Eigenschaften der besten Schaltung
void saveBest(uint i, float d, float r) {
    best_o   = order;
    *circuit = i;
    best_d   = d;
    best_r   = r;
    memcpy(best_p, perm, sizeof(uint) * ck);
    memcpy(best_c, circuit, cpos + 1);
}

// Schreibt Schaltungselement in aktuelle Schaltung
inline void writeC(char c) {
    circuit[cpos++] = c;
}

// testet eine Widerstandspermutation
#define TEST(i, expr)                                                \
    order = "0123";                                                  \
    cpos  = 1;                                                       \
    if ((d = abs((r = (expr)) - srch_r)) < best_d || best_d == -1) { \
        writeC(-1);                                                  \
        saveBest(i, d, r);                                           \
    }

// testet mehrere gegebene Widerstandpermutationen
#define TEST2(i, expr, ...)                                              \
    for (cstr o: vector<cstr>(__VA_ARGS__)) {                            \
        order = o;                                                       \
        cpos  = 1;                                                       \
        if ((d = abs((r = (expr)) - srch_r)) < best_d || best_d == -1) { \
            writeC(-1);                                                  \
            saveBest(i, d, r);                                           \
        }                                                                \
    }

// gibt Widerstandswert abh. von Permutation und aktueller Auswahl zurück
inline float R(char i) {
    writeC('0' + i);
    return resistors[perm[order[i - 0] - '0']];
}

// berechnet und schreibt Reihenschaltung zweier Elemente
#define ROW(...) (writeC('R'), _ROW(__VA_ARGS__))
inline float _ROW(float a, float b, float c = 0, float d = 0) {
    writeC(')');
    return a + b + c + d;
}

// berechnet und schreibt Parallelschaltung zweier Elemente
#define PAR(...) (writeC('P'), _PAR(__VA_ARGS__))
inline float _PAR(float a, float b, float c = 0, float d = 0) {
    writeC(')');
    return 1 / (1 / a + 1 / b + (c ? 1 / c : 0) + (d ? 1 / d : 0));
}

// testet alle Schaltungen und Permutationen einer Widerstandsauswahl
void testPerm() {
    float d = -1, r = -1;

    if (!ck) { // kein Widerstand
        TEST(0, 0);
    } else if (ck == 1) { // 1 Widerstand
        TEST(10, R(0));
    } else if (ck == 2) { // 2 Widerstände
        TEST(20, ROW(R(0), R(1)));
        TEST(21, PAR(R(0), R(1)));
    } else if (ck == 3) { // 3 Widerstände
        TEST(30, ROW(R(0), R(1), R(2)));
        TEST(31, PAR(R(0), R(1), R(2)));

        TEST2(32, ROW(R(0), PAR(R(1), R(2))), {"012", "102", "201"});
        TEST2(33, PAR(R(0), ROW(R(1), R(2))), {"012", "102", "201"});


    } else if (ck == 4) { // 4 Widerstände

        TEST(40, ROW(R(0), R(1), R(2), R(3)));
        TEST(41, PAR(R(0), R(1), R(2), R(3)));

        TEST2(
            42, ROW(R(0), R(1), PAR(R(2), R(3))),
            {"0123", "0213", "0312", "1203", "1302", "2301"});
        // Entspricht oberer Schaltung
        // TEST2(
        //    42, PAR(R(0), R(1), ROW(R(2), R(3))),
        //    {"0123", "0213", "0312", "1203", "1302", "2301"});

        TEST2(
            43, PAR(R(0), ROW(R(1), R(2), R(3))),
            {"0123", "1023", "2013", "3012"});
        TEST2(
            44, ROW(R(0), PAR(R(1), R(2), R(3))),
            {"0123", "1023", "2013", "3012"});

        TEST2(
            45, PAR(ROW(R(0), R(1)), ROW(R(2), R(3))),
            {"0123", "0213", "0312", "1203", "1302", "2301"});
        TEST2(
            46, ROW(PAR(R(0), R(1)), PAR(R(2), R(3))),
            {"0123", "0213", "0312", "1203", "1302", "2301"});

        TEST2(
            47, PAR(R(0), ROW(R(1), PAR(R(2), R(3)))),
            {"0123", "0213", "0312", "1023", "1203", "1302", "2013", "2103",
             "2301", "3012", "3102", "3201"});
        TEST2(
            48, ROW(R(0), PAR(R(1), ROW(R(2), R(3)))),
            {"0123", "0213", "0312", "1023", "1203", "1302", "2013", "2103",
             "2301", "3012", "3102", "3201"});

    } else { // Fehler
        error("invalid resistor count: %i\n", ck);
        exit(1);
    }
}

// Wählt alle Permutationen von k aus n Elementen aus
// d: aktuelle Rekursionstiefe, s: Startposition
void permut_mn(uint d, uint s) {
    if (d) {
        for (uint i = s; i < len; i++) {
            perm[d - 1] = i;
            permut_mn(d - 1, i + 1);
        }
    } else
        testPerm();
}


#undef R // Neudefinition auf beste Auswahl und Reihenfolge
#define R(i) resistors[best_p[best_o[i - '0'] - '0']]

int main(int argc, char* argv[]) {
    FILE* fp = NULL;
    int i;

    // Argumente einlesen
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--help")) {
            help(*argv);
            return 0;

        } else if (*argv[i] == '-') {
            error("unknown option %s", argv[i]);
            help(*argv);
            return 1;

        } else if (!fp) {
            tryOpen(argv[i], fp);
            if (!fp) {
                error("initialization failed");
                return 1;
            }
        }
    }

    // Gegebene Widerstände einlesen
    if (fp == NULL && tryOpen("res/widerstaende.txt", fp)) {
        error("no input file");
        return 1;
    } else
        initResistors(fp);

    best_d = -1;

    // Gesuchten Widerstandswert einlesen
    printf("resistor value (uint) in ohm: ");
    do {
        if (fscanf(stdin, "%i", &srch_r) == 1) break;
        fprintf(stderr, "invalid!\n");
    } while (1);
    printf("got %i Ω\n", srch_r);

    // Schaltungen testen
    for (ck = 1; ck <= 4; ck++) permut_mn(ck, 0);

    // Ausgabe
    printf("best: circuit %i with %f Ω difference\n", *best_c, best_d);

    cpos = 1;

    if (!*best_c) printf("no resistor used");

    do {
        switch (best_c[cpos]) {
            case 'P': printf("Parallel("); break; // Parallel
            case 'R': printf("Series("); break;   // Reihe
            case ')': printf(")"); break;         // Elementende
            case -1: break;                       // Schaltungsende
            default:                              // Widerstand
                printf(
                    "%s%.lfΩ",
                    cpos == 1 || strchr("PR", best_c[cpos - 1]) ? "" : ", ",
                    R(best_c[cpos]));
        }
    } while (best_c[cpos++] != -1 && cpos < 20);

    printf(" -> %fΩ\n", best_r);
    fclose(fp);
    return 0;
}

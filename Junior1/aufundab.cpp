
/*
 *
 */

#include <stdint.h>
#include <stdio.h>

#define LEDDER_COUNT 12

// Felder (0: unberührt, 1: betreten)
bool fields[100] = {};

// Leitern
uint8_t ledders[LEDDER_COUNT * 2] = {6,  27, 14, 19, 21, 53, 31, 42,
                                     33, 38, 46, 62, 51, 59, 57, 96,
                                     65, 85, 68, 80, 70, 76, 92, 98};

int main(int argc, const char* argv[]) {
    uint8_t n, // Würfelzahl
        p,     // Position
        i;     // Zähler

    uint32_t c; // Wurfanzahl

    printf("Zahl | Ziel | Würfe\n-----+------+------\n");

    for (n = 1; n <= 6; n++) {
        p = 1;
        c = 0;

        // resette Felder
        for (i = 0; i < 100; i++) fields[i] = false;

        while (p != 100) {
            p += n;
            c++;

            // 100 überschritten
            if (p > 100) p = 200 - p;

            // suche Leiter
            for (i = 0; i < LEDDER_COUNT * 2; i++) {
                // Leiter gefunden?
                if (p == ledders[i]) {
                    if (i % 2 == 0)         // Leiteranfang
                        p = ledders[i + 1]; // gehe zu ~ende
                    else                    // Leiterende
                        p = ledders[i - 1]; // gehe zu ~anfang

                    break;
                }
            }

            // Feld bereits betreten? -> abbrechen
            if (fields[p - 1]) break;

            fields[p - 1] = true;
        }

        printf("%4i | %4s | %4i\n", n, p == 100 ? "ja" : "nein", c);
    }
}

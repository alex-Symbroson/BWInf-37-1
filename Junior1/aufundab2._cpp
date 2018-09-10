
/*
 *
 */

#include <stdint.h>
#include <stdio.h>

#define LEDDER_COUNT 12
#define FIELD_COUNT 100
#define FIELD(p) fields[p - 1]

struct Field {
    bool touched   = false; // Status: Feld betreten
    uint8_t target = 0;     // ggf. Ziel der Leiter
} fields[FIELD_COUNT + 1];

// Leitern
uint8_t ledders[LEDDER_COUNT * 2] = {6,  27, 14, 19, 21, 53, 31, 42,
                                     33, 38, 46, 62, 51, 59, 57, 96,
                                     65, 85, 68, 80, 70, 76, 92, 98};

int main(int argc, const char* argv[]) {
    uint8_t n, // Würfelzahl
        p,     // Position
        i;     // Zähler

    uint32_t c; // Wurfanzahl

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
        p = 1;
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

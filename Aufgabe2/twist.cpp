
/*
 *
 */

// Includes
#include "../base/base.hpp"

#include <forward_list>

const char* helpStr =
    "Usage: %s [FILE] [OPTIONS]\n"
    "Uses FILE as input (defaults to \"res/enttwist.txt\")\n"
    "\nOptions:\n"
    "  --check-alt   check spelling alternatives (ß->ss, th->t)\n"
    "  --help        this help\n";



#define MAXLEN 40       // maximale Wortlänge
#define CHRCNT (26 + 5) // = lowercase + äöüß + 1 for others



// speichert Wörter sortiert nach Länge
forward_list<char*> wordMap[MAXLEN][CHRCNT];



// gibt Zeichennummer zurück (nicht Zeichencode)
uint charNum(char* cp) {
    static char lc;

    if (*cp >= 'A' && *cp <= 'Z') return *cp - 'A';
    if (*cp >= 'a' && *cp <= 'z') return *cp - 'a';

    uint sub    = 0;           //  Ergebnis-Subtrahent
    bool isSpec = *cp == *"ü"; // Status: ist Umlaut

    // Umlaute sind nicht in ASCII enthalten, stattdessen bestehen sie aus zwei
    // ASCII-Zeichen, wobei der erste (hier) bei alles gleich ist
    if (isSpec || lc == *"ü") {
        lc = *cp;

        // vergleiche nächstes Zeichen wenn das letzte ein Umlautbeginn war,
        // sonst das erste
        switch (cp[isSpec]) {
            case "ü"[1]:
            case "Ü"[1]: sub = 2; break;
            case "ä"[1]:
            case "Ä"[1]: sub = 3; break;
            case "ö"[1]:
            case "Ö"[1]: sub = 4; break;
            case "ß"[1]: sub = 5; break;
            default: sub = 0;
        }

        if (sub && isSpec) sub = 1;
    }
    return CHRCNT - sub;
}

// zählt Umlaute
uint cntUml(char* word, int len = -1) {
    char* c = word;
    int n   = 0, i;

    if (len == -1) {
        while (*c)
            if (*c++ == *"ü") n++;
    } else {
        for (i = 0; i < len && *c; i++)
            if (*c++ == *"ü") n++;
    }

    return n;
}



void freeWordMap() {
    uint i, j;
    for (i = 0; i < MAXLEN; i++) {
        for (j = 0; j < CHRCNT; j++) {
            wordMap[i][j].remove_if([](char* word) {
                free(word);
                return true;
            });
        }
    }
}

bool initWordMap() {
    FILE* fp;
    char* line = NULL;
    ssize_t read;
    size_t len = 0;

    if (tryOpen("res/woerterliste.txt", fp)) return true;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (read > 1) {
            if (line[read - 1] == '\n') read--;
            if (read > MAXLEN) error("word %.*s too long", (int)read, line);
            wordMap[read - cntUml(line, read)][charNum(line)].push_front(
                strndup(line, read));
        }
    }
    fclose(fp);
    return false;
}



int main(int argc, const char* argv[]) {
    FILE* fp   = NULL;
    char* line = NULL;
    size_t len = 0;
    uint i, read;
    bool chkAlt = false;

    // Argumente einlesen
    for (i = 1; i < (uint)argc; i++) {
        if (!strcmp(argv[i], "--check-alt")) {
            chkAlt = true;

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

    if (fp == NULL && tryOpen("res/enttwist.txt", fp)) {
        error("initialization failed");
        fclose(fp);
        return 1;
    }

    if (initWordMap()) {
        fclose(fp);
        freeWordMap();
        return 1;
    }

    // lese Eingabedatei zeilenweise
    while ((read = getline(&line, &len, fp)) != (uint)-1) {
        if (read > 1) {
            char *c = line, *b;
            uint l, corr;

            do {
                // lese nicht-Wörter
                l = 0;
                b = c;
                while (*c && charNum(c) == CHRCNT) c++, l++;
                printf("%.*s", l, b);

                // lese Wörter
                l = 0;
                b = c;
                while (*c && charNum(c) != CHRCNT) c++, l++;

                // Keine vertauschten Bst. möglich
                if (l < 3 || l - cntUml(b, l) <= 3) {
                    printf("\033[0;32m%.*s\033[0;37m", l, b);
                    continue;

                    // Wort zu lang
                } else if (l > MAXLEN) {
                    printf("\033[0;31m%.*s\033[0;37m", l, b);
                    continue;
                }

                char p[l], t[l], *match = NULL;
                uint j, found = 0;
                corr = 0;

            search:
                // suche Basiswörter
                for (char* word: wordMap[l - cntUml(b, l)][charNum(b)]) {
                    if ((b[l - 1] != word[l - 1]) || // Endbst. verschieden
                        ((b[0] != word[0]) &&        // Anfangsbst. verschieden
                         (b[0] != word[0] - 'a' + 'A')) // klein->groß
                    )
                        continue;

                    // kopiere Eingabewort zu p für Buchstabenzählung
                    strncpy(p, b, l);

                    // von 1 - l-1 weil Anfangs- und Endbuchstaben bereits
                    // überprüft worden sind
                    for (i = 1; i < l - 1; i++) {
                        for (j = 1; j < l - 1; j++) {
                            if (charNum(word + i) == charNum(p + j)) {
                                p[j] = ' '; // überschreibe gefundene Zeichen
                                break;
                            }
                        }

                        // Zeichen nicht gefunden
                        if (j == l - 1) break;
                    }

                    // alle Zeichen gefunden
                    if (i == l - 1) {
                        if (match) {
                            // gleiches Wort gefunden -> überspringen
                            if (!strncmp(match + 1, word + 1, l - 1)) continue;

                            // bereits anderes Wort gefunden -> gelb
                            printf("\033[0;33m%.*s\033[0;37m|", l, match);
                        }

                        // kopiere richtiges Wort temporär in Eingabezeile
                        // Anfangs- und Endbuchstabe wird beibehalten
                        strncpy(b + 1, word + 1, l - 2);
                        match = b;
                        found++;
                    }
                }

                // teste Schreibalternativen
                if (chkAlt && !found) {
                    if (!corr) {
                        // Temp-Speicher
                        corr = 1;
                        strncpy(t, b, l);
                        strncpy(p, b, l);
                    }

                    char* f;
                    uint d;

                    //ß->ss
                    if ((f = strstr(p, "ß"))) {
                        d = f - p;
                        strncpy(b + d, "ss", 2);
                        goto search;
                    }

                    // th->t
                    if ((f = strstr(p, "th"))) {
                        d = f - p;
                        strncpy(b + (d + 1), b + (d + 2), l - d - 1);
                        corr++;
                        l--;
                        goto search;
                    }

                    // Th->T
                    if ((f = strstr(p, "Th"))) {
                        d = f - p;
                        strncpy(b + (d + 1), b + (d + 2), l - d - 1);
                        corr++;
                        l--;
                        goto search;
                    }

                    // Rückkopieren falls erfolglos
                    if (corr) {
                        l += corr - 1;
                        strncpy(b, t, l);
                    }
                }

                if (found == 1) // eine Übereinstimmung -> grün
                    printf("\033[0;32m%.*s\033[0;37m", l, match);
                else if (!found) // keine Übereinstimmung -> rot
                    printf("\033[0;31m%.*s\033[0;37m", l, b);
                else // mehrere Übereinstimmungen -> gelb
                    printf("\033[0;33m%.*s\033[0;37m", l, match);

            } while (*c);
        } else
            printf("\n");
    }


    printf("\n");
    fclose(fp);
    free(line);
    freeWordMap();
    return 0;
}

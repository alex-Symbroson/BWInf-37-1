
/*
 *
 */

#include "headers.hpp"
using namespace std;

// (Zeiger auf Wortanfang) Liste[Anfangsbuchstabe][Wortlänge]
#define MAXLEN 40
#define CHRCNT (26 + 5) // = ascii-chars + äöü start + spec
forward_list<char*> wordMap[CHRCNT][MAXLEN];

uint8_t charNum(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a'; // + 26;

    switch (c) {
        case -61: return CHRCNT - 1; // same for ä ö ü ß
        case "ü"[1]:
        case "Ü"[1]: return CHRCNT - 2;
        case "ä"[1]:
        case "Ä"[1]: return CHRCNT - 3;
        case "ö"[1]:
        case "Ö"[1]: return CHRCNT - 4;
        case "ß"[1]: return CHRCNT - 5;
        default: return CHRCNT;
    }
}

bool tryOpen(const char* name, FILE*& fp) {
    fp = fopen(name, "r");
    if (fp == NULL) {
        printf("\033[1;33mError opening '%s'\033[0;37\n", name);
        return true;
    }
    return false;
}

bool initWordMap() {
    FILE* fp;
    char* line = NULL;
    size_t len = 0, read;

    if (tryOpen("res/woerterliste2.txt", fp)) return true;
    while ((read = getline(&line, &len, fp)) != (size_t)-1) {
        if (--read > 1) {
            if (read > MAXLEN) printf("word %.*s too long\n", (int)read, line);

            /* // prevent doubles
            bool found = false;
            for (char* word: wordMap[charNum(*line)][read]) {
                if (!strncmp(word, line, read)) found = true;
                if (found) printf("%.*s\n", (int)read, line);
                break;
            }
            if (!found)*/
            wordMap[charNum(*line)][read].push_front(strdup(line));
        }
    }
    fclose(fp);
    return false;
}

void freeWordMap() {
    uint8_t i, j;
    for (i = 0; i < CHRCNT; i++) {
        for (j = 0; j < MAXLEN; j++) {
            wordMap[i][j].remove_if([](char* word) {
                free(word);
                return true;
            });
        }
    }
}

int main(int argc, const char* argv[]) {
    if (initWordMap()) return 1;
    FILE* fp   = NULL;
    char* line = NULL;
    size_t len = 0, read;

    if (argc > 1) tryOpen(argv[1], fp);

    if (fp == NULL && tryOpen("res/enttwist.txt", fp)) {
        fclose(fp);
        freeWordMap();
        return 1;
    }

    // lese Eingabedatei zeilenweise
    while ((read = getline(&line, &len, fp)) != (size_t)-1) {
        if (read > 1) {
            char *c = line, *b;
            uint8_t l;

            do {
                // lese nicht-Wörter
                l = 0, b = c;
                while (*c && charNum(*c) == CHRCNT) c++, l++;
                printf("%.*s", l, b);

                // lese Wörter
                l = 0, b = c;
                while (*c && charNum(*c) != CHRCNT) c++, l++;
                if (!*c) break;

                char p[l];
                uint8_t i, j, found = 0;
                forward_list<char*> matches;

                // suche Basiswörter
                for (char* word: wordMap[charNum(*b)][l]) {
                    // vergleiche Zeichen
                    strncpy(p, b, l);
                    // Start von 1 weil Anfanggsbst. gleich sind
                    for (i = 1; i < l; i++) {
                        for (j = 1; j < l; j++) {
                            if (charNum(word[i]) == charNum(p[j])) {
                                p[j] = '-'; // überschreibe gefundene Zeichen
                                break;
                            }
                        }
                        // Zeichen nicht gefunden
                        if (j == l) break;
                    }

                    // alle Zeichen gefunden
                    if (i == l) {
                        matches.push_front(word);
                        *matches.front() = *b;
                        found++;
                    }
                }

                switch (found) {
                    case 1: // eindeutig
                        printf("\033[0;32m%.*s\033[0;37m", l, matches.front());
                        break;

                    case 0: // keine Übereinstimmung
                        printf("\033[0;31m%.*s\033[0;37m", l, b);
                        break;

                    default: // mehrdeutig
                        for (char* word: matches) {
                            printf("\033[0;33m%.*s\033[0;37m", l, word);
                            if (--found) printf("|");
                        }
                }
            } while (*c);
        }
    }

    printf("\n");
    free(line);
    fclose(fp);
    freeWordMap();
    return 0;
}


// Includes
#include "../base/base.hpp"

#include <locale.h>
#include <wchar.h>
#include <wctype.h>

#include <forward_list>

const char* helpStr =
    "Usage: %s [FILE] [OPTIONS]\n"
    "Uses FILE as input\n"
    "if no file is specified, stdin is used\n"
    "\nOptions:\n"
    "  --untwist     untwist twisted text"
    "  --check-alt   check spelling alternatives (ß->ss, th->t)\n"
    "  --help        shows this help\n";



#define MAXLEN 40 // maximale Wortlänge
#define CHRCNT (26 + 10)
// = 26 Buchstaben + 9 verwendete Umlaute + 1 für andere Sonderzeichen/Umlaute


// speichert Wörter sortiert nach Länge
forward_list<wchar_t*> wordMap[MAXLEN][CHRCNT];

wchar_t uml[] = L"ÜÄÖßÉÀÊÂÑ";

// gibt Zeichennummer zurück (nicht Zeichencode)
uint charNum(wchar_t wc) {
    if (isalpha(wc)) return towupper(wc) - 'A';

    if (iswalpha(wc)) {
        // check special chars
        wchar_t* p = wcschr(uml, towupper(wc));
        return CHRCNT - (p ? p - uml + 2 : 1);
    }
    return CHRCNT;
}

// zählt Umlaute
uint cntUml(wchar_t* word, int len = -1) {
    wchar_t* c = word;
    int n      = 0, i;

    if (len == -1) {
        while (*c)
            if (wcschr(uml, towupper(*c++))) n++;
    } else {
        for (i = 0; i < len && *c; i++)
            if (wcschr(uml, towupper(*c++))) n++;
    }

    return n;
}



void freeWordMap() {
    uint i, j;
    for (i = 0; i < MAXLEN; i++) {
        for (j = 0; j < CHRCNT; j++) {
            wordMap[i][j].remove_if([](wchar_t* word) {
                free(word);
                return true;
            });
        }
    }
}

bool initWordMap() {
    FILE* fp;
    wchar_t line[MAXLEN];
    size_t len = 0;

    if (tryOpen("res/woerterliste.txt", fp)) return true;
    while ((fgetws(line, MAXLEN, fp)) != NULL) {
        len = wcslen(line);
        if (line[len - 1] == '\n') line[--len] = 0;
        if (len > MAXLEN) werror(L"word %hs too long", line);
        wordMap[len][charNum(*line)].push_front(wcsdup(line));
    }
    fclose(fp);
    return false;
}


int main(int argc, const char* argv[]) {
    FILE* fp = NULL;
    uint i;
    bool chkAlt = false, untwist = false;
    setlocale(LC_CTYPE, "");

    // Argumente einlesen
    for (i = 1; i < (uint)argc; i++) {
        if (!strcmp(argv[i], "--check-alt")) {
            chkAlt = true;

        } else if (!strcmp(argv[i], "--untwist")) {
            untwist = true;

        } else if (!strcmp(argv[i], "--help")) {
            help(*argv);
            return 0;

        } else if (*argv[i] == '-') {
            werror(L"unknown option %s", argv[i]);
            help(*argv);
            return 1;

        } else if (!fp) {
            tryOpen(argv[i], fp);
            if (!fp) {
                werror(L"initialization failed");
                return 1;
            }
        }
    }

    if (fp == NULL) {
        fwprintf(stderr, L"info: press ctrl-d to exit from input\n\n");
        fp = stdin;
    }

    if (untwist && initWordMap()) {
        fclose(fp);
        freeWordMap();
        return 1;
    }

    // lese Eingabedatei zeilenweise
    wint_t wc;
    wchar_t buf[MAXLEN];
    wc = fgetwc(fp);

    // randomize
    srand(clock() + (long)fp);

    while (wc && wc != WEOF) {
        uint l;
        int corr;

        // lese & schreibe nicht-Wörter
        l    = 0;
        *buf = wc;
        while (wc != WEOF && charNum(wc) == CHRCNT) {
            if (wc == '\033') // ignore color escape sequences
                fwscanf(fp, L"[%i;%im", &i, &i);
            else
                buf[l++] = wc;
            wc = fgetwc(fp);
        }
        wprintf(L"%.*ls", l, buf);

        // lese Wörter
        l    = 0;
        *buf = wc;
        while (wc != WEOF && charNum(wc) != CHRCNT) {
            buf[l++] = wc;
            wc       = fgetwc(fp);
        }


        // Keine vertauschten Bst. möglich
        if (l <= 3) {
            wprintf(L"\033[0;%im%.*ls\033[0;37m", untwist ? 32 : 37, l, buf);
            continue;

            // Wort zu lang
        } else if (untwist && l > MAXLEN) {
            wprintf(L"\033[0;31m%.*ls\033[0;37m", l, buf);
            continue;
        }

        // twist word
        if (!untwist) {
            for (i = 2; i <= l; i++)
                swap(buf[rand() % (l - 2) + 1], buf[rand() % (l - 2) + 1]);

            wprintf(L"%.*ls", l, buf);

            // untwist
        } else {
            wchar_t p[MAXLEN], tmp[MAXLEN], *match = NULL;
            uint j, found = 0;
            corr = 255;

        search:
            // suche Basiswörter
            for (wchar_t* word: wordMap[l][charNum(*buf)]) {
                if ((buf[l - 1] != word[l - 1]) || // Endbst. verschieden
                    ((buf[0] != word[0]) &&        // Anfangsbst. verschieden
                     (buf[0] != word[0] - 32)) ||  // klein->groß
                    (cntUml(buf, l) != cntUml(word, l)) // gleiche Umlautzahl
                )
                    continue;

                // kopiere Eingabewort zu p für Buchstabenzählung
                wcsncpy(p, buf, l);

                // von 1 - l-1 weil Anfangs- und Endbuchstaben bereits
                // überprüft worden sind
                for (i = 1; i < l - 1; i++) {
                    for (j = 1; j < l - 1; j++) {
                        if (charNum(word[i]) == charNum(p[j])) {
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
                        if (!wcsncmp(match + 1, word + 1, l - 1)) continue;

                        // bereits anderes Wort gefunden -> gelb
                        wprintf(L"\033[0;33m%.*ls\033[0;37m|", l, match);
                    }

                    // kopiere richtiges Wort temporär in Eingabezeile
                    // Anfangs- und Endbuchstabe wird beibehalten
                    wcsncpy(buf + 1, word + 1, l - 2);
                    match = buf;
                    found++;
                }
            }

            // teste Schreibalternativen
            if (chkAlt && !found) {
                if (corr == 255) {
                    corr = 0;
                    // Temp-Speicher
                    wcsncpy(tmp, buf, l);

                    wchar_t* f;
                    uint d;

                    // ß -> ss
                    if ((d = (f = wcschr(buf, L'ß')) - buf) < l) {
                        wcsncpy(f + 1, tmp + d, l - d);
                        *f   = L's';
                        f[1] = L's';
                        corr--;
                        l++;
                    }
                    // h ->
                    else if ((d = (f = wcschr(buf, L'h')) - buf) < l) {
                        wcsncpy(f, f + 1, l - d + 1);
                        corr++;
                        l--;
                    }
                    // dt -> t
                    else if (
                        (wcschr(buf, L't') - buf < l) &&
                        (d = (f = wcschr(buf, L'd')) - buf) < l) {
                        wcsncpy(f, f + 1, l - d);
                        corr++;
                        l--;
                    }

                    goto search;
                } else {
                    // Rückkopieren falls erfolglos
                    l += corr;
                    wcsncpy(buf, tmp, l);
                }
            }

            if (found == 1) // eine Übereinstimmung -> grün
                wprintf(L"\033[0;32m%.*ls\033[0;37m", l, match);
            else if (!found) // keine Übereinstimmung -> rot
                wprintf(L"\033[0;31m%.*ls\033[0;37m", l, buf);
            else // mehrere Übereinstimmungen -> gelb
                wprintf(L"\033[0;33m%.*ls\033[0;37m", l, match);
        }
    }

    wprintf(L"\n");
    fclose(fp);
    if (untwist) freeWordMap();
    return 0;
}

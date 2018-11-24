
// Includes
#include "../base/base.hpp"

#include <algorithm>
#include <list>

const char *helpStr =
    "Usage: %s [FILE] [OPTIONS]\n"
    "Uses FILE as input (defaults to \"res/schrebergaerten.txt\")\n"
    "\nOptions:\n"
    "  --circuit=[i]  runs circuit specified with i. before\n"
    "  --debug        shows every permutation result\n"
    "  --help         this help\n";



struct Rect {
    uint x, y, w, h, i;

    Rect(uint X, uint Y, uint W, uint H, uint I):
        x(X), y(Y), w(W), h(H), i(I) {}

    void assign(Rect *v) {
        x = v->x;
        y = v->y;
        w = v->w;
        h = v->h;
        i = v->i;
    }
};



// enthält {ggT(w1, .., wn), ggT(h1, .., hn), minw, i, n} einer Gartenliste
Rect *opt,
    // order: enthält Zeiger auf Gärten in optimaler Reihenfolge
    **order;

// Rechteck:
uint minA, // Minimalfläche
    minw,  // Minimalbreite
    maxW,  // Breite
    maxH;  // Höhe

bool debug = false;

// das erste Element einer Gartenliste sind die 'opt' Parameter
list<list<Rect *>> gardenList;



uint ggt(uint x, uint y) {
    uint c;
    while (y) {
        c = x % y;
        x = y;
        y = c;
    }
    return x;
}



void freeSchrebergaerten() {
    for (auto &gardens: gardenList)
        for (Rect *&rect: gardens) delete rect;
}

bool initSchrebergaerten(FILE *fp) {
    uint w, h, l, c, n, dx, dy, minw;
    char line[1024], *lp;
    bool read = false;

    // Erwartet Daten im Format "[n].\n[w1] x [h1], ..., [wn] x [hn]."
    while (fgets(line, 1024, fp) != NULL) {
        // lese Gartenliste wenn zuvor Listennummer gefunden ("[n].\n")
        if (read) {
            lp = line;
            n = minw = dx = dy = 0;
            gardenList.push_back({});

            // scannt Zeile nach [w] x [h] Paar
            while (*lp && (l = sscanf(lp, " %u x %u", &w, &h)) != (uint)EOF) {
                // Speichert Garten
                if (l == 2) {
                    gardenList.back().push_back(new Rect(0, 0, w, h, n++));
                    // berechnet ggT (Assoziativgesetz gilt)
                    dx = dx ? ggt(dx, w) : w;
                    dy = dy ? ggt(dy, h) : h;
                }

                // gehe zu nächstem Garten oder Zeilenende
                while (*lp)
                    if (*lp++ == ',') break;
            }

            // speichere zusätzliche Gartenlisteninformationen
            // (ggt(w), ggt(h), minw, maxh, n)
            gardenList.back().push_back(new Rect(dx, dy, minw, c, n));
            dx = dy = 0;
            read    = false;

            // prüfe Zeile auf "[n].\n" ([n] -> index)
        } else {
            c  = 0;
            lp = line;
            while (isdigit(*lp)) c = 10 * c + *lp++ - '0';
            read = lp[0] == '.' && lp[1] == '\n';
        }
    }

    return false;
}



// Ausgabe
void printGardens(Rect **list, uint w, uint h) {
    uint x, y, i, out[h][w];
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++) out[y][x] = 0;

    for (i = 0; i < opt->i; i++)
        for (y = 0; y < list[i]->h; y++)
            for (x = 0; x < list[i]->w; x++)
                out[list[i]->y + y][list[i]->x + x] = list[i]->i + 1;

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            if (out[y][x]) {
                printf("\033[30;%um%2u", out[y][x] % 8 + 100, out[y][x]);
            } else
                printf("\033[0;90m 0");
        }
        printf("\033[0;37m\n");
    }
    printf("\n");
}

// Sortiert Gartenpermutation in Rechteck
void testGardens(Rect **gds) {
    uint i, j,          // Zähler
        n = 1 + opt->i, // ersten n Rechtecke ergeben Maximalbreite
        maxw,           // Macimalbreite des Rechtecks
        maxy = 0;       // höchster Y-Wert

    while (--n) {
        maxw = 0;

        //  maxw berechnen
        for (i = 0; i < n; i++) maxw += gds[i]->w;
        if (maxw < minw) maxw = minw;

        // Positionen resetten
        for (i = 0; i < opt->i; i++) {
            gds[i]->x = 0;
            gds[i]->y = 0;
        }

        // Alle Gärten einsortieren
        for (i = 0; i < opt->i; i++) {
            bool coll;

            do {
                coll = false;

                // Überschneidung mit anderem Garten?
                for (j = 0; j < i; j++) {
                    if ((gds[i]->x < gds[j]->x + gds[j]->w) &&
                        (gds[i]->y < gds[j]->y + gds[j]->h) &&
                        (gds[i]->y + gds[i]->h > gds[j]->y) &&
                        (gds[i]->x + gds[i]->w > gds[j]->x)) {
                        // akt. Garten hinter gefundenen Garten bewegen
                        gds[i]->x = gds[j]->x + gds[j]->w;

                        // Rechteckbreite überschritten
                        if (gds[i]->x + gds[i]->w > maxw) {
                            gds[i]->x = 0;
                            gds[i]->y++;
                        }
                        coll = true;
                        break;
                    }
                }
            } while (coll);

            if (gds[i]->y + gds[i]->h > maxy) maxy = gds[i]->y + gds[i]->h;
        }

        // neues minimum der Rechteckfläche gefunden
        if (!minA || (maxy * maxw != 0 && maxy * maxw < minA)) {
            minA = maxy * maxw;
            maxW = maxw;
            maxH = maxy;
            // alle Garteneigenschaften für Ausgabe kopieren
            for (i = 0; i < opt->i; i++) order[i]->assign(gds[i]);
        }

        if (debug) printGardens(gds, maxw, maxy);
    }
}



// Permutation
void permut(Rect **r, uint end) {
    if (end == 0) {
        testGardens(r);
    } else {
        permut(r, end - 1);
        uint i;

        for (i = 0; i < end; i++) {
            // nicht tauschen und permutieren wenn Größe übereinstimmt
            if (r[i]->w != r[end]->w || r[i]->h != r[end]->h) {
                swap(r[i], r[end]);
                permut(r, end - 1);
                swap(r[i], r[end]);
            }
        }
    }
}



int main(int argc, const char *argv[]) {
    FILE *fp = NULL;
    uint i, cnum = -1;

    // Argumente einlesen
    for (i = 1; i < (uint)argc; i++) {
        if (!strcmp(argv[i], "--debug")) {
            debug = true;

        } else if (!strcmp(argv[i], "--help")) {
            help(*argv);
            return 0;

        } else if (!strncmp(argv[i], "--circuit=", 10)) {
            if (sscanf(argv[i] + 10, "%u", &cnum) != 1) {
                error("invalid syntax %s", argv[i]);
                help(*argv);
                return 1;
            }

        } else if (*argv[i] == '-') {
            error("unknown option %s", argv[i]);
            help(*argv);
            return 1;

        } else if (!fp) {
            tryOpen(argv[i], fp);
        }
    }

    // Default Datei öffnen falls nötig
    if (fp == NULL && tryOpen("res/schrebergaerten.txt", fp)) goto error;

    // Datei Einlesen & Parsen
    if (initSchrebergaerten(fp)) {
        error("initialization failed");
        goto error;
    }
    fclose(fp);

    // jede Gartenliste aus Eingabedatei behandeln
    for (auto &gardens: gardenList) {
        minw = i = 0;

        // lese Gartenlisteninformationen
        opt = gardens.back();
        gardens.pop_back();

        if (cnum != (uint)-1 && cnum != opt->h) continue;
        printf("\n-- %i: ---------------------\n", opt->h);

        // wende ggT an
        opt->w /= opt->x;

        // besetze Speicher
        Rect *garden[opt->i], // random-access Gartenliste
            *_order[opt->i];  // für Kopie der besten Reihenfolge
        order = _order;

        // resette minimale Fläche
        minA = 0;

        // Vorberechnungen
        for (Rect *&rect: gardens) {
            // Speichere Zeiger auf Gärten in Array
            garden[i++] = rect;

            // wende ggT an
            rect->w /= opt->x;
            rect->h /= opt->y;

            // berechne minimale Rechteck-Breite
            if (minw < rect->w) minw = rect->w;

            // erstelle Zeiger auf Garten in order-Liste
            *order++ = new Rect(0, 0, 0, 0, i);
        }

        // teste alle Garten-Permutationen
        order = _order;
        permut(garden, opt->i - 1);

        // Ausgabe
        printf(
            "min: %um x %um = %um²\n", maxW * opt->x, maxH * opt->y,
            minA * opt->x * opt->y);

        printGardens(order, maxW, maxH);

        for (i = 0; i < opt->i; i++) delete order[i];
    }

    freeSchrebergaerten();
    return 0;

error:
    fclose(fp);
    return 1;
}

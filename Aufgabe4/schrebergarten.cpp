
/*
 *
 */

// Includes
#include "../base/base.hpp"

#include <list>

const char *helpStr =
    "Usage: %s [FILE] [OPTIONS]\n"
    "Uses FILE as input (defaults to \"res/schrebergaerten.txt\")\n"
    "\nOptions:\n"
    "  --debug   shows every permutation result\n"
    "  --help    this help\n";



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



// enthält {ggT(w1, .., wn), ggT(h1, .., hn), minw, maxh, n} einer Gartenliste
Rect *opt,
    // order: enthält Zeiger auf Gärten in optimaler Reihenfolge
    **order;

uint minA, minw, maxW, maxH;
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

uint log2(uint v) {
    int n = -1;
    while (v) v >>= 1, n++;
    return n;
}



void freeSchrebergaerten() {
    for (auto &gardens: gardenList)
        for (Rect *&rect: gardens) delete rect;
}

bool initSchrebergaerten(FILE *fp) {
    uint w, h, l, c, n, dx, dy, minw, maxh;
    char line[1024], *lp;
    bool read = false;

    // expects data in format "[n].\n[w1] x [h1], ..., [wn] x [hn]."

    while (fgets(line, 1024, fp) != NULL) {
        if (read) {
            lp = line;
            n = minw = maxh = dx = dy = 0;
            gardenList.push_back({});

            // scannt Zeile nach [w] x [h] Paar
            while (*lp && (l = sscanf(lp, " %u x %u", &w, &h)) != (uint)EOF) {
                // Speichert Garten
                if (l == 2) {
                    gardenList.back().push_back(new Rect(0, 0, w, h, n++));
                    dx = dx ? ggt(dx, w) : w;
                    dy = dy ? ggt(dy, h) : h;
                }

                // gehe zu nächstem Garten oder Zeilenende
                while (*lp)
                    if (*lp++ == ',') break;
            }
            gardenList.back().push_back(new Rect(dx, dy, minw, maxh, n));
            gardenList.back().front()->x = c;
            dx = dy = 0;
            read    = false;

            // scannt Zeile nach "[n].\n" index
        } else
            read = sscanf(line, "%u.\n", &c) == 1;
    }

    return false;
}

void testGardens(Rect **gds) {
    uint i, j, lx = 0, n = 2 + log2(opt->i), maxw = 0, maxy = 0;

    while (--n) {
        maxw = 0;

        //  maxw berechnen
        for (i = 0; i < n; i++) maxw += gds[i]->w;
        if (maxw < minw) maxw = minw;
        // printf("(%u,%u),", n,  maxw);

        // Positionen resetten
        for (i = 0; i < opt->i; i++) {
            gds[i]->x = 0;
            gds[i]->y = 0;
        }

        for (i = 0; i < opt->i; i++) {
            bool coll;
            do {
                coll = false;

                /*if (lx + gds[i]->x + gds[i]->w >  maxw) {
                    gds[i]->x = 0;
                    gds[i]->y++;
                } else
                    gds[i]->x = lx;*/

                for (j = 0; j < i; j++) {
                    if ((gds[i]->x < gds[j]->x + gds[j]->w) &&
                        (gds[i]->y < gds[j]->y + gds[j]->h) &&
                        (gds[i]->y + gds[i]->h > gds[j]->y) &&
                        (gds[i]->x + gds[i]->w > gds[j]->x)) {
                        gds[i]->x = gds[j]->x + gds[j]->w;

                        if (lx + gds[i]->x + gds[i]->w > maxw) {
                            gds[i]->x = 0;
                            gds[i]->y++;
                        }
                        coll = true;
                        continue;
                    }
                }
            } while (coll);

            if (gds[i]->y + gds[i]->h > maxy) maxy = gds[i]->y + gds[i]->h;
            // lx = gds[i]->x + gds[i]->w;
        }

        if (!minA || (maxy * maxw != 0 && maxy * maxw < minA)) {
            minA = maxy * maxw;
            maxW = maxw;
            maxH = maxy;
            for (i = 0; i < opt->i; i++) {
                // memcpy(order + i, gds[i], sizeof(Rect));
                order[i]->assign(gds[i]);
            }
        }

        //* output
        if (debug) {
            uint x, y, k, out[maxy][maxw];
            for (y = 0; y < maxy; y++)
                for (x = 0; x < maxw; x++) out[y][x] = 0;

            for (k = 0; k < opt->i; k++) {
                for (y = 0; y < gds[k]->h; y++)
                    for (x = 0; x < gds[k]->w; x++)
                        out[gds[k]->y + y][gds[k]->x + x] = gds[k]->i + 1;
            }

            for (y = 0; y < maxy; y++) {
                for (x = 0; x < maxw; x++) {
                    if (out[y][x] > 10) {
                        printf("\033[30;%um%u", out[y][x] % 9 + 99, out[y][x]);
                    } else if (out[y][x]) {
                        printf(
                            "\033[30;%um%u%u", out[y][x] % 9 + 99, out[y][x],
                            out[y][x]);
                    } else
                        printf("\033[0;90m00");
                }
                printf("\n");
            }
            printf("\n\033[0;37m");
        }
    }
    //*/
}



inline void swap(Rect **c, uint a, uint b) {
    Rect *tmp = c[a];
    c[a]      = c[b];
    c[b]      = tmp;
}

void permut(Rect **r, uint end) {
    if (end == 0) {
        testGardens(r);
    } else {
        permut(r, end - 1);
        uint i;

        for (i = 0; i < end; i++) {
            // nicht tauschen und permutieren wenn Größe übereinstimmt
            if (r[i]->w != r[end]->w || r[i]->h != r[end]->h) {
                swap(r, i, end);
                permut(r, end - 1);
                swap(r, i, end);
            }
        }
    }
}

int main(int argc, const char *argv[]) {
    FILE *fp = NULL;
    uint i;

    // Argumente einlesen
    for (i = 1; i < (uint)argc; i++) {
        if (!strcmp(argv[i], "--debug")) {
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

    // Default Datei öffnen falls nötig
    if (fp == NULL && tryOpen("res/schrebergaerten.txt", fp)) goto error;

    // Datei Einlesen & Parsen
    if (initSchrebergaerten(fp)) {
        error("initialization failed");
        goto error;
    }
    fclose(fp);

    for (auto &gardens: gardenList) {
        i = 0;

        opt = gardens.back();
        gardens.pop_back();

        // wende ggT an
        opt->w /= opt->x;
        opt->h /= opt->y;

        // besetze Speicher für Reihenfolge
        Rect *_order[opt->i];

        // resette minimale Fläche
        minA = 0;

        // Speichere Zeiger auf Gärten in Array
        Rect *garden[opt->i];
        order = _order;

        for (Rect *&rect: gardens) {
            *order++    = new Rect(0, 0, 0, 0, i);
            garden[i++] = rect;
            // wende ggT an
            rect->w /= opt->x;
            rect->h /= opt->y;
            if (minw < rect->w) minw = rect->w;
        }

        printf("\n-- %i: ---------------------\n", garden[0]->x);
        // teste alle Permutationen
        order = _order;
        permut(garden, opt->i - 1);

        printf(
            "min: %u x %u = %u\n", maxW * opt->x, maxH * opt->y,
            minA * opt->x * opt->y);

        uint x, y, out[maxH][maxW];
        for (y = 0; y < maxH; y++)
            for (x = 0; x < maxW; x++) out[y][x] = 0;

        for (i = 0; i < opt->i; i++) {
            for (y = 0; y < order[i]->h; y++)
                for (x = 0; x < order[i]->w; x++)
                    out[order[i]->y + y][order[i]->x + x] = order[i]->i + 1;

            delete order[i];
        }

        for (y = 0; y < maxH; y++) {
            for (x = 0; x < maxW; x++) {
                if (out[y][x]) {
                    printf("\033[30;%um%2u", out[y][x] % 9 + 99, out[y][x]);
                } else
                    printf("\033[0;90m 0");
            }
            printf("\033[0;37m\n");
        }
    }

    freeSchrebergaerten();
    return 0;

error:
    fclose(fp);
    return 1;
}

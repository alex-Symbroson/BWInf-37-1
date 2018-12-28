
/*
 *
 */

// Includes
#include "../base/base.hpp"

#include <vector>

const char* helpStr =
    "Usage: %s [FILE] [OPTIONS]\n"
    "Uses FILE as input (defaults to \"res/superstar1.txt\")\n"
    "\nOptions:\n"
    "  --help        this help\n";



#define FOLLOW_NOP 0 // folgt nicht
#define FOLLOW_UKN 1 // nicht abgefragt
#define FOLLOW_YES 2 // folgt



struct User {
    char* name;            // Name
    uint id;               // ID = users-Index
    bool star;             // Status: kann Superstar sein
    vector<User*> follows; // Folgt-Liste
};



vector<User> users; // User-Liste

uint count = 0, // gesamt User-Anzahl
    stars  = 0, // verbleibende Superstars
    cost   = 0, // entstandene Kosten
    *follow;    // gespeicherte Abfragen



// sucht User per Namen und gibt einen Zeiger zurück
User* findUser(char* name) {
    for (User& u: users)
        if (!strcmp(name, u.name)) return &u;
    error("couldn't find user '%s'\n", name);
    return NULL;
}



void freeSuperstar() {
    for (User& u: users) free(u.name);
    users.clear();
}

bool initSuperstar(FILE* fp) {
    char *line = NULL, *pch;
    uint read;
    size_t len = 0;

    // lese Namenliste
    read = getline(&line, &len, fp);
    if (read == (uint)-1) {
        error("Error reading file");
        return true;
    }

    char buffer[read];
    strncpy(buffer, line, read);

    // parse Namenliste
    pch = strtok(buffer, " \n");
    while (pch != NULL) {
        if (isalpha(*pch)) {
            users.push_back({.name    = strdup(pch),
                             .id      = count,
                             .star    = true,
                             .follows = {}});
            count++;
        }
        pch = strtok(NULL, " \n");
    }

    // lese Folgebeziehungen
    User* cur = NULL;
    while ((read = getline(&line, &len, fp)) != (uint)-1) {
        if (!(pch = strtok(line, " \n"))) continue;
        if (cur == NULL || strcmp(pch, cur->name)) cur = findUser(pch);
        cur->follows.push_back(findUser(strtok(NULL, " \n")));
    }

    free(line);
    return false;
}


void printReq(bool res, int paid, bool nobrk) {
    printf("  %s%5i", res ? " true" : "false", paid);
    if (nobrk)
        printf(" | ");
    else
        printf(" |%4i\n", cost);
}

bool follows(User& a, User& b, bool nobrk) {
    printf("%4i %4i ", a.id, b.id);
    uint* folw = follow + (a.id * count + b.id);

    switch (follow[a.id * count + b.id]) {
        case FOLLOW_NOP: printReq(false, 0, nobrk); return false;
        case FOLLOW_YES: printReq(true, 0, nobrk); return true;

        default:
            cost++;
            // suche Übereinstimmung
            for (User* u: a.follows) {
                if (u == &b) {
                    printReq(true, 1, nobrk);
                    *folw = FOLLOW_YES;
                    if (a.star) {
                        a.star = 0;
                        stars--;
                    }
                    return true;
                }
            }

            printReq(false, 1, nobrk);
            *folw = FOLLOW_NOP;
            if (b.star) {
                b.star = 0;
                stars--;
            }
            return false;
    }
}

int main(int argc, const char* argv[]) {
    FILE* fp = NULL;
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
    if (fp == NULL && tryOpen("res/superstar1.txt", fp)) {
        error("no input file");
        return 1;
    }

    // Datei einlesen
    if (initSuperstar(fp)) {
        fclose(fp);
        error("initialization failed");
        return 1;
    }

    // Namen mit ID ausgeben
    printf("\nNames:");
    for (User& a: users) printf(" %i:%s", a.id, a.name);
    printf("\n\nStar User Follows Cost | Sum\n");

    uint j, _follow[count * count];

    stars  = count;
    follow = _follow;

    // resette Folgebeziehungen
    for (i = 0; i < count; i++)
        for (j = 0; j < count; j++) follow[i * count + j] = FOLLOW_UKN;

    // schließe User als Superstar aus
    User *first = &users[0], *second;
    while (stars > 1) {
        second = NULL;
        // wähle zwei verbleibende Stars aus
        for (User& a: users) {
            if (a.star && &a != first) {
                second = &a;
                break;
            }
        }

        // speichere verbleibenden Star in first
        if (follows(*first, *second, false)) first = second;
    }

    printf(
        "\n\033[0;33mSuspected star: %s\033[0;37m\n\nStar User Follows Cost | "
        "Star User Follows Cost | Sum\n",
        first->name);

    // validiere Star
    for (User& b: users)
        if (&b != first &&
            (follows(*first, b, true) || !follows(b, *first, false))) {
            printf(
                "\n\n\033[0;33mTermination due to (%i:%s) (%i:%s)\033[0;31m",
                first->id, first->name, b.id, b.name);
            break;
        }


    // Ausgabe
    if (stars == 1)
        printf("\n\033[0;32m%s is the superstar!\033[0;37m\n", first->name);
    else
        printf("\n\033[0;31mThere is no superstar in this group.\033[0;37m\n");

    printf(
        "\nPersons:%5i\nPrice:  %5i€\n"
        "\nEstimation (on success):\n Worst:  %4i€\n Best:   %4i€\n",
        count, cost, 3 * (count - 1), 2 * (count - 1));


    freeSuperstar();
    fclose(fp);
}

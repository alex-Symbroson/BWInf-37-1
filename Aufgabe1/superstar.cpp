
/*
 *
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

using namespace std;

#define error(a, ...) \
    fprintf(stderr, "\033[0;33m" a "\033[0;37m\n", ##__VA_ARGS__)

#define FOLLOW_NOP 0
#define FOLLOW_UKN 1
#define FOLLOW_YES 2

struct User {
    char* name;
    uint16_t star, id;
    vector<User*> follows;
};
vector<User> users;
uint16_t count = 0, stars = 0, cost = 0;
uint8_t* follow;

bool tryOpen(const char* name, FILE*& fp) {
    fp = fopen(name, "r");
    if (fp == NULL) {
        error("Error opening '%s'", name);
        return true;
    }
    return false;
}

User* findUser(char* name) {
    for (User& u: users)
        if (!strcmp(name, u.name)) return &u;
    printf("couldn't find user '%s'\n", name);
    return NULL;
}

bool initUsers(FILE* fp) {
    char *line = NULL, *pch;
    ssize_t read;
    size_t len = 0;

    // lese Namenliste
    read = getline(&line, &len, fp);
    if (read == -1) {
        error("Error reading file");
        return true;
    }

    char buffer[read];
    strncpy(buffer, line, read);

    // parse Namenliste
    pch = strtok(buffer, " \n");
    while (pch != NULL) {
        if (isalpha(*pch)) {
            users.push_back({.name = strdup(pch), .star = 1, .id = count});
            count++;
        }
        pch = strtok(NULL, " \n");
    }

    // lese Folgebeziehungen
    User* cur = NULL;
    while ((read = getline(&line, &len, fp)) != -1) {
        if (!(pch = strtok(line, " \n"))) continue;
        if (cur == NULL || strcmp(pch, cur->name)) cur = findUser(pch);
        cur->follows.push_back(findUser(strtok(NULL, " \n")));
    }

    free(line);
    return false;
}

void freeUsers() {
    for (User& u: users) free(u.name);
    users.clear();
}

bool follows(User& a, User& b, bool nobrk) {
    printf("%4i %4i%4i ", stars, a.id, b.id);
    uint8_t* folw = follow + (a.id * count + b.id);

    switch (follow[a.id * count + b.id]) {
        case FOLLOW_NOP: printf(" nein    * |%c", "\n "[nobrk]); return false;
        case FOLLOW_YES: printf("   ja    * |%c", "\n "[nobrk]); return true;

        default:
            cost++;
            for (User* u: a.follows) {
                if (u == &b) {
                    printf("   ja%5i |%c", cost, "\n "[nobrk]);
                    *folw = FOLLOW_YES;
                    if (b.star) b.star++;
                    if (a.star) {
                        a.star = 0;
                        stars--;
                    }
                    return true;
                }
            }

            printf(" nein%5i |%c", cost, "\n "[nobrk]);
            *folw = FOLLOW_NOP;
            if (a.star) a.star++;
            if (b.star) {
                b.star = 0;
                stars--;
            }
            return false;
    }
}

int main(int argc, const char* argv[]) {
    FILE* fp = NULL;

    // Datei öffnen
    if (argc > 1) tryOpen(argv[1], fp);
    if (fp == NULL && tryOpen("res/superstar1.txt", fp)) return true;

    // Datei einlesen
    if (initUsers(fp)) {
        fclose(fp);
        error("initialization failed");
        return 1;
    }

    printf("\nNamen:\n");
    for (User& a: users) printf("(%i:%s)", a.id, a.name);
    printf("\n\nstar   U1  U2 folgt Cost |\n");

    uint8_t _follow[count * count];
    uint16_t i, j;

    stars  = count;
    follow = _follow;

    // resette Folgebeziehungen
    for (i = 0; i < count; i++)
        for (j = 0; j < count; j++) follow[i * count + j] = FOLLOW_UKN;

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
        "\nvermuteter Star: %s\n\nstar   U1  U2 folgt Cost | star   U1  U2 "
        "folgt Cost |\n",
        first->name);

    // validiere Star
    for (User& b: users)
        if (&b != first &&
            (follows(*first, b, true) || !follows(b, *first, false))) {
            printf(
                "\n\nabbruch wegen (%i:%s) (%i:%s)", first->id, first->name,
                b.id, b.name);
            break;
        }

    // Ausgabe
    if (stars == 1)
        printf("\n\033[0;32m%s ist der Superstar!\033[0;37m\n", first->name);
    else
        printf(
            "\n\033[0;33mEs gibt keinen Superstar in dieser "
            "Gruppe.\033[0;37m\n");

    printf("\nPersonen:%4i\nPreis:   %4i€\n", count, cost);
    printf(
        "\nSchätzung (bei Erfolg):\n Worst:  %4i€\n Best:   %4i€\n",
        3 * (count - 1), 2 * (count - 1));
    freeUsers();
    fclose(fp);
}

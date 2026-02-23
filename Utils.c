#include "utils.h"
#include <stdio.h>      // Pour printf, getchar, fgets, EOF, sprintf
#include <ctype.h>
#include <string.h>
#include <time.h>       // Pour time_t, time, localtime, struct tm

// ---------------------------------------------
// Vider le buffer stdin
// ---------------------------------------------
void clearBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ---------------------------------------------
// Supprimer le '\n' d’une chaîne
// ---------------------------------------------
void nettoyerString(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

// ---------------------------------------------
// Lire une string en sécurité
// ---------------------------------------------
void lireString(char *str, int taille) {
    if (fgets(str, taille, stdin) != NULL) {
        size_t len = strlen(str);
        if(len > 0 && str[len-1] == '\n') str[len-1] = '\0';
    }
}

// ---------------------------------------------
// Vérifier si une string contient uniquement des chiffres
// ---------------------------------------------
int isNumber(const char *str) {
    int i;
    if (str[0] == '\0') return 0;
    for (i = 0; str[i]; i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

// ---------------------------------------------
// Vérification du numéro téléphone (10 chiffres marocains)
// ---------------------------------------------
int numeroValide(const char *numero) {
    if (strlen(numero) != 10) return 0;
    return isNumber(numero);
}

// ---------------------------------------------
// Vérification Email simple
// ---------------------------------------------
int isEmail(const char *email) {
    int at = -1, point = -1;
    int i;
    for (i = 0; email[i]; i++) {
        if (email[i] == '@') at = i;
        if (email[i] == '.') point = i;
    }

    if (at < 1) return 0;                  // @ ne doit pas être premier
    if (point < at + 2) return 0;          // . doit venir après @
    if (point == strlen(email) - 1) return 0; // . ne peut pas être le dernier

    return 1;
}

// ---------------------------------------------
// Convertir string en majuscules
// ---------------------------------------------
void toUpperCase(char *str) {
    int i;
    for (i = 0; str[i]; i++)
        str[i] = toupper(str[i]);
}

// ---------------------------------------------
// Pause console
// ---------------------------------------------
void pauseConsole() {
    printf("\nAppuyez sur Entree pour continuer...");
    clearBuffer();
}

// ---------------------------------------------
// Obtenir la date et l'heure actuelles
// ---------------------------------------------
void obtenirDateActuelle(char *date, char *heure) {
    time_t t = time(NULL);
    struct tm *tmInfo = localtime(&t);

    sprintf(date, "%02d/%02d/%04d",
            tmInfo->tm_mday,
            tmInfo->tm_mon + 1,
            tmInfo->tm_year + 1900);

    sprintf(heure, "%02d:%02d:%02d",
            tmInfo->tm_hour,
            tmInfo->tm_min,
            tmInfo->tm_sec);
}
void calculerDateLivraison(char *buffer, int jours_delai) {
    time_t t = time(NULL);

    // Ajouter les jours de délai (en secondes)
    t += jours_delai * 24 * 3600;

    struct tm *tmInfo = localtime(&t);

    sprintf(buffer, "%02d/%02d/%04d",
            tmInfo->tm_mday,
            tmInfo->tm_mon + 1,
            tmInfo->tm_year + 1900);
}

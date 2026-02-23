#ifndef UTILS_H
#define UTILS_H

void clearBuffer();
void nettoyerString(char *str);
void lireString(char *str, int taille);
int isNumber(const char *str);
int numeroValide(const char *numero);
int isEmail(const char *email);
void toUpperCase(char *str);
void pauseConsole();

void obtenirDateActuelle(char *date, char *heure);
void calculerDateLivraison(char *buffer, int jours_delai);  // Point-virgule manquant ajouté

#endif

#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

typedef struct {
    int id;
    char nom[30];
    char prenom[30];
    char telephone[20];
     int achats[100];
    int nbAchats;
} Client;

// Fonctions de gestion des clients
void chargerClients();
void sauvegarderClients();
void ajouterClient();
void afficherClients();

void afficherHistorique(int id);
void rechercherParPrefixe(const char *prefixe);
int rechercherClient(int id);
void modifierClient(int id);
void supprimerClient(int id);
void gestionClientMenu();
#endif /* CLIENT_H_INCLUDED */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Client.h"
#include "utils.h"
#include "vente.h"
#include "ui.h"
#define FICHIER "clients.txt"
#define MAX_CLIENTS 1000
#define MAX_VENTES 1000

Client clients[MAX_CLIENTS];
int nbClients = 0;
extern Vente ventes[MAX_VENTES];
extern int nbVentes;

// ------------------------------------------------------
// Charger les clients depuis le fichier
// ------------------------------------------------------
void chargerClients() {
    FILE *f = fopen(FICHIER, "r");
    if (!f) {
        printf("Fichier clients.txt inexistant, démarrage avec 0 clients.\n");
        nbClients = 0;
        return;
    }

    if (fscanf(f, "%d", &nbClients) != 1) {
        printf("Fichier clients.txt corrompu.\n");
        nbClients = 0;
        fclose(f);
        return;
    }

    for (int i = 0; i < nbClients; i++) {
        if (fscanf(f, "%d %s %s %s %d",
                   &clients[i].id,
                   clients[i].nom,
                   clients[i].prenom,
                   clients[i].telephone,
                   &clients[i].nbAchats) != 5) {
            printf("Erreur de lecture du client #%d\n", i + 1);
            nbClients = i;
            break;
        }

        for (int j = 0; j < clients[i].nbAchats; j++) {
            if (fscanf(f, "%d", &clients[i].achats[j]) != 1) {
                clients[i].nbAchats = j;
                break;
            }
        }
    }

    fclose(f);
}

// ------------------------------------------------------
// Sauvegarder les clients dans le fichier
// ------------------------------------------------------
void sauvegarderClients() {
    FILE *f = fopen(FICHIER, "w");
    if (!f) {
        printf("Erreur de sauvegarde dans %s.\n", FICHIER);
        return;
    }

    fprintf(f, "%d\n", nbClients);

    for (int i = 0; i < nbClients; i++) {
        fprintf(f, "%d %s %s %s %d",
                clients[i].id,
                clients[i].nom,
                clients[i].prenom,
                clients[i].telephone,
                clients[i].nbAchats);

        for (int j = 0; j < clients[i].nbAchats; j++)
            fprintf(f, " %d", clients[i].achats[j]);

        fprintf(f, "\n");
    }

    fclose(f);
}

// ------------------------------------------------------
// Rechercher par ID
// ------------------------------------------------------
int rechercherClient(int id) {
    for (int i = 0; i < nbClients; i++)
        if (clients[i].id == id)
            return i;
    return -1;
}

// ------------------------------------------------------
// Afficher Clients avec tableau
// ------------------------------------------------------
void afficherClients() {
    if (nbClients == 0) {
        ui_warning("Aucun client a afficher !");
        return;
    }

    const char *columns[] = {"ID", "Nom", "Prenom", "Telephone", "Nb Achats"};
    int widths[] = {5, 20, 20, 15, 10};

    ui_table_header(columns, 5, widths);

    char bufId[10], bufAchats[10];
    for (int i = 0; i < nbClients; i++) {
        char *values[5];
        snprintf(bufId, sizeof(bufId), "%d", clients[i].id);
        snprintf(bufAchats, sizeof(bufAchats), "%d", clients[i].nbAchats);

        values[0] = bufId;
        values[1] = clients[i].nom;
        values[2] = clients[i].prenom;
        values[3] = clients[i].telephone;
        values[4] = bufAchats;

        ui_table_row(values, 5, widths);
    }
    ui_table_separator(widths, 5);
}

// ------------------------------------------------------
// Ajouter Client
// ------------------------------------------------------
void ajouterClient() {
    if (nbClients >= MAX_CLIENTS) {
        ui_error("Limite maximale de clients atteinte !");
        return;
    }

    Client c;
    ui_header("Ajout d'un Client");

    ui_input_prompt("ID");
    scanf("%d", &c.id); getchar();

    if (rechercherClient(c.id) != -1) {
        ui_error("Cet ID existe deja !");
        return;
    }

    ui_input_prompt("Nom");
    lireString(c.nom, 100);

    ui_input_prompt("Prenom");
    lireString(c.prenom, 100);

    do {
        ui_input_prompt("Telephone (10 chiffres)");
        lireString(c.telephone, 20);
    } while (!numeroValide(c.telephone));

    c.nbAchats = 0;

    clients[nbClients++] = c;
    sauvegarderClients();
}

// ------------------------------------------------------
// Modifier Client
// ------------------------------------------------------
void modifierClient(int id) {
    int pos = rechercherClient(id);
    if (pos == -1) {
        ui_error("Client introuvable !");
        return;
    }

    char tmp[100];

    ui_input_prompt("Nouveau nom");
    lireString(tmp, 100);
    if (strlen(tmp) > 0) strcpy(clients[pos].nom, tmp);

    ui_input_prompt("Nouveau prenom");
    lireString(tmp, 100);
    if (strlen(tmp) > 0) strcpy(clients[pos].prenom, tmp);

    do {
        ui_input_prompt("Nouveau telephone");
        lireString(tmp, 20);
        if (strlen(tmp) == 0) break;
    } while (!numeroValide(tmp));

    if (strlen(tmp) > 0) strcpy(clients[pos].telephone, tmp);

    sauvegarderClients();
}

// ------------------------------------------------------
// Supprimer Client
// ------------------------------------------------------
void supprimerClient(int id) {
    int pos = rechercherClient(id);
    if (pos == -1) {
        ui_error("Client introuvable !");
        return;
    }

    for (int i = pos; i < nbClients - 1; i++)
        clients[i] = clients[i + 1];

    nbClients--;
    sauvegarderClients();
}

// ------------------------------------------------------
// Menu Gestion Clients
// ------------------------------------------------------
void gestionClientsMenu() {
    int choix;
    const char *options[] = {
        "Afficher Clients",
        "Ajouter Client",
        "Modifier Client",
        "Supprimer Client",
        "Retour"
    };

    do {
        ui_clear();
        ui_logo();
        ui_title("Gestion Clients");

        choix = ui_menu("Menu Clients", options, 5);

        switch (choix) {
            case 1:
                afficherClients();
                ui_pause();
                break;
            case 2:
                ajouterClient();
                ui_success("Client ajoute avec succes !");
                ui_pause();
                break;
            case 3: {
                int id;
                ui_input_prompt("ID du client a modifier");
                scanf("%d", &id); getchar();
                modifierClient(id);
                ui_success("Client modifie avec succes !");
                ui_pause();
                break;
            }
            case 4: {
                int id;
                ui_input_prompt("ID du client a supprimer");
                scanf("%d", &id); getchar();
                supprimerClient(id);
                ui_success("Client supprime avec succes !");
                ui_pause();
                break;
            }
            case 5:
                break;
            default:
                ui_warning("Choix invalide !");
                ui_pause();
        }

    } while (choix != 5);
}

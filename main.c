/*
 * ======================================================================================
 * MAIN.C - GESTION PHARMACIE AVEC UI AMELIOREE
 * ======================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Headers du projet existant */
#include "Utilisateur.h"
#include "Client.h"
#include "medicaments.h"
#include "fournisseurs.h"
#include "Vente.h"
#include "Facture.h"
#include "utils.h"
#include "Menu.h"
#include "ui.h"
#include "commande.h"
/* Variables externes */
extern User users[];
extern int usersCount;
extern User loggedUser;

extern Client clients[];
extern int nbClients;

extern Medicament med[];
extern int nbMed;

extern Fournisseur fournisseurs[];
extern int nbFournisseurs;

extern Vente ventes[];
extern int nbVentes;

extern Facture factures[];
extern int nbFactures;

/* ======================================================================================
   CHARGEMENT DES DONNEES
   ====================================================================================== */
void loadAllData() {
    ui_clear();
    ui_logo();
    ui_title("CHARGEMENT DES DONNEES");

    ui_progress_bar(0, 6, "Initialisation");
    loadUsersFromFile();
    ui_sleep_ms(200);

    ui_progress_bar(1, 6, "Clients");
    chargerClients();
    ui_sleep_ms(200);

    ui_progress_bar(2, 6, "Medicaments");
    chargerMed();
    ui_sleep_ms(200);

    ui_progress_bar(3, 6, "Fournisseurs");
    chargerF();
    ui_sleep_ms(200);

    ui_progress_bar(4, 6, "Ventes");
    chargerVentesTexte();
    ui_sleep_ms(200);

    ui_progress_bar(5, 6, "Factures");
chargerFacturesTexte();// après ventes et fournisseurs
    ui_sleep_ms(200);
 ui_progress_bar(5, 6, "Commandes");
chargerCommandes();// après ventes et fournisseurs
    ui_sleep_ms(200);
    ui_progress_bar(6, 6, "Termine");
    ui_success("Toutes les donnees ont ete chargees !");
    ui_sleep_ms(1000);
}

/* ======================================================================================
   ECRAN D'ACCUEIL
   ====================================================================================== */
void showWelcomeScreen() {
    ui_clear();
    ui_logo();
    ui_header("SYSTEME DE GESTION PHARMACEUTIQUE");
    ui_center_text("Version 2.0 - Interface Professionnelle", 80);
    ui_center_text("Gestion complete : Clients, Stock, Ventes, Factures", 80);
    ui_pause();
}

/* ======================================================================================
   LOGIN
   ====================================================================================== */
int performLogin() {
    ui_clear();
    ui_logo();
    ui_title("CONNEXION");

    char username[50], password[50];
    ui_input_prompt("Username");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = 0;

    ui_input_prompt("Password");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0;

    for(int i = 0; i < usersCount; i++) {
        if(strcmp(users[i].username, username) == 0 &&
           strcmp(users[i].password, password) == 0) {
            loggedUser = users[i];
            return 1;
        }
    }
    return 0;
}

/* ======================================================================================
   MAIN
   ====================================================================================== */
int main() {
    ui_init();          // Active couleurs ANSI
    loadAllData();
    initDefaultAdmin();
    showWelcomeScreen();

    while(1) {
        if(performLogin()) {
            ui_success("Connexion reussie !");
            ui_sleep_ms(1000);
            if(loggedUser.role == 1) adminMenu();
            else simpleUserMenu();
        } else {
            ui_error("Identifiants incorrects.");
            ui_sleep_ms(1500);
        }
    }

    return 0;
}

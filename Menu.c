/* ===== MENU.C ===== */
#include <stdio.h>
#include <string.h>
#include "Menu.h"
#include "Client.h"
#include "fournisseurs.h"
#include "Utilisateur.h"
#include "medicaments.h"
#include "utils.h"
#include "facture.h"
#include "ui.h"
#include "commande.h"
#define MAX 100
#define MAX_FOURNISSEURS 100

/* ===== Variables externes ===== */
extern User loggedUser;
extern Fournisseur fournisseurs[MAX_FOURNISSEURS];
extern int nbFournisseurs;
extern Medicament med[MAX_MED];
extern int nbMed;

/* ===== MENU ADMIN ===== */
void adminMenu() {
    int choice;
    const char *options[] = {
        "Gestion des Utilisateurs",
        "Gestion des Clients",
        "Gestion des Medicaments",
        "Gestion des Fournisseurs",
        "Gestion des Ventes",
        "Gestion des Factures",
        "Gestion des Commandes Fournisseurs",  // NOUVELLE OPTION
        "Deconnexion"
    };

    do {
        ui_clear();
        ui_logo();
        ui_title("MENU ADMINISTRATEUR");

        char welcome[100];
        sprintf(welcome, "Bienvenue, %s !", loggedUser.username);
        ui_info(welcome);

        choice = ui_menu("Menu Principal", options, 8);  // 8 au lieu de 7

        switch(choice) {
            case 1: gestionUserMenu(); break;
            case 2: gestionClientsMenu(); break;
            case 3: gestionMedicamentsMenu(); break;
            case 4: gestionFournisseurMenu(); break;
            case 5: menuVentes(); break;
            case 6: menuFactures(); break;
            case 7: menuCommandes(); break;  // NOUVEAU CASE
            case 8:
            case 0:
                ui_info("Deconnexion...");
                ui_sleep_ms(1000);
                break;
            default:
                ui_warning("Choix invalide !");
                ui_pause();
        }

    } while(choice != 0 && choice != 8);  // 8 au lieu de 7
}


/* ===== MENU UTILISATEUR SIMPLE ===== */
void simpleUserMenu() {
    int choice;
    const char *options[] = {
        "Gestion des Ventes",
        "Consulter Stock Medicaments",
        "Consulter Clients",
        "Commander Medicaments Manquants",  // NOUVELLE OPTION
        "Deconnexion"
    };

    do {
        ui_clear();
        ui_logo();
        ui_title("MENU UTILISATEUR");

        char welcome[100];
        sprintf(welcome, "Bienvenue, %s !", loggedUser.username);
        ui_info(welcome);

        choice = ui_menu("Menu Principal", options, 5);

        switch(choice) {
            case 1: menuVentes(); break;
            case 2: afficherTousMed(); ui_pause(); break;
            case 3:afficherClients(); ui_pause(); break;
            case 4:
                passerCommandeMedicamentManquant(0, NULL);
                break;
            case 5:
            case 0:
                ui_info("Deconnexion...");
                ui_sleep_ms(1000);
                break;
            default:
                ui_warning("Choix invalide !");
                ui_pause();
        }

    } while(choice != 0 && choice != 5);
}

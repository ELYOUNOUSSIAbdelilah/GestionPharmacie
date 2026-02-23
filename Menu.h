#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED
#include "Utilisateur.h" // Pour utiliser la structure User

// ======== PROTOTYPES DES MENUS ========

// Menu Admin (gestion complète du système)
void adminMenu();

// Menu Simple User (vérifier médicaments uniquement)
void simpleUserMenu();
void gestionClientsMenu();      // Définie dans Client.c
void gestionUserMenu();          // Définie dans Utilisateur.c
void gestionFournisseurMenu();   // Définie dans fournisseurs.c
void gestionMedicamentsMenu();   // Définie dans medicaments.c
void menuVentes();               // Définie dans vente.c
void menuFactures();

#endif

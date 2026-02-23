#ifndef VENTE_H
#define VENTE_H

#include "Client.h"

#define MAX_LIGNES_VENTE 20

// Structure LigneVente et Vente
typedef struct {
    int   idMedicament;
    int   quantite;
    float prixUnitaire;
    float sousTotal;
} LigneVente;

typedef struct {
    int   id;
    Client client;
    char  date[11];
    char  heure[9];

    LigneVente lignes[MAX_LIGNES_VENTE];
    int   nbLignes;

    float montantTotal;
    float montantPaye;
    float monnaieRendue;
    char  modePaiement[20];
} Vente;

// Prototypes des fonctions
void menuVentes();
Vente effectuerNouvelleVente();
Vente effectuerNouvelleVenteAvecCommande();  // ⭐ AJOUTER CETTE LIGNE
void afficherRecu(Vente *v);
void rechercherVente();
void afficherVentesParDate();
void afficherVentesParClient();
void afficherToutesLesVentes();
void statistiquesVentes();
void modifierVente();
void supprimerVente();
void sauvegarderVentes();
void chargerVentesTexte();
Vente* rechercherVente_parID(int id);

#endif // VENTE_H

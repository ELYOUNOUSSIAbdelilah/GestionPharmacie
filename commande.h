#ifndef COMMANDE_H
#define COMMANDE_H

#include "medicaments.h"
#include "fournisseurs.h"

#define MAX_COMMANDES 500
#define MAX_LIGNES_COMMANDE 30

/* États possibles d'une commande */
typedef enum {
    COMMANDE_EN_ATTENTE,
    COMMANDE_VALIDEE,
    COMMANDE_ENVOYEE,
    COMMANDE_RECUE,
    COMMANDE_ANNULEE
} StatutCommande;

/* Ligne de commande */
typedef struct {
    int idMedicament;           // 0 si nouveau médicament
    char nomMedicament[100];    // Nom du médicament commandé
    char formePharma[50];       // Forme pharmaceutique
    char dosage[50];            // Dosage
    int quantite;               // Quantité commandée
    float prixUnitaire;         // Prix unitaire
    float sousTotal;            // Sous-total
} LigneCommande;

/* Structure Commande */
typedef struct {
    int id;                     // ID unique de la commande
    char numero[30];            // Numéro de commande (ex: CMD-2025-001)
    char dateCommande[11];      // Date de création
    char dateLivraisonPrevue[11]; // Date de livraison prévue
    char dateLivraisonReelle[11]; // Date de livraison réelle

    int fournisseurId;          // ID du fournisseur
    char fournisseurNom[100];   // Nom du fournisseur
    char fournisseurContact[100]; // Contact du fournisseur

    LigneCommande lignes[MAX_LIGNES_COMMANDE];
    int nbLignes;

    float montantTotal;         // Montant total de la commande
    StatutCommande statut;      // Statut de la commande
    char remarques[200];        // Remarques éventuelles

    int clientId;               // ID du client demandeur (0 si commande générale)
    char clientNom[60];         // Nom du client demandeur
} Commande;

/* Variables globales */
extern Commande commandes[MAX_COMMANDES];
extern int nbCommandes;

/* Fonctions principales */
void menuCommandes();
void passerCommandeMedicamentManquant(int clientId, const char *nomClient);
void afficherToutesCommandes();
void afficherCommandesParStatut(StatutCommande statut);
void validerReceptionCommande();
void annulerCommande();
void rechercherCommande();
Commande* rechercherCommandeParId(int id);

/* Fonctions de sauvegarde */
void sauvegarderCommandes();
void chargerCommandes();

/* Fonctions utilitaires */
void genererNumeroCommande(char *numero);
const char* statutToString(StatutCommande statut);
StatutCommande stringToStatut(const char *str);
void afficherDetailsCommande(Commande *cmd);

#endif // COMMANDE_H

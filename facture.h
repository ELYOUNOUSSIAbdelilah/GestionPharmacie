#ifndef FACTURE_H
#define FACTURE_H

#include "Vente.h"
#include "fournisseurs.h"

#define MAX_FACTURES 1000
#define MAX_LIGNES_FACTURE 20

typedef struct {
    int idMedicament;
    int quantite;
    float prixUnitaire;
    float sousTotal;
} LigneFacture;

typedef struct {
    int id;
    char type[20];              // "CLIENT" ou "FOURNISSEUR"
    char numero[30];            // Numéro de facture (ex: FAC-2025-001)
    char date[11];              // Date de facturation
    char dateEcheance[11];      // Date d'échéance

    // Informations client/fournisseur
    union {
        struct {
            int clientId;
            char clientNom[30];
            char clientPrenom[30];
            char clientTelephone[20];
        };
        struct {
            int fournisseurId;
            char fournisseurNom[100];
            char fournisseurContact[60];
            char fournisseurTelephone[20];
        };
    };

    // Lignes de la facture
    LigneFacture lignes[MAX_LIGNES_FACTURE];
    int nbLignes;

    // Montants
    float montantHT;            // Hors taxe
    float tauxTVA;              // Taux de TVA (%)
    float montantTVA;           // Montant de la TVA
    float montantTTC;           // Toutes taxes comprises
    float montantPaye;          // Montant déjà payé
    float montantRestant;       // Reste à payer

    // Statut
    char statut[20];            // "BROUILLON", "VALIDEE", "PAYEE", "ANNULEE"
    char modePaiement[20];      // "ESPECES", "CARTE", "CHEQUE", "VIREMENT"
    char remarques[200];        // Notes supplémentaires

} Facture;

/* Variables globales */
extern Facture factures[MAX_FACTURES];
extern int nbFactures;

/* Prototypes des fonctions */
void menuFactures();
void chargerFacturesTexte();
void sauvegarderFacturesTexte();

/* Fonctions de gestion des factures */
void creerFactureDepuisVente();
void creerFactureFournisseur();
void afficherFacture(Facture *f);
void afficherFacturesFournisseurs();
void afficherFacturesClients();
void rechercherFacture();
void modifierFacture();
void supprimerFacture();
void marquerFacturePayee();

/* Fonctions de recherche et filtrage */
Facture* rechercherFactureParId(int id);
Facture* rechercherFactureParNumero(const char *numero);
void afficherFacturesParStatut(const char *statut);
void afficherFacturesParDate();
void afficherFacturesParClient();
void afficherFacturesParFournisseur();
void afficherFacturesImpayees();

/* Fonctions statistiques */
void statistiquesFactures();
void afficherChiffreAffaires();
void afficherFacturesEchues();

/* Fonctions utilitaires */
void genererNumeroFacture(char *numero, const char *type);
void calculerMontantsFacture(Facture *f);
void imprimerFacture(Facture *f);

#endif // FACTURE_H

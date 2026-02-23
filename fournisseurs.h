#ifndef FOURNISSEURS_H
#define FOURNISSEURS_H

#include "medicaments.h"

#define MAX_FOURNISSEURS 100
#define MAX_MED_PAR_FOURNISSEUR 50
#define FICHIER_F "fournisseurs.bin"
#define MAX_CHAR 100

typedef struct {
    int fournisseur_id;
    char reference_code[MAX_CHAR];
    char raison_sociale[MAX_CHAR];
    char contact_prenom[MAX_CHAR];
    char contact_nom[MAX_CHAR];
    char poste[MAX_CHAR];
    char telephone[MAX_CHAR];
    char email[MAX_CHAR];
    char site_web[MAX_CHAR];
    char adresse_ligne[MAX_CHAR];
    char ville[MAX_CHAR];
    char province_code[MAX_CHAR];
    char code_postal[MAX_CHAR];
    char pays[MAX_CHAR];
    char num_tva[MAX_CHAR];
    char forme_juridique[MAX_CHAR];
    char banque_nom[MAX_CHAR];
    char banque_rib[MAX_CHAR];
    char iban[MAX_CHAR];
    int delai_livraison_jours;
    int actif;
    char date_inscription[MAX_CHAR];

    int medicaments_ids[MAX_MED_PAR_FOURNISSEUR];
    int nbMedicaments;

} Fournisseur;

/* Variables globales */
extern Fournisseur fournisseurs[MAX_FOURNISSEURS];
extern int nbFournisseurs;

/* Fonctions */
void sauvegarderF();
void chargerF();
void AjouterF();
void afficherF();
void modifierF();
Fournisseur* rechercherF_parID(int id);
void rechercheFparNom(const char *nom);
void filtrerFactifs();
void filtrerFparRegion(const char *region);
void afficherFparPays(const char *pays);
int compterFactifs();
void lierMedAFournisseur(int fournisseurID, int medicamentID);
void afficherMedFour(int fournisseurID);
void gestionFournisseurMenu();
#endif

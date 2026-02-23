#ifndef MEDICAMENTS_H
#define MEDICAMENTS_H

#define MAX_CHAR 100
#define MAX_MED 100

typedef struct {
    int medicament_id;
    char code_barre[MAX_CHAR];
    char nom_commercial[MAX_CHAR];
    char nom_scientifique[MAX_CHAR];
    char categorie[MAX_CHAR];
    char forme_pharmaceutique[MAX_CHAR];
    char dosage[MAX_CHAR];
    char unite_dosage[MAX_CHAR];
    char presentation[MAX_CHAR];
    char fabricant[MAX_CHAR];
    char pays_origine[MAX_CHAR];
    char date_fabrication[MAX_CHAR];
    char date_expiration[MAX_CHAR];
    int stock_actuel;
    int stock_min;
    char emplacement_stock[MAX_CHAR];
    float prix_achat;
    float prix_vente;
    char mode_administration[MAX_CHAR];
    char ordonnance_obligatoire[MAX_CHAR];
} Medicament;

/* Variables globales déclarées dans medicaments.c */
extern Medicament medicaments[MAX_MED];
extern int nbMedicaments;

/* Fonctions */
void chargerMed();
void sauvegarderMed();
int estExiste(int id);
void ajouterMed();
void afficherMed();
void modifierMed();
Medicament* chercherMedParId(int id);
void chercherMedParNom(char *nom_commercial);
void rechercherMedParPrefixe(char *prefix);
void chercherMedParCat(char *cat);
int nombreMedicaments();
int nombreMedParCat(char *cat);
float PrixMoyenAchat();
Medicament MedPlusCher();
Medicament MedMoinsCher();
void gestionMedicamentsMenu();

#endif

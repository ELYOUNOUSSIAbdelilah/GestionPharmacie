#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "facture.h"
#include "Vente.h"
#include "Client.h"
#include "medicaments.h"
#include "fournisseurs.h"
#include "utils.h"
#include "ui.h"
/* Tableaux globaux */
Facture factures[MAX_FACTURES];
int nbFactures = 0;

/* Données externes */
extern Vente ventes[];
extern int nbVentes;
extern Client clients[];
extern int nbClients;
extern Medicament med[];
extern int nbMed;
extern Fournisseur fournisseurs[];
extern int nbFournisseurs;

/* ====================================================================
   SAUVEGARDE ET CHARGEMENT
   ==================================================================== */
void sauvegarderFacturesTexte() {
    FILE *f = fopen("factures.txt", "w");
    if (!f) {
        printf("Erreur lors de l'ouverture du fichier factures.txt\n");
        return;
    }

    fprintf(f, "%d\n", nbFactures);

    for (int i = 0; i < nbFactures; i++) {
        Facture *fact = &factures[i];

        fprintf(f, "%d;%s;%s;%s;%s;",
            fact->id,
            fact->type,
            fact->numero,
            fact->date,
            fact->dateEcheance
        );

        if (strcmp(fact->type, "CLIENT") == 0) {
            fprintf(f, "%d;%s;%s;%s;",
                fact->clientId,
                fact->clientNom,
                fact->clientPrenom,
                fact->clientTelephone
            );
        } else {
            fprintf(f, "%d;%s;%s;%s;",
                fact->fournisseurId,
                fact->fournisseurNom,
                fact->fournisseurContact,
                fact->fournisseurTelephone
            );
        }

        fprintf(f, "%f;%f;%f;%f;%f;%f;%s;%s;%s;%d\n",
            fact->montantHT,
            fact->tauxTVA,
            fact->montantTVA,
            fact->montantTTC,
            fact->montantPaye,
            fact->montantRestant,
            fact->statut,
            fact->modePaiement,
            fact->remarques,
            fact->nbLignes
        );

        for (int j = 0; j < fact->nbLignes; j++) {
            fprintf(f, "%d;%d;%f;%f\n",
                fact->lignes[j].idMedicament,
                fact->lignes[j].quantite,
                fact->lignes[j].prixUnitaire,
                fact->lignes[j].sousTotal
            );
        }
    }

    fclose(f);
}

void chargerFacturesTexte() {
    FILE *f = fopen("factures.txt", "r");
    if (!f) {
        nbFactures = 0;
        return;
    }

    fscanf(f, "%d\n", &nbFactures);

    for (int i = 0; i < nbFactures; i++) {
        Facture *fact = &factures[i];

        fscanf(f, "%d;%19[^;];%29[^;];%10[^;];%10[^;];",
            &fact->id,
            fact->type,
            fact->numero,
            fact->date,
            fact->dateEcheance
        );

        if (strcmp(fact->type, "CLIENT") == 0) {
            fscanf(f, "%d;%29[^;];%29[^;];%19[^;];",
                &fact->clientId,
                fact->clientNom,
                fact->clientPrenom,
                fact->clientTelephone
            );
        } else {
            fscanf(f, "%d;%99[^;];%59[^;];%19[^;];",
                &fact->fournisseurId,
                fact->fournisseurNom,
                fact->fournisseurContact,
                fact->fournisseurTelephone
            );
        }

        fscanf(f, "%f;%f;%f;%f;%f;%f;%19[^;];%19[^;];%199[^;];%d\n",
            &fact->montantHT,
            &fact->tauxTVA,
            &fact->montantTVA,
            &fact->montantTTC,
            &fact->montantPaye,
            &fact->montantRestant,
            fact->statut,
            fact->modePaiement,
            fact->remarques,
            &fact->nbLignes
        );

        for (int j = 0; j < fact->nbLignes; j++) {
            fscanf(f, "%d;%d;%f;%f\n",
                &fact->lignes[j].idMedicament,
                &fact->lignes[j].quantite,
                &fact->lignes[j].prixUnitaire,
                &fact->lignes[j].sousTotal
            );
        }
    }

    fclose(f);
}

/* ====================================================================
   FONCTIONS UTILITAIRES
   ==================================================================== */
void genererNumeroFacture(char *numero, const char *type) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    int compteur = 1;
    for (int i = 0; i < nbFactures; i++) {
        if (strcmp(factures[i].type, type) == 0) {
            compteur++;
        }
    }

    if (strcmp(type, "CLIENT") == 0) {
        sprintf(numero, "FC-%04d-%03d", tm.tm_year + 1900, compteur);
    } else {
        sprintf(numero, "FF-%04d-%03d", tm.tm_year + 1900, compteur);
    }
}

void calculerMontantsFacture(Facture *f) {
    f->montantHT = 0;

    for (int i = 0; i < f->nbLignes; i++) {
        f->montantHT += f->lignes[i].sousTotal;
    }

    f->montantTVA = f->montantHT * (f->tauxTVA / 100.0);
    f->montantTTC = f->montantHT + f->montantTVA;
    f->montantRestant = f->montantTTC - f->montantPaye;
}

/* ====================================================================
   RECHERCHE
   ==================================================================== */
Facture* rechercherFactureParId(int id) {
    for (int i = 0; i < nbFactures; i++) {
        if (factures[i].id == id) {
            return &factures[i];
        }
    }
    return NULL;
}

Facture* rechercherFactureParNumero(const char *numero) {
    for (int i = 0; i < nbFactures; i++) {
        if (strcmp(factures[i].numero, numero) == 0) {
            return &factures[i];
        }
    }
    return NULL;
}

/* ====================================================================
   AFFICHAGE FACTURE
   ==================================================================== */
void afficherFacture(Facture *f) {
    printf("\n");
    printf("===============================================\n");
    printf("            FACTURE %s #%d\n", f->type, f->id);
    printf("===============================================\n");
    printf("Numero : %s\n", f->numero);
    printf("Date : %s\n", f->date);
    printf("Echeance : %s\n", f->dateEcheance);
    printf("Statut : %s\n", f->statut);
    printf("-----------------------------------------------\n");

    if (strcmp(f->type, "CLIENT") == 0) {
        printf("Client : %s %s\n", f->clientNom, f->clientPrenom);
        if (strlen(f->clientTelephone) > 0) {
            printf("Tel : %s\n", f->clientTelephone);
        }
    } else {
        printf("Fournisseur : %s\n", f->fournisseurNom);
        printf("Contact : %s\n", f->fournisseurContact);
        if (strlen(f->fournisseurTelephone) > 0) {
            printf("Tel : %s\n", f->fournisseurTelephone);
        }
    }

    printf("-----------------------------------------------\n");
    printf("%-4s %-25s %5s %8s %10s\n", "ID", "Medicament", "Qte", "P.U.", "Total");
    printf("-----------------------------------------------\n");

    for (int i = 0; i < f->nbLignes; i++) {
        int idx = estExiste(f->lignes[i].idMedicament);
        char nomAffiche[26];

        if (idx != -1) {
            strncpy(nomAffiche, med[idx].nom_commercial, 25);
            nomAffiche[25] = '\0';
        } else {
            strcpy(nomAffiche, "Inconnu");
        }

        printf("%-4d %-25s %5d %8.2f %10.2f\n",
            f->lignes[i].idMedicament,
            nomAffiche,
            f->lignes[i].quantite,
            f->lignes[i].prixUnitaire,
            f->lignes[i].sousTotal
        );
    }

    printf("-----------------------------------------------\n");
    printf("%-39s %10.2f DH\n", "Montant HT :", f->montantHT);
    printf("%-39s %10.2f%%\n", "TVA :", f->tauxTVA);
    printf("%-39s %10.2f DH\n", "Montant TVA :", f->montantTVA);
    printf("%-39s %10.2f DH\n", "Montant TTC :", f->montantTTC);
    printf("%-39s %10.2f DH\n", "Montant paye :", f->montantPaye);
    printf("%-39s %10.2f DH\n", "Reste a payer :", f->montantRestant);

    if (strlen(f->modePaiement) > 0) {
        printf("%-39s %s\n", "Mode de paiement :", f->modePaiement);
    }

    if (strlen(f->remarques) > 0) {
        printf("\nRemarques : %s\n", f->remarques);
    }

    printf("===============================================\n");
}

void imprimerFacture(Facture *f) {
    afficherFacture(f);
}

/* ====================================================================
   CREATION FACTURE DEPUIS VENTE
   ==================================================================== */
void creerFactureDepuisVente() {
    if (nbVentes == 0) {
        ui_error("Aucune vente enregistree.");
        return;
    }

    printf("\n=== CREER FACTURE DEPUIS VENTE ===\n");
    printf("ID de la vente : ");
    int idVente;
    scanf("%d", &idVente);
    clearBuffer();

    Vente *v = rechercherVente_parID(idVente);
    if (!v) {
        ui_error("Vente introuvable.");
        return;
    }

    if (nbFactures >= MAX_FACTURES) {
        ui_error("Capacite maximale de factures atteinte.");
        return;
    }

    Facture f;
    f.id = nbFactures + 1;
    strcpy(f.type, "CLIENT");
    genererNumeroFacture(f.numero, "CLIENT");
    strcpy(f.date, v->date);

    // Date d'échéance : 30 jours après
    strcpy(f.dateEcheance, v->date);

    // Informations client
    f.clientId = v->client.id;
    strcpy(f.clientNom, v->client.nom);
    strcpy(f.clientPrenom, v->client.prenom);
    strcpy(f.clientTelephone, v->client.telephone);

    // Copier les lignes
    f.nbLignes = v->nbLignes;
    for (int i = 0; i < v->nbLignes; i++) {
        f.lignes[i].idMedicament = v->lignes[i].idMedicament;
        f.lignes[i].quantite = v->lignes[i].quantite;
        f.lignes[i].prixUnitaire = v->lignes[i].prixUnitaire;
        f.lignes[i].sousTotal = v->lignes[i].sousTotal;
    }

    // TVA
    printf("Taux TVA (%%) [20] : ");
    char input[10];
    lireString(input, 10);
    f.tauxTVA = (strlen(input) > 0) ? atof(input) : 20.0;

    // Calculer les montants
    calculerMontantsFacture(&f);

    // Paiement
    f.montantPaye = v->montantPaye;
    strcpy(f.modePaiement, v->modePaiement);

    if (f.montantRestant <= 0) {
        strcpy(f.statut, "PAYEE");
    } else {
        strcpy(f.statut, "VALIDEE");
    }

    printf("Remarques (optionnel) : ");
    lireString(f.remarques, 200);

    factures[nbFactures++] = f;
    sauvegarderFacturesTexte();

    ui_success("Facture creee avec succes !");
    afficherFacture(&f);
}

/* ====================================================================
   CREATION FACTURE FOURNISSEUR
   ==================================================================== */
void creerFactureFournisseur() {
    if (nbFournisseurs == 0) {
        ui_error("Aucun fournisseur enregistre.");
        return;
    }

    if (nbFactures >= MAX_FACTURES) {
        ui_error("Capacite maximale de factures atteinte.");
        return;
    }

    Facture f;
    f.id = nbFactures + 1;
    strcpy(f.type, "FOURNISSEUR");
    genererNumeroFacture(f.numero, "FOURNISSEUR");

    printf("\n=== NOUVELLE FACTURE FOURNISSEUR ===\n");

    // Date
    obtenirDateActuelle(f.date, NULL);

    printf("Date echeance (JJ/MM/AAAA) : ");
    lireString(f.dateEcheance, 11);

    // Fournisseur
    printf("ID Fournisseur : ");
    scanf("%d", &f.fournisseurId);
    clearBuffer();

    Fournisseur *fournisseur = rechercherF_parID(f.fournisseurId);
    if (!fournisseur) {
        ui_error("Fournisseur introuvable.");
        return;
    }

    strcpy(f.fournisseurNom, fournisseur->raison_sociale);
    sprintf(f.fournisseurContact, "%s %s",
            fournisseur->contact_prenom,
            fournisseur->contact_nom);
    strcpy(f.fournisseurTelephone, fournisseur->telephone);

    // Lignes
    f.nbLignes = 0;
    char continuer = 'o';

    while (continuer == 'o' || continuer == 'O') {
        if (f.nbLignes >= MAX_LIGNES_FACTURE) {
            printf("Nombre maximum de lignes atteint.\n");
            break;
        }

        LigneFacture l;
        printf("\n--- Article %d ---\n", f.nbLignes + 1);
        printf("ID medicament : ");
        scanf("%d", &l.idMedicament);
        clearBuffer();

        int idx = estExiste(l.idMedicament);
        if (idx == -1) {
            printf("Medicament introuvable.\n");
            continue;
        }

        printf("Medicament : %s\n", med[idx].nom_commercial);
        printf("Quantite : ");
        scanf("%d", &l.quantite);
        clearBuffer();

        printf("Prix unitaire HT : ");
        scanf("%f", &l.prixUnitaire);
        clearBuffer();

        l.sousTotal = l.quantite * l.prixUnitaire;
        f.lignes[f.nbLignes++] = l;

        printf("Sous-total : %.2f DH\n", l.sousTotal);
        printf("\nAjouter un autre article ? (o/n) : ");
        scanf(" %c", &continuer);
        clearBuffer();
    }

    if (f.nbLignes == 0) {
        ui_error("Aucun article ajoute. Facture annulee.");
        return;
    }

    // TVA
    printf("Taux TVA (%%) [20] : ");
    char input[10];
    lireString(input, 10);
    f.tauxTVA = (strlen(input) > 0) ? atof(input) : 20.0;

    calculerMontantsFacture(&f);

    // Statut
    strcpy(f.statut, "VALIDEE");
    f.montantPaye = 0;
    strcpy(f.modePaiement, "");

    printf("Remarques (optionnel) : ");
    lireString(f.remarques, 200);

    factures[nbFactures++] = f;
    sauvegarderFacturesTexte();

    ui_success("Facture fournisseur creee avec succes !");
    afficherFacture(&f);
}

/* ====================================================================
   AFFICHAGE ET RECHERCHE
   ==================================================================== */
void afficherFacturesClients() {
    int trouve = 0;

    printf("\n=========== FACTURES CLIENTS ===========\n");
    printf("%-5s %-15s %-12s %-20s %12s %10s\n",
           "ID", "Numero", "Date", "Client", "Montant", "Statut");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < nbFactures; i++) {
        if (strcmp(factures[i].type, "CLIENT") == 0) {
            printf("%-5d %-15s %-12s %-20s %12.2f %10s\n",
                factures[i].id,
                factures[i].numero,
                factures[i].date,
                factures[i].clientNom,
                factures[i].montantTTC,
                factures[i].statut
            );
            trouve = 1;
        }
    }

    if (!trouve)
        printf("Aucune facture client.\n");

    printf("---------------------------------------------------------------\n");
}


void afficherFacturesFournisseurs() {
    int trouve = 0;

    printf("\n======= FACTURES FOURNISSEURS =======\n");
    printf("%-5s %-15s %-12s %-25s %12s %10s\n",
           "ID", "Numero", "Date", "Fournisseur", "Montant", "Statut");
    printf("------------------------------------------------------------------\n");

    for (int i = 0; i < nbFactures; i++) {
        if (strcmp(factures[i].type, "FOURNISSEUR") == 0) {
            printf("%-5d %-15s %-12s %-25s %12.2f %10s\n",
                factures[i].id,
                factures[i].numero,
                factures[i].date,
                factures[i].fournisseurNom,
                factures[i].montantTTC,
                factures[i].statut
            );
            trouve = 1;
        }
    }

    if (!trouve)
        printf("Aucune facture fournisseur.\n");

    printf("------------------------------------------------------------------\n");
}

void rechercherFacture() {
    if (nbFactures == 0) {
        ui_info("Aucune facture enregistree.");
        return;
    }

    printf("\n=== RECHERCHER UNE FACTURE ===\n");
    printf("1. Par ID\n");
    printf("2. Par numero\n");
    printf("Votre choix : ");

    int choix;
    scanf("%d", &choix);
    clearBuffer();

    Facture *f = NULL;

    if (choix == 1) {
        int id;
        printf("ID de la facture : ");
        scanf("%d", &id);
        clearBuffer();
        f = rechercherFactureParId(id);
    } else if (choix == 2) {
        char numero[30];
        printf("Numero de facture : ");
        lireString(numero, 30);
        f = rechercherFactureParNumero(numero);
    }

    if (f) {
        afficherFacture(f);
    } else {
        ui_error("Facture introuvable.");
    }
}

void afficherFacturesParStatut(const char *statut) {
    int trouve = 0;

    printf("\n=== FACTURES %s ===\n", statut);
    printf("%-5s %-15s %-12s %-20s %12s\n",
           "ID", "Numero", "Date", "Client/Fournisseur", "Montant TTC");
    printf("----------------------------------------------------------------------\n");

    for (int i = 0; i < nbFactures; i++) {
        if (strcmp(factures[i].statut, statut) == 0) {
            char nom[21];
            if (strcmp(factures[i].type, "CLIENT") == 0) {
                snprintf(nom, 21, "%s", factures[i].clientNom);
            } else {
                snprintf(nom, 21, "%s", factures[i].fournisseurNom);
            }

            printf("%-5d %-15s %-12s %-20s %12.2f\n",
                factures[i].id,
                factures[i].numero,
                factures[i].date,
                nom,
                factures[i].montantTTC
            );
            trouve = 1;
        }
    }

    if (!trouve) {
        printf("Aucune facture avec ce statut.\n");
    }
    printf("----------------------------------------------------------------------\n");
}

void afficherFacturesImpayees() {
    int count = 0;
    float total = 0;

    printf("\n=== FACTURES IMPAYEES ===\n");
    printf("%-5s %-15s %-12s %-20s %12s %12s\n",
           "ID", "Numero", "Echeance", "Client/Fournisseur", "Montant TTC", "Reste");
    printf("------------------------------------------------------------------------------------\n");

    for (int i = 0; i < nbFactures; i++) {
        if (factures[i].montantRestant > 0) {
            char nom[21];
            if (strcmp(factures[i].type, "CLIENT") == 0) {
                snprintf(nom, 21, "%s", factures[i].clientNom);
            } else {
                snprintf(nom, 21, "%s", factures[i].fournisseurNom);
            }

            printf("%-5d %-15s %-12s %-20s %12.2f %12.2f\n",
                factures[i].id,
                factures[i].numero,
                factures[i].dateEcheance,
                nom,
                factures[i].montantTTC,
                factures[i].montantRestant
            );
            total += factures[i].montantRestant;
            count++;
        }
    }

    printf("------------------------------------------------------------------------------------\n");
    printf("Total impaye : %.2f DH (%d facture(s))\n", total, count);
}

/* ====================================================================
   MODIFICATION ET SUPPRESSION
   ==================================================================== */
void marquerFacturePayee() {
    if (nbFactures == 0) {
        ui_info("Aucune facture enregistree.");
        return;
    }

    printf("ID de la facture : ");
    int id;
    scanf("%d", &id);
    clearBuffer();

    Facture *f = rechercherFactureParId(id);
    if (!f) {
        ui_error("Facture introuvable.");
        return;
    }

    if (f->montantRestant <= 0) {
        ui_info("Cette facture est deja payee.");
        return;
    }

    printf("Montant actuel a payer : %.2f DH\n", f->montantRestant);
    printf("Montant du paiement : ");
    float montant;
    scanf("%f", &montant);
    clearBuffer();

    if (montant <= 0) {
        ui_error("Montant invalide.");
        return;
    }

    f->montantPaye += montant;
    f->montantRestant = f->montantTTC - f->montantPaye;

    if (f->montantRestant <= 0) {
        strcpy(f->statut, "PAYEE");
        f->montantRestant = 0;
    }

    printf("Mode de paiement :\n");
    printf("1. Especes\n");
    printf("2. Carte\n");
    printf("3. Cheque\n");
    printf("4. Virement\n");
    printf("Choix : ");

    int mode;
    scanf("%d", &mode);
    clearBuffer();

    switch(mode) {
        case 1: strcpy(f->modePaiement, "ESPECES"); break;
        case 2: strcpy(f->modePaiement, "CARTE"); break;
        case 3: strcpy(f->modePaiement, "CHEQUE"); break;
        case 4: strcpy(f->modePaiement, "VIREMENT"); break;
        default: strcpy(f->modePaiement, "AUTRE");
    }

    sauvegarderFacturesTexte();
    ui_success("Paiement enregistre avec succes !");
    afficherFacture(f);
}

void modifierFacture() {
    if (nbFactures == 0) {
        ui_info("Aucune facture enregistree.");
        return;
    }

    printf("ID de la facture a modifier : ");
    int id;
    scanf("%d", &id);
    clearBuffer();

    Facture *f = rechercherFactureParId(id);
    if (!f) {
        ui_error("Facture introuvable.");
        return;
    }

    if (strcmp(f->statut, "PAYEE") == 0) {
        ui_warning("Impossible de modifier une facture payee.");
        return;
    }

    printf("\n--- Modification de la facture %s ---\n", f->numero);
    printf("1. Modifier statut\n");
    printf("2. Modifier date echeance\n");
    printf("3. Modifier remarques\n");
    printf("0. Annuler\n");
    printf("Choix : ");

    int choix;
    scanf("%d", &choix);
    clearBuffer();

    switch(choix) {
        case 1:
            printf("Nouveau statut (BROUILLON/VALIDEE/ANNULEE) : ");
            lireString(f->statut, 20);
            break;
        case 2:
            printf("Nouvelle date echeance (JJ/MM/AAAA) : ");
            lireString(f->dateEcheance, 11);
            break;
        case 3:
            printf("Nouvelles remarques : ");
            lireString(f->remarques, 200);
            break;
        default:
            return;
    }

    sauvegarderFacturesTexte();
    ui_success("Facture modifiee avec succes !");
}

void supprimerFacture() {
    if (nbFactures == 0) {
        ui_info("Aucune facture enregistree.");
        return;
    }

    printf("ID de la facture a supprimer : ");
    int id;
    scanf("%d", &id);
    clearBuffer();

    Facture *f = rechercherFactureParId(id);
    if (!f) {
        ui_error("Facture introuvable.");
        return;
    }

    printf("Confirmer la suppression de la facture %s ? (o/n) : ", f->numero);
    char confirm;
    scanf(" %c", &confirm);
    clearBuffer();

    if (confirm == 'o' || confirm == 'O') {
        int pos = f - factures;
        for (int i = pos; i < nbFactures - 1; i++) {
            factures[i] = factures[i + 1];
        }
        nbFactures--;
        sauvegarderFacturesTexte();
        ui_success("Facture supprimee avec succes.");
    } else {
        ui_info("Suppression annulee.");
    }
}

/* ====================================================================
   STATISTIQUES
   ==================================================================== */
void statistiquesFactures() {
    if (nbFactures == 0) {
        ui_info("Aucune facture enregistree.");
        return;
    }

    float totalHT = 0, totalTTC = 0, totalPaye = 0, totalRestant = 0;
    int nbClients = 0, nbFournisseurs = 0;
    int nbPayees = 0, nbImpayees = 0;

    for (int i = 0; i < nbFactures; i++) {
        totalHT += factures[i].montantHT;
        totalTTC += factures[i].montantTTC;
        totalPaye += factures[i].montantPaye;
        totalRestant += factures[i].montantRestant;

        if (strcmp(factures[i].type, "CLIENT") == 0) {
            nbClients++;
        } else {
            nbFournisseurs++;
        }

        if (strcmp(factures[i].statut, "PAYEE") == 0) {
            nbPayees++;
        } else if (factures[i].montantRestant > 0) {
            nbImpayees++;
        }
    }

    printf("\n========== STATISTIQUES DES FACTURES ==========\n");
    printf("Nombre total de factures : %d\n", nbFactures);
    printf("  - Factures clients : %d\n", nbClients);
    printf("  - Factures fournisseurs : %d\n", nbFournisseurs);
    printf("-----------------------------------------------\n");
    printf("Montant total HT : %.2f DH\n", totalHT);
    printf("Montant total TTC : %.2f DH\n", totalTTC);
    printf("Montant total paye : %.2f DH\n", totalPaye);
    printf("Montant total restant : %.2f DH\n", totalRestant);
    printf("-----------------------------------------------\n");
    printf("Factures payees : %d\n", nbPayees);
    printf("Factures impayees : %d\n", nbImpayees);
    printf("===============================================\n");
}

void afficherChiffreAffaires() {
    float caClients = 0, caFournisseurs = 0;

    for (int i = 0; i < nbFactures; i++) {
        if (strcmp(factures[i].type, "CLIENT") == 0) {
            caClients += factures[i].montantTTC;
        } else {
            caFournisseurs += factures[i].montantTTC;
        }
    }

    printf("\n========== CHIFFRE D'AFFAIRES ==========\n");
    printf("CA Clients : %.2f DH\n", caClients);
    printf("CA Fournisseurs : %.2f DH\n", caFournisseurs);
    printf("Resultat : %.2f DH\n", caClients - caFournisseurs);
    printf("========================================\n");
}

/* ====================================================================
   MENU PRINCIPAL
   ==================================================================== */
void menuFactures() {
    int choix;
    const char *options[] = {
        "Creer facture depuis vente",
        "Creer facture fournisseur",
       "Afficher factures clients",
    "Afficher factures fournisseurs",
        "Rechercher une facture",
        "Marquer facture comme payee",
        "Afficher factures impayees",
        "Modifier une facture",
        "Supprimer une facture",
        "Statistiques",
        "Chiffre d'affaires",
        "Retour"
    };

    do {
        ui_clear();
        ui_logo();
        ui_title("Gestion des Factures");

        choix = ui_menu("Menu Factures", options, 11);

        switch (choix) {
            case 1:
                creerFactureDepuisVente();
                ui_pause();
                break;

            case 2:
                creerFactureFournisseur();
                ui_pause();
                break;

           case 3:
    afficherFacturesClients();
    ui_pause();
    break;

case 4:
    afficherFacturesFournisseurs();
    ui_pause();
    break;

            case 5:
                rechercherFacture();
                ui_pause();
                break;

            case 6:
                marquerFacturePayee();
                ui_pause();
                break;

            case 7:
                afficherFacturesImpayees();
                ui_pause();
                break;

            case 8:
                modifierFacture();
                ui_pause();
                break;

            case 9:
                supprimerFacture();
                ui_pause();
                break;

            case 10:
                statistiquesFactures();
                ui_pause();
                break;

            case 11:
                afficherChiffreAffaires();
                ui_pause();
                break;

            case 12:
            case 0:
                break;

            default:
                ui_warning("Choix invalide !");
                ui_pause();
        }

    } while (choix != 0 && choix != 12);
}

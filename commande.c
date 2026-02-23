#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commande.h"
#include "medicaments.h"
#include "fournisseurs.h"
#include "Client.h"
#include "utils.h"
#include "ui.h"

Commande commandes[MAX_COMMANDES];
int nbCommandes = 0;

extern Medicament med[];
extern int nbMed;
extern Fournisseur fournisseurs[];
extern int nbFournisseurs;
extern Client clients[];
extern int nbClients;

/* ====================================================================
   SAUVEGARDE ET CHARGEMENT
   ==================================================================== */
void sauvegarderCommandes() {
    FILE *f = fopen("commandes.txt", "w");
    if (!f) {
        ui_error("Erreur lors de la sauvegarde des commandes !");
        return;
    }

    fprintf(f, "%d\n", nbCommandes);

    for (int i = 0; i < nbCommandes; i++) {
        Commande *cmd = &commandes[i];

        fprintf(f, "%d;%s;%s;%s;%s;%d;%s;%s;%f;%d;%s;%d;%s;%d\n",
            cmd->id,
            cmd->numero,
            cmd->dateCommande,
            cmd->dateLivraisonPrevue,
            cmd->dateLivraisonReelle,
            cmd->fournisseurId,
            cmd->fournisseurNom,
            cmd->fournisseurContact,
            cmd->montantTotal,
            cmd->statut,
            cmd->remarques,
            cmd->clientId,
            cmd->clientNom,
            cmd->nbLignes
        );

        for (int j = 0; j < cmd->nbLignes; j++) {
            fprintf(f, "%d;%s;%s;%s;%d;%f;%f\n",
                cmd->lignes[j].idMedicament,
                cmd->lignes[j].nomMedicament,
                cmd->lignes[j].formePharma,
                cmd->lignes[j].dosage,
                cmd->lignes[j].quantite,
                cmd->lignes[j].prixUnitaire,
                cmd->lignes[j].sousTotal
            );
        }
    }

    fclose(f);
}

void chargerCommandes() {
    FILE *f = fopen("commandes.txt", "r");
    if (!f) {
        nbCommandes = 0;
        return;
    }

    fscanf(f, "%d\n", &nbCommandes);

    for (int i = 0; i < nbCommandes; i++) {
        Commande *cmd = &commandes[i];
        int statutInt;

        fscanf(f, "%d;%29[^;];%10[^;];%10[^;];%10[^;];%d;%99[^;];%99[^;];%f;%d;%199[^;];%d;%59[^;];%d\n",
            &cmd->id,
            cmd->numero,
            cmd->dateCommande,
            cmd->dateLivraisonPrevue,
            cmd->dateLivraisonReelle,
            &cmd->fournisseurId,
            cmd->fournisseurNom,
            cmd->fournisseurContact,
            &cmd->montantTotal,
            &statutInt,
            cmd->remarques,
            &cmd->clientId,
            cmd->clientNom,
            &cmd->nbLignes
        );

        cmd->statut = (StatutCommande)statutInt;

        for (int j = 0; j < cmd->nbLignes; j++) {
            fscanf(f, "%d;%99[^;];%49[^;];%49[^;];%d;%f;%f\n",
                &cmd->lignes[j].idMedicament,
                cmd->lignes[j].nomMedicament,
                cmd->lignes[j].formePharma,
                cmd->lignes[j].dosage,
                &cmd->lignes[j].quantite,
                &cmd->lignes[j].prixUnitaire,
                &cmd->lignes[j].sousTotal
            );
        }
    }

    fclose(f);
}

/* ====================================================================
   FONCTIONS UTILITAIRES
   ==================================================================== */
void genererNumeroCommande(char *numero) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(numero, "CMD-%04d-%03d", tm.tm_year + 1900, nbCommandes + 1);
}

const char* statutToString(StatutCommande statut) {
    switch(statut) {
        case COMMANDE_EN_ATTENTE: return "EN_ATTENTE";
        case COMMANDE_VALIDEE: return "VALIDEE";
        case COMMANDE_ENVOYEE: return "ENVOYEE";
        case COMMANDE_RECUE: return "RECUE";
        case COMMANDE_ANNULEE: return "ANNULEE";
        default: return "INCONNU";
    }
}

StatutCommande stringToStatut(const char *str) {
    if (strcmp(str, "EN_ATTENTE") == 0) return COMMANDE_EN_ATTENTE;
    if (strcmp(str, "VALIDEE") == 0) return COMMANDE_VALIDEE;
    if (strcmp(str, "ENVOYEE") == 0) return COMMANDE_ENVOYEE;
    if (strcmp(str, "RECUE") == 0) return COMMANDE_RECUE;
    if (strcmp(str, "ANNULEE") == 0) return COMMANDE_ANNULEE;
    return COMMANDE_EN_ATTENTE;
}

Commande* rechercherCommandeParId(int id) {
    for (int i = 0; i < nbCommandes; i++) {
        if (commandes[i].id == id) {
            return &commandes[i];
        }
    }
    return NULL;
}

/* ====================================================================
   PASSER UNE COMMANDE POUR UN MÉDICAMENT MANQUANT
   ==================================================================== */
void passerCommandeMedicamentManquant(int clientId, const char *nomClient) {
    if (nbCommandes >= MAX_COMMANDES) {
        ui_error("Capacité maximale de commandes atteinte !");
        return;
    }

    if (nbFournisseurs == 0) {
        ui_error("Aucun fournisseur enregistré. Veuillez d'abord ajouter des fournisseurs.");
        return;
    }

    Commande cmd;
    cmd.id = nbCommandes + 1;
    cmd.nbLignes = 0;
    cmd.montantTotal = 0;
    cmd.statut = COMMANDE_EN_ATTENTE;
    cmd.clientId = clientId;

    if (clientId > 0 && nomClient != NULL) {
        strncpy(cmd.clientNom, nomClient, 59);
        cmd.clientNom[59] = '\0';
    } else {
        strcpy(cmd.clientNom, "Stock général");
    }

    genererNumeroCommande(cmd.numero);

    char heure[9];
    obtenirDateActuelle(cmd.dateCommande, heure);
    strcpy(cmd.dateLivraisonReelle, "");

    ui_clear();
    ui_logo();
    ui_title("COMMANDE MEDICAMENT MANQUANT");

    if (clientId > 0) {
        printf("\033[1;36mClient demandeur:\033[0m %s\n\n", cmd.clientNom);
    }

    /* ---- Afficher les fournisseurs disponibles ---- */
    printf("\033[1;33m=== FOURNISSEURS DISPONIBLES ===\033[0m\n");
    for (int i = 0; i < nbFournisseurs; i++) {
        printf("%d. %s (Tel: %s)\n",
            fournisseurs[i].fournisseur_id,
            fournisseurs[i].raison_sociale,
            fournisseurs[i].telephone);
    }
    printf("\n");

    /* ---- Sélection du fournisseur ---- */
    ui_input_prompt("ID du fournisseur");
    scanf("%d", &cmd.fournisseurId);
    clearBuffer();

    Fournisseur *fournisseur = rechercherF_parID(cmd.fournisseurId);
    if (!fournisseur) {
        ui_error("Fournisseur introuvable !");
        return;
    }

    strcpy(cmd.fournisseurNom, fournisseur->raison_sociale);
    sprintf(cmd.fournisseurContact, "%s %s - %s",
            fournisseur->contact_prenom,
            fournisseur->contact_nom,
            fournisseur->telephone);

    /* ---- Calcul date de livraison prévue ---- */
    time_t t = time(NULL);
    t += fournisseur->delai_livraison_jours * 24 * 3600;
    struct tm *tm_info = localtime(&t);
    sprintf(cmd.dateLivraisonPrevue, "%02d/%02d/%04d",
            tm_info->tm_mday,
            tm_info->tm_mon + 1,
            tm_info->tm_year + 1900);

    printf("\n\033[1;32mFournisseur sélectionné:\033[0m %s\n", cmd.fournisseurNom);
    printf("\033[1;32mDélai de livraison:\033[0m %d jours\n", fournisseur->delai_livraison_jours);
    printf("\033[1;32mLivraison prévue le:\033[0m %s\n\n", cmd.dateLivraisonPrevue);

    /* ---- Ajout des lignes de commande ---- */
    char continuer = 'o';
    while (continuer == 'o' || continuer == 'O') {
        if (cmd.nbLignes >= MAX_LIGNES_COMMANDE) {
            ui_warning("Nombre maximum de lignes atteint !");
            break;
        }

        LigneCommande ligne;
        printf("\n\033[1;33m--- Article %d ---\033[0m\n", cmd.nbLignes + 1);

        printf("Le médicament existe-t-il déjà dans votre stock ? (o/n) : ");
        char existe;
        scanf(" %c", &existe);
        clearBuffer();

        if (existe == 'o' || existe == 'O') {
            /* Médicament existant - juste réapprovisionner */
            ui_input_prompt("ID du médicament");
            scanf("%d", &ligne.idMedicament);
            clearBuffer();

            int idx = estExiste(ligne.idMedicament);
            if (idx == -1) {
                ui_error("Médicament introuvable dans le stock !");
                continue;
            }

            strcpy(ligne.nomMedicament, med[idx].nom_commercial);
            strcpy(ligne.formePharma, med[idx].forme_pharmaceutique);
            strcpy(ligne.dosage, med[idx].dosage);
            ligne.prixUnitaire = med[idx].prix_achat;

        } else {
            /* Nouveau médicament */
            ligne.idMedicament = 0; // 0 = nouveau médicament

            ui_input_prompt("Nom du médicament");
            lireString(ligne.nomMedicament, 100);

            ui_input_prompt("Forme pharmaceutique (ex: Comprimé, Sirop, Gélule)");
            lireString(ligne.formePharma, 50);

            ui_input_prompt("Dosage (ex: 500mg, 10ml)");
            lireString(ligne.dosage, 50);

            ui_input_prompt("Prix unitaire d'achat");
            scanf("%f", &ligne.prixUnitaire);
            clearBuffer();
        }

        ui_input_prompt("Quantité à commander");
        scanf("%d", &ligne.quantite);
        clearBuffer();

        if (ligne.quantite <= 0) {
            ui_error("Quantité invalide !");
            continue;
        }

        ligne.sousTotal = ligne.quantite * ligne.prixUnitaire;
        cmd.lignes[cmd.nbLignes++] = ligne;
        cmd.montantTotal += ligne.sousTotal;

        printf("\n\033[1;32mSous-total:\033[0m %.2f DH\n", ligne.sousTotal);
        printf("\033[1;32mTotal commande:\033[0m %.2f DH\n", cmd.montantTotal);

        printf("\nAjouter un autre article ? (o/n) : ");
        scanf(" %c", &continuer);
        clearBuffer();
    }

    if (cmd.nbLignes == 0) {
        ui_error("Aucun article ajouté. Commande annulée.");
        return;
    }

    /* ---- Remarques ---- */
    printf("\nRemarques (optionnel) : ");
    lireString(cmd.remarques, 200);

    /* ---- Enregistrement ---- */
    commandes[nbCommandes++] = cmd;
    sauvegarderCommandes();

    ui_success("Commande créée avec succès !");
    afficherDetailsCommande(&cmd);

    printf("\n\033[1;33mLe fournisseur sera contacté pour confirmer la commande.\033[0m\n");
    printf("\033[1;33mLes nouveaux médicaments seront ajoutés au stock lors de la réception.\033[0m\n");

    ui_pause();
}

/* ====================================================================
   VALIDER LA RÉCEPTION D'UNE COMMANDE
   ==================================================================== */
void validerReceptionCommande() {
    if (nbCommandes == 0) {
        ui_error("Aucune commande enregistrée !");
        return;
    }

    ui_clear();
    ui_logo();
    ui_title("RÉCEPTION DE COMMANDE");

    /* Afficher les commandes en attente de réception */
    printf("\033[1;33m=== COMMANDES EN ATTENTE ===\033[0m\n");
    int found = 0;
    for (int i = 0; i < nbCommandes; i++) {
        if (commandes[i].statut == COMMANDE_VALIDEE ||
            commandes[i].statut == COMMANDE_ENVOYEE) {
            printf("ID: %d | Numéro: %s | Fournisseur: %s | Montant: %.2f DH\n",
                commandes[i].id,
                commandes[i].numero,
                commandes[i].fournisseurNom,
                commandes[i].montantTotal);
            found = 1;
        }
    }

    if (!found) {
        ui_warning("Aucune commande en attente de réception !");
        ui_pause();
        return;
    }

    printf("\n");
    ui_input_prompt("ID de la commande à réceptionner");
    int id;
    scanf("%d", &id);
    clearBuffer();

    Commande *cmd = rechercherCommandeParId(id);
    if (!cmd) {
        ui_error("Commande introuvable !");
        ui_pause();
        return;
    }

    if (cmd->statut == COMMANDE_RECUE) {
        ui_warning("Cette commande a déjà été réceptionnée !");
        ui_pause();
        return;
    }

    if (cmd->statut == COMMANDE_ANNULEE) {
        ui_error("Cette commande a été annulée !");
        ui_pause();
        return;
    }

    /* Afficher les détails */
    afficherDetailsCommande(cmd);

    printf("\n\033[1;31mConfirmer la réception de cette commande ? (o/n) : \033[0m");
    char confirm;
    scanf(" %c", &confirm);
    clearBuffer();

    if (confirm != 'o' && confirm != 'O') {
        ui_info("Réception annulée.");
        ui_pause();
        return;
    }

    /* Traiter chaque ligne de la commande */
    for (int i = 0; i < cmd->nbLignes; i++) {
        LigneCommande *ligne = &cmd->lignes[i];

        if (ligne->idMedicament == 0) {
            /* NOUVEAU MÉDICAMENT - L'ajouter au stock */
            if (nbMed >= MAX_MED) {
                printf("\n\033[1;31mATTENTION: Stock plein ! Impossible d'ajouter: %s\033[0m\n",
                    ligne->nomMedicament);
                continue;
            }

            Medicament nouveau;
            nouveau.medicament_id = nbMed + 1;

            sprintf(nouveau.code_barre, "AUTO-%d", nouveau.medicament_id);
            strncpy(nouveau.nom_commercial, ligne->nomMedicament, 49);
            strcpy(nouveau.nom_scientifique, ligne->nomMedicament);
            strncpy(nouveau.forme_pharmaceutique, ligne->formePharma, 49);
            strncpy(nouveau.dosage, ligne->dosage, 49);

            strcpy(nouveau.unite_dosage, "unité");
            strcpy(nouveau.presentation, "Boîte");
            strcpy(nouveau.categorie, "À classifier");
            strcpy(nouveau.fabricant, cmd->fournisseurNom);
            strcpy(nouveau.pays_origine, "À définir");
            strcpy(nouveau.mode_administration, "Selon prescription");
            strcpy(nouveau.ordonnance_obligatoire, "À vérifier");
            strcpy(nouveau.emplacement_stock, "Zone-A");

            obtenirDateActuelle(nouveau.date_fabrication, NULL);
            strcpy(nouveau.date_expiration, "À définir");

            nouveau.stock_actuel = ligne->quantite;
            nouveau.stock_min = ligne->quantite / 4; // 25% du stock initial
            nouveau.prix_achat = ligne->prixUnitaire;
            nouveau.prix_vente = ligne->prixUnitaire * 1.3; // Marge de 30%

            med[nbMed++] = nouveau;

            printf("\n\033[1;32m[NOUVEAU] %s ajouté au stock (ID: %d, Qté: %d)\033[0m\n",
                nouveau.nom_commercial, nouveau.medicament_id, nouveau.stock_actuel);

        } else {
            /* MÉDICAMENT EXISTANT - Mettre à jour le stock */
            int idx = estExiste(ligne->idMedicament);
            if (idx != -1) {
                med[idx].stock_actuel += ligne->quantite;
                printf("\n\033[1;32m[MAJ] %s - Stock mis à jour: %d unités\033[0m\n",
                    med[idx].nom_commercial, med[idx].stock_actuel);
            }
        }
    }

    /* Mettre à jour la commande */
    cmd->statut = COMMANDE_RECUE;
    char heure[9];
    obtenirDateActuelle(cmd->dateLivraisonReelle, heure);

    /* Sauvegarder */
    sauvegarderMed();
    sauvegarderCommandes();

    ui_success("Commande réceptionnée avec succès !");
    printf("\n\033[1;36mLe stock a été mis à jour.\033[0m\n");

    ui_pause();
}

/* ====================================================================
   AFFICHAGE
   ==================================================================== */
void afficherDetailsCommande(Commande *cmd) {
    printf("\n\033[1;36m========================================\033[0m\n");
    printf("\033[1;36m       COMMANDE %s\033[0m\n", cmd->numero);
    printf("\033[1;36m========================================\033[0m\n");
    printf("ID: %d\n", cmd->id);
    printf("Date: %s\n", cmd->dateCommande);
    printf("Statut: \033[1;33m%s\033[0m\n", statutToString(cmd->statut));
    printf("----------------------------------------\n");
    printf("Fournisseur: %s\n", cmd->fournisseurNom);
    printf("Contact: %s\n", cmd->fournisseurContact);
    printf("Livraison prévue: %s\n", cmd->dateLivraisonPrevue);
    if (strlen(cmd->dateLivraisonReelle) > 0)
        printf("Livraison réelle: %s\n", cmd->dateLivraisonReelle);

    if (cmd->clientId > 0)
        printf("Client demandeur: %s\n", cmd->clientNom);

    printf("----------------------------------------\n");
    printf("%-25s %8s %10s %12s\n", "Médicament", "Qté", "P.U.", "Total");
    printf("----------------------------------------\n");

    for (int i = 0; i < cmd->nbLignes; i++) {
        printf("%-25s %8d %10.2f %12.2f\n",
            cmd->lignes[i].nomMedicament,
            cmd->lignes[i].quantite,
            cmd->lignes[i].prixUnitaire,
            cmd->lignes[i].sousTotal);
    }

    printf("----------------------------------------\n");
    printf("%-44s %12.2f DH\n", "TOTAL:", cmd->montantTotal);

    if (strlen(cmd->remarques) > 0)
        printf("\nRemarques: %s\n", cmd->remarques);

    printf("\033[1;36m========================================\033[0m\n");
}

void afficherToutesCommandes() {
    if (nbCommandes == 0) {
        ui_warning("Aucune commande enregistrée !");
        return;
    }

    ui_title("TOUTES LES COMMANDES");

    printf("%-5s %-15s %-12s %-25s %12s %15s\n",
        "ID", "Numéro", "Date", "Fournisseur", "Montant", "Statut");
    printf("---------------------------------------------------------------------------------\n");

    for (int i = 0; i < nbCommandes; i++) {
        printf("%-5d %-15s %-12s %-25s %12.2f %15s\n",
            commandes[i].id,
            commandes[i].numero,
            commandes[i].dateCommande,
            commandes[i].fournisseurNom,
            commandes[i].montantTotal,
            statutToString(commandes[i].statut));
    }

    printf("---------------------------------------------------------------------------------\n");
    ui_pause();
}

void afficherCommandesParStatut(StatutCommande statut) {
    int found = 0;

    ui_title("COMMANDES PAR STATUT");
    printf("\033[1;33mStatut: %s\033[0m\n\n", statutToString(statut));

    printf("%-5s %-15s %-12s %-25s %12s\n",
        "ID", "Numéro", "Date", "Fournisseur", "Montant");
    printf("------------------------------------------------------------------------\n");

    for (int i = 0; i < nbCommandes; i++) {
        if (commandes[i].statut == statut) {
            printf("%-5d %-15s %-12s %-25s %12.2f\n",
                commandes[i].id,
                commandes[i].numero,
                commandes[i].dateCommande,
                commandes[i].fournisseurNom,
                commandes[i].montantTotal);
            found = 1;
        }
    }

    if (!found)
        ui_warning("Aucune commande avec ce statut !");

    printf("------------------------------------------------------------------------\n");
    ui_pause();
}

void rechercherCommande() {
    if (nbCommandes == 0) {
        ui_error("Aucune commande enregistrée !");
        return;
    }

    ui_input_prompt("ID de la commande");
    int id;
    scanf("%d", &id);
    clearBuffer();

    Commande *cmd = rechercherCommandeParId(id);
    if (cmd) {
        afficherDetailsCommande(cmd);
    } else {
        ui_error("Commande introuvable !");
    }

    ui_pause();
}

void annulerCommande() {
    if (nbCommandes == 0) {
        ui_error("Aucune commande enregistrée !");
        return;
    }

    ui_input_prompt("ID de la commande à annuler");
    int id;
    scanf("%d", &id);
    clearBuffer();

    Commande *cmd = rechercherCommandeParId(id);
    if (!cmd) {
        ui_error("Commande introuvable !");
        ui_pause();
        return;
    }

    if (cmd->statut == COMMANDE_RECUE) {
        ui_error("Impossible d'annuler une commande déjà réceptionnée !");
        ui_pause();
        return;
    }

    afficherDetailsCommande(cmd);

    printf("\n\033[1;31mConfirmer l'annulation ? (o/n) : \033[0m");
    char confirm;
    scanf(" %c", &confirm);
    clearBuffer();

    if (confirm == 'o' || confirm == 'O') {
        cmd->statut = COMMANDE_ANNULEE;
        sauvegarderCommandes();
        ui_success("Commande annulée !");
    } else {
        ui_info("Annulation abandonnée.");
    }

    ui_pause();
}

/* ====================================================================
   MENU PRINCIPAL
   ==================================================================== */
void menuCommandes() {
    int choix;
    const char *options[] = {
        "Commander un medicament manquant",
        "Receptionner une commande",
        "Afficher toutes les commandes",
        "Commandes en attente",
        "Commandes recues",
        "Rechercher une commande",
        "Annuler une commande",
        "Retour"
    };

    do {
        ui_clear();
        ui_logo();
        ui_title("Gestion des Commandes Fournisseurs");

        choix = ui_menu("Menu Commandes", options, 8);

        switch (choix) {
            case 1:
                passerCommandeMedicamentManquant(0, NULL);
                break;

            case 2:
                validerReceptionCommande();
                break;

            case 3:
                afficherToutesCommandes();
                break;

            case 4:
                afficherCommandesParStatut(COMMANDE_EN_ATTENTE);
                break;

            case 5:
                afficherCommandesParStatut(COMMANDE_RECUE);
                break;

            case 6:
                rechercherCommande();
                break;

            case 7:
                annulerCommande();
                break;

            case 8:
            case 0:
                break;

            default:
                ui_warning("Choix invalide !");
                ui_pause();
        }

    } while (choix != 0 && choix != 8);
}

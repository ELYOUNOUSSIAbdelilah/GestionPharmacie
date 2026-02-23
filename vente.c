#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vente.h"
#include "Client.h"
#include "medicaments.h"
#include "utils.h"
#include "commande.h"
#define MAX_VENTES 1000
#include "ui.h"           // ⭐ AJOUTER CETTE LIGNE (pour ui_clear, ui_logo, etc.)
Vente ventes[MAX_VENTES];
int nbVentes = 0;

extern Client clients[];
extern int nbClients;

extern Medicament med[];
extern int nbMed;

// ------------------------------------------------------
// Sauvegarde et chargement des ventes
// ------------------------------------------------------
void sauvegarderVentes() {
    FILE *f = fopen("ventes.txt", "w");
    if (!f) { ui_error("Impossible d'ouvrir ventes.txt pour sauvegarde."); return; }

    fprintf(f, "%d\n", nbVentes);
    for (int i = 0; i < nbVentes; i++) {
        Vente *v = &ventes[i];
        fprintf(f, "%d;%s;%s;%d;%s;%s;%f;%f;%f;%s;%d\n",
                v->id, v->date, v->heure,
                v->client.id, v->client.nom, v->client.prenom,
                v->montantTotal, v->montantPaye, v->monnaieRendue,
                v->modePaiement, v->nbLignes);

        for (int j = 0; j < v->nbLignes; j++) {
            fprintf(f, "%d;%d;%f;%f\n",
                    v->lignes[j].idMedicament,
                    v->lignes[j].quantite,
                    v->lignes[j].prixUnitaire,
                    v->lignes[j].sousTotal);
        }
    }
    fclose(f);
}

void chargerVentesTexte() {
    FILE *f = fopen("ventes.txt", "r");
    if (!f) return;

    fscanf(f, "%d\n", &nbVentes);
    for (int i = 0; i < nbVentes; i++) {
        Vente *v = &ventes[i];
        fscanf(f, "%d;%10[^;];%5[^;];%d;%29[^;];%29[^;];%f;%f;%f;%9[^;];%d\n",
               &v->id, v->date, v->heure,
               &v->client.id, v->client.nom, v->client.prenom,
               &v->montantTotal, &v->montantPaye, &v->monnaieRendue,
               v->modePaiement, &v->nbLignes);

        for (int j = 0; j < v->nbLignes; j++) {
            fscanf(f, "%d;%d;%f;%f\n",
                   &v->lignes[j].idMedicament,
                   &v->lignes[j].quantite,
                   &v->lignes[j].prixUnitaire,
                   &v->lignes[j].sousTotal);
        }
    }
    fclose(f);
}

// ------------------------------------------------------
// Recherche vente par ID
// ------------------------------------------------------
static int trouverIndiceVenteParId(int id) {
    for (int i = 0; i < nbVentes; i++)
        if (ventes[i].id == id) return i;
    return -1;
}

Vente* rechercherVente_parID(int id) {
    int pos = trouverIndiceVenteParId(id);
    if (pos != -1) return &ventes[pos];
    return NULL;
}

// ------------------------------------------------------
// Affichage du reçu avec style
// ------------------------------------------------------
void afficherRecu(Vente *v) {
    ui_clear();
    ui_title("RECU DE VENTE");

    printf("\033[1;34mID Vente:\033[0m %d\n", v->id);
    printf("\033[1;34mDate:\033[0m %s  \033[1;34mHeure:\033[0m %s\n", v->date, v->heure);
    printf("\033[1;34mClient:\033[0m %s %s\n", v->client.nom, v->client.prenom);
    if (v->client.id != 0 && strlen(v->client.telephone) > 0)
        printf("\033[1;34mTel:\033[0m %s\n", v->client.telephone);

    printf("\033[1;33m-------------------------------------------------------------\033[0m\n");
    printf("\033[1;36m%-4s %-25s %5s %8s %10s\033[0m\n", "ID", "Medicament", "Qte", "P.U.", "Total");
    printf("\033[1;33m-------------------------------------------------------------\033[0m\n");

    for (int i = 0; i < v->nbLignes; i++) {
        int idx = estExiste(v->lignes[i].idMedicament);
        char nom[26];
        if (idx != -1) strncpy(nom, med[idx].nom_commercial, 25);
        else strcpy(nom, "Inconnu");
        nom[25] = '\0';

        printf("%-4d %-25s %5d %8.2f %10.2f\n",
               v->lignes[i].idMedicament,
               nom,
               v->lignes[i].quantite,
               v->lignes[i].prixUnitaire,
               v->lignes[i].sousTotal);
    }

    printf("\033[1;33m-------------------------------------------------------------\033[0m\n");
    printf("%-39s %10.2f DH\n", "TOTAL :", v->montantTotal);
    printf("%-39s %s\n", "Mode paiement :", v->modePaiement);
    printf("%-39s %10.2f DH\n", "Montant paye :", v->montantPaye);
    if (v->monnaieRendue > 0)
        printf("%-39s %10.2f DH\n", "Monnaie rendue :", v->monnaieRendue);
    printf("\033[1;33m=============================================================\033[0m\n");

    ui_pause();
}

void afficherToutesLesVentes() {
    if (nbVentes == 0) {
        ui_warning("Aucune vente enregistrée !");
        return;
    }

    ui_title("TOUTES LES VENTES");

    printf("\033[1;33m%-5s %-12s %-20s %12s\033[0m\n", "ID", "Date", "Client", "Montant");
    printf("\033[1;33m---------------------------------------------------------------\033[0m\n");

    for (int i = 0; i < nbVentes; i++) {
        printf("%-5d %-12s %-20s %12.2f DH\n",
               ventes[i].id,
               ventes[i].date,
               ventes[i].client.nom,
               ventes[i].montantTotal);
    }

    printf("\033[1;33m---------------------------------------------------------------\033[0m\n");
    printf("Total : %d vente(s)\n", nbVentes);

    ui_pause();
}

// ----------------------------
// Afficher ventes par date
void afficherVentesParDate() {
    if (nbVentes == 0) {
        ui_warning("Aucune vente enregistrée !");
        return;
    }

    char date[11];
    ui_input_prompt("Date (JJ/MM/AAAA)");
    lireString(date, 11);

    float total = 0;
    int trouve = 0;

    ui_title("VENTES PAR DATE");

    printf("\033[1;33m%-5s %-20s %-12s %12s\033[0m\n", "ID", "Client", "Heure", "Montant");
    printf("\033[1;33m---------------------------------------------------------------\033[0m\n");

    for (int i = 0; i < nbVentes; i++) {
        if (strcmp(ventes[i].date, date) == 0) {
            printf("%-5d %-20s %-12s %12.2f DH\n",
                   ventes[i].id,
                   ventes[i].client.nom,
                   ventes[i].heure,
                   ventes[i].montantTotal);
            total += ventes[i].montantTotal;
            trouve = 1;
        }
    }

    printf("\033[1;33m---------------------------------------------------------------\033[0m\n");
    if (!trouve) ui_warning("Aucune vente pour cette date !");
    else printf("Total des ventes : %.2f DH\n", total);

    ui_pause();
}

// ----------------------------
// Afficher ventes par client
void afficherVentesParClient() {
    if (nbVentes == 0) {
        ui_warning("Aucune vente enregistrée !");
        return;
    }

    char nom[30];
    ui_input_prompt("Nom du client");
    lireString(nom, 30);

    float total = 0;
    int count = 0;

    ui_title("VENTES PAR CLIENT");

    printf("\033[1;33m%-5s %-12s %-12s %12s\033[0m\n", "ID", "Date", "Heure", "Montant");
    printf("\033[1;33m---------------------------------------------------------------\033[0m\n");

    for (int i = 0; i < nbVentes; i++) {
        if (strstr(ventes[i].client.nom, nom) || strstr(ventes[i].client.prenom, nom)) {
            printf("%-5d %-12s %-12s %12.2f DH\n",
                   ventes[i].id,
                   ventes[i].date,
                   ventes[i].heure,
                   ventes[i].montantTotal);
            total += ventes[i].montantTotal;
            count++;
        }
    }

    printf("\033[1;33m---------------------------------------------------------------\033[0m\n");
    if (count == 0) ui_warning("Aucune vente pour ce client !");
    else {
        printf("Nombre de ventes : %d\n", count);
        printf("Total des achats : %.2f DH\n", total);
    }

    ui_pause();
}

// ----------------------------
// Statistiques des ventes
void statistiquesVentes() {
    if (nbVentes == 0) {
        ui_warning("Aucune vente enregistrée !");
        return;
    }

    float total = 0, maxV = ventes[0].montantTotal, minV = ventes[0].montantTotal;
    int idMax = ventes[0].id, idMin = ventes[0].id;

    for (int i = 0; i < nbVentes; i++) {
        float t = ventes[i].montantTotal;
        total += t;
        if (t > maxV) { maxV = t; idMax = ventes[i].id; }
        if (t < minV) { minV = t; idMin = ventes[i].id; }
    }

    ui_title("STATISTIQUES DES VENTES");

    printf("\033[1;36mNombre total de ventes :\033[0m %d\n", nbVentes);
    printf("\033[1;36mChiffre d'affaires total :\033[0m %.2f DH\n", total);
    printf("\033[1;36mMontant moyen par vente :\033[0m %.2f DH\n", total / nbVentes);
    printf("\033[1;36mVente maximale :\033[0m %.2f DH (ID %d)\n", maxV, idMax);
    printf("\033[1;36mVente minimale :\033[0m %.2f DH (ID %d)\n", minV, idMin);

    ui_pause();
}

// ----------------------------
// Rechercher une vente par ID
void rechercherVente() {
    if (nbVentes == 0) {
        ui_warning("Aucune vente enregistrée !");
        return;
    }

    int id;
    ui_input_prompt("ID de la vente");
    scanf("%d", &id);
    clearBuffer();

    int pos = trouverIndiceVenteParId(id);
    if (pos == -1) ui_error("Vente introuvable !");
    else afficherRecu(&ventes[pos]);
}
// ------------------------------------------------------
// Menu Ventes avec couleurs et UI
// ------------------------------------------------------
void menuVentes() {
    int choix;
    const char *options[] = {
        "Effectuer une nouvelle vente",
        "Rechercher une vente",
        "Afficher les ventes par date",
        "Afficher les ventes par client",
        "Statistiques des ventes",
        "Modifier une vente (client)",
        "Supprimer une vente",
        "Afficher toutes les ventes",
        "Retour"
    };

    do {
        ui_clear();
        ui_logo();
        ui_title("Gestion des Ventes");
        choix = ui_menu("Menu Ventes", options, 9);

        switch (choix) {
               case 1: {
    if (nbVentes >= MAX_VENTES) {
        ui_error("Capacite maximale atteinte.");
    } else {
        // Utiliser la nouvelle fonction avec gestion des commandes
        Vente v = effectuerNouvelleVenteAvecCommande();
        if (v.id != 0) {
            ventes[nbVentes++] = v;
            sauvegarderVentes();
            ui_success("Vente enregistree !");
        }
    }
    ui_pause();

                break;
            }
            case 2: rechercherVente(); break;
            case 3: afficherVentesParDate(); break;
            case 4: afficherVentesParClient(); break;
            case 5: statistiquesVentes(); break;
            case 6: modifierVente(); break;
            case 7: supprimerVente(); break;
            case 8: afficherToutesLesVentes(); break;
            case 9:
            case 0: break;
            default: ui_warning("Choix invalide !"); ui_pause();
        }
    } while (choix != 0 && choix != 9);
}


/* ----------------------------------------------------
   CREATION D'UNE NOUVELLE VENTE
   ---------------------------------------------------- */

Vente effectuerNouvelleVente() {
    Vente v;
    v.id = nbVentes + 1;
    v.nbLignes = 0;
    v.montantTotal = 0;
    v.montantPaye = 0;
    v.monnaieRendue = 0;
    v.modePaiement[0] = '\0';

    obtenirDateActuelle(v.date, v.heure);

    printf("\n===== NOUVELLE VENTE =====\n");

    /* ---- CLIENT ---- */
    printf("ID du client (0 = inconnu) : ");
    int idClient;
    scanf("%d", &idClient);
    clearBuffer();

    if (idClient == 0) {
        v.client.id = 0;
        strcpy(v.client.nom, "Inconnu");
        strcpy(v.client.prenom, "");
        strcpy(v.client.telephone, "");
    } else {
        int pos = rechercherClient(idClient);
        if (pos >= 0) {
            v.client = clients[pos];
            printf("Client trouve : %s %s\n", v.client.nom, v.client.prenom);
        } else {
            printf("Client non trouve. Creation d'un nouveau client.\n");
            v.client.id = idClient;
            printf("Nom : ");   lireString(v.client.nom, 30);
            printf("Prenom : ");lireString(v.client.prenom, 30);
            printf("Tel : ");   lireString(v.client.telephone, 20);

            // Ajouter le nouveau client
            if (nbClients < 100) {
                clients[nbClients++] = v.client;
                sauvegarderClients();
            }
        }
    }

    /* ---- LIGNES ---- */
    char continuer = 'o';
    while (continuer == 'o' || continuer == 'O') {
        if (v.nbLignes >= MAX_LIGNES_VENTE) {
            printf("Nombre maximum de lignes atteint.\n");
            break;
        }

        LigneVente l;
        printf("\n--- Article %d ---\n", v.nbLignes + 1);
        printf("ID medicament : ");
        scanf("%d", &l.idMedicament);
        clearBuffer();

        // Utiliser la fonction estExiste pour vÃƒÂ©rifier l'existence
        int idx = estExiste(l.idMedicament);

        if (idx == -1) {
            printf("Medicament introuvable.\n");
            printf("Continuer ? (o/n) : ");
            scanf(" %c", &continuer);
            clearBuffer();
            continue;
        }

        Medicament *m = &med[idx];

        printf("Medicament : %s\n", m->nom_commercial);
        printf("Stock disponible : %d\n", m->stock_actuel);
        printf("Prix unitaire : %.2f DH\n", m->prix_vente);

        printf("Quantite : ");
        scanf("%d", &l.quantite);
        clearBuffer();

        if (l.quantite <= 0) {
            printf("Quantite invalide.\n");
            continue;
        }

        if (l.quantite > m->stock_actuel) {
            printf("Stock insuffisant ! Stock disponible : %d\n", m->stock_actuel);
            continue;
        }

        // Mettre ÃƒÂ  jour le stock
        m->stock_actuel -= l.quantite;
        sauvegarderMed();

        // Enregistrer le prix au moment de la vente
        l.prixUnitaire = m->prix_vente;
        l.sousTotal = l.quantite * l.prixUnitaire;

        v.lignes[v.nbLignes++] = l;
        v.montantTotal += l.sousTotal;

        printf("Sous-total : %.2f DH\n", l.sousTotal);
        printf("Total actuel : %.2f DH\n", v.montantTotal);

        printf("\nAjouter un autre article ? (o/n) : ");
        scanf(" %c", &continuer);
        clearBuffer();
    }

    if (v.nbLignes == 0) {
        printf("Aucun article ajoute. Vente annulee.\n");
        Vente vide;
        memset(&vide, 0, sizeof(Vente));
        return vide;
    }

    /* ---- PAIEMENT ---- */
    printf("\n========== PAIEMENT ==========\n");
    printf("Montant total : %.2f DH\n", v.montantTotal);
    printf("Mode paiement :\n");
    printf("  1. Especes\n");
    printf("  2. Carte bancaire\n");
    printf("  3. Cheque\n");
    printf("Votre choix : ");

    int mode;
    scanf("%d", &mode);
    clearBuffer();

    if (mode == 1) {
        strcpy(v.modePaiement, "ESPECES");
        printf("Montant donne par le client : ");
        scanf("%f", &v.montantPaye);
        clearBuffer();

        if (v.montantPaye < v.montantTotal) {
            printf("Montant insuffisant ! Vente annulee.\n");

            // Restaurer les stocks
            for (int i = 0; i < v.nbLignes; i++) {
                int idx = estExiste(v.lignes[i].idMedicament);
                if (idx != -1) {
                    med[idx].stock_actuel += v.lignes[i].quantite;
                }
            }
            sauvegarderMed();

            Vente vide;
            memset(&vide, 0, sizeof(Vente));
            return vide;
        }

        v.monnaieRendue = v.montantPaye - v.montantTotal;
        printf("Monnaie a rendre : %.2f DH\n", v.monnaieRendue);

    } else if (mode == 2) {
        strcpy(v.modePaiement, "CARTE");
        v.montantPaye = v.montantTotal;
        v.monnaieRendue = 0;
        printf("Paiement par carte accepte.\n");

    } else if (mode == 3) {
        strcpy(v.modePaiement, "CHEQUE");
        v.montantPaye = v.montantTotal;
        v.monnaieRendue = 0;
        printf("Paiement par cheque accepte.\n");

    } else {
        printf("Mode de paiement invalide. Vente annulee.\n");

        // Restaurer les stocks
        for (int i = 0; i < v.nbLignes; i++) {
            int idx = estExiste(v.lignes[i].idMedicament);
            if (idx != -1) {
                med[idx].stock_actuel += v.lignes[i].quantite;
            }
        }
        sauvegarderMed();

        Vente vide;
        memset(&vide, 0, sizeof(Vente));
        return vide;
    }

    // Afficher le reÃƒÂ§u
    afficherRecu(&v);

    return v;
}

/* ----------------------------------------------------
   MODIFIER UNE VENTE (CLIENT UNIQUEMENT)
   ---------------------------------------------------- */
void modifierVente() {
    if (nbVentes == 0) {
        printf("Aucune vente enregistree.\n");
        pauseConsole();
        return;
    }

    int id;
    printf("ID de la vente a modifier : ");
    scanf("%d", &id);
    clearBuffer();

    int pos = trouverIndiceVenteParId(id);
    if (pos == -1) {
        printf("Vente introuvable.\n");
        pauseConsole();
        return;
    }

    Vente *v = &ventes[pos];
    printf("\n--- Modification de la vente %d ---\n", v->id);
    printf("Client actuel : %s %s\n", v->client.nom, v->client.prenom);

    printf("\nNouveau ID client (0 pour laisser inchange) : ");
    int newId;
    scanf("%d", &newId);
    clearBuffer();

    if (newId > 0) {
        int pos = rechercherClient(newId);
        if (pos >= 0) {
            v->client = clients[pos];
            printf("Client modifie avec succes.\n");
        } else {
            printf("Client non trouve.\n");
        }
    }

    pauseConsole();
}

/* ----------------------------------------------------
   SUPPRESSION
   ---------------------------------------------------- */
void supprimerVente() {
    if (nbVentes == 0) {
        printf("Aucune vente enregistree.\n");
        pauseConsole();
        return;
    }

    int id;
    printf("ID de la vente a supprimer : ");
    scanf("%d", &id);
    clearBuffer();

    int pos = trouverIndiceVenteParId(id);
    if (pos == -1) {
        printf("Vente introuvable.\n");
        pauseConsole();
        return;
    }

    printf("Confirmer la suppression de la vente %d ? (o/n) : ", id);
    char confirm;
    scanf(" %c", &confirm);
    clearBuffer();

    if (confirm == 'o' || confirm == 'O') {
        for (int i = pos; i < nbVentes - 1; i++)
            ventes[i] = ventes[i + 1];

        nbVentes--;
        printf("Vente supprimee avec succes.\n");
        sauvegarderVentes(); // penser à sauvegarder après suppression
    } else {
        printf("Suppression annulee.\n");
    }

    pauseConsole();
}

Vente effectuerNouvelleVenteAvecCommande() {
    Vente v;
    v.id = nbVentes + 1;
    v.nbLignes = 0;
    v.montantTotal = 0;
    v.montantPaye = 0;
    v.monnaieRendue = 0;
    v.modePaiement[0] = '\0';

    obtenirDateActuelle(v.date, v.heure);

    ui_clear();
    ui_logo();
    ui_title("NOUVELLE VENTE");

    /* ---- CLIENT ---- */
    printf("ID du client (0 = inconnu) : ");
    int idClient;
    scanf("%d", &idClient);
    clearBuffer();

    if (idClient == 0) {
        v.client.id = 0;
        strcpy(v.client.nom, "Inconnu");
        strcpy(v.client.prenom, "");
        strcpy(v.client.telephone, "");
    } else {
        int pos = rechercherClient(idClient);
        if (pos >= 0) {
            v.client = clients[pos];
            printf("Client trouve : %s %s\n", v.client.nom, v.client.prenom);
        } else {
            printf("Client non trouve. Creation d'un nouveau client.\n");
            v.client.id = idClient;
            printf("Nom : ");   lireString(v.client.nom, 30);
            printf("Prenom : ");lireString(v.client.prenom, 30);
            printf("Tel : ");   lireString(v.client.telephone, 20);

            if (nbClients < 100) {
                clients[nbClients++] = v.client;
                sauvegarderClients();
            }
        }
    }


    char continuer = 'o';
    int medicamentsManquants = 0;

    while (continuer == 'o' || continuer == 'O') {
        if (v.nbLignes >= MAX_LIGNES_VENTE) {
            printf("Nombre maximum de lignes atteint.\n");
            break;
        }

        LigneVente l;
        printf("\n--- Article %d ---\n", v.nbLignes + 1);
        printf("ID medicament : ");
        scanf("%d", &l.idMedicament);
        clearBuffer();

        int idx = estExiste(l.idMedicament);

        if (idx == -1) {
            /* MÉDICAMENT INTROUVABLE */
            printf("\n\033[1;31m[ATTENTION] Medicament introuvable dans le stock !\033[0m\n");
            printf("ID recherche : %d\n", l.idMedicament);

            printf("\nOptions disponibles :\n");
            printf("1. Passer une commande au fournisseur\n");
            printf("2. Rechercher par nom\n");
            printf("3. Ignorer cet article\n");
            printf("Votre choix : ");

            int choixAction;
            scanf("%d", &choixAction);
            clearBuffer();

            if (choixAction == 1) {
                /* COMMANDER LE MÉDICAMENT MANQUANT */
                printf("\n\033[1;33m=== COMMANDE MEDICAMENT MANQUANT ===\033[0m\n");

                char nomClient[60];
                if (v.client.id > 0) {
                    sprintf(nomClient, "%s %s", v.client.nom, v.client.prenom);
                } else {
                    strcpy(nomClient, "Client inconnu");
                }

                printf("Une commande va etre passee pour le client : %s\n", nomClient);
                printf("Appuyez sur ENTREE pour continuer...");
                clearBuffer();

                /* Appeler la fonction de commande */
                passerCommandeMedicamentManquant(v.client.id, nomClient);

                medicamentsManquants++;

                printf("\n\033[1;32m[OK] Commande enregistree !\033[0m\n");
                printf("Le medicament sera disponible apres reception de la commande.\n");
                printf("Voulez-vous continuer la vente avec d'autres articles ? (o/n) : ");
                scanf(" %c", &continuer);
                clearBuffer();
                continue;

            } else if (choixAction == 2) {
                /* RECHERCHE PAR NOM */
                char nomRecherche[50];
                printf("Nom du medicament a rechercher : ");
                lireString(nomRecherche, 50);

                printf("\n\033[1;36m=== RESULTATS DE LA RECHERCHE ===\033[0m\n");
                int trouve = 0;
                for (int i = 0; i < nbMed; i++) {
                    if (med[i].stock_actuel > 0 &&
                        strstr(med[i].nom_commercial, nomRecherche) != NULL) {
                        printf("ID: %d | Nom: %s | Stock: %d | Prix: %.2f DH\n",
                            med[i].medicament_id,
                            med[i].nom_commercial,
                            med[i].stock_actuel,
                            med[i].prix_vente);
                        trouve = 1;
                    }
                }

                if (trouve) {
                    printf("\nEntrez l'ID du medicament a ajouter (0 pour annuler) : ");
                    scanf("%d", &l.idMedicament);
                    clearBuffer();

                    if (l.idMedicament > 0) {
                        idx = estExiste(l.idMedicament);
                        if (idx != -1) {
                            // Continuer le traitement normal
                        } else {
                            printf("ID invalide.\n");
                            continue;
                        }
                    } else {
                        continue;
                    }
                } else {
                    printf("\033[1;31mAucun medicament trouve.\033[0m\n");
                    printf("Voulez-vous passer une commande ? (o/n) : ");
                    char cmd;
                    scanf(" %c", &cmd);
                    clearBuffer();

                    if (cmd == 'o' || cmd == 'O') {
                        char nomClient[60];
                        if (v.client.id > 0) {
                            sprintf(nomClient, "%s %s", v.client.nom, v.client.prenom);
                        } else {
                            strcpy(nomClient, "Client inconnu");
                        }
                        passerCommandeMedicamentManquant(v.client.id, nomClient);
                        medicamentsManquants++;
                    }
                    continue;
                }
            } else {
                /* IGNORER */
                printf("Article ignore.\n");
                printf("Continuer la vente ? (o/n) : ");
                scanf(" %c", &continuer);
                clearBuffer();
                continue;
            }
        }

        /* TRAITEMENT NORMAL DU MÉDICAMENT (existant et trouvé) */
        if (idx != -1) {
            Medicament *m = &med[idx];

            printf("\n\033[1;32mMedicament : %s\033[0m\n", m->nom_commercial);
            printf("Stock disponible : %d\n", m->stock_actuel);
            printf("Prix unitaire : %.2f DH\n", m->prix_vente);

            printf("Quantite : ");
            scanf("%d", &l.quantite);
            clearBuffer();

            if (l.quantite <= 0) {
                printf("Quantite invalide.\n");
                continue;
            }

            if (l.quantite > m->stock_actuel) {
                printf("\n\033[1;31m[STOCK INSUFFISANT]\033[0m\n");
                printf("Stock disponible : %d\n", m->stock_actuel);
                printf("Quantite demandee : %d\n", l.quantite);

                printf("\nOptions :\n");
                printf("1. Commander la quantite manquante (%d unites)\n",
                    l.quantite - m->stock_actuel);
                printf("2. Vendre uniquement le stock disponible (%d unites)\n",
                    m->stock_actuel);
                printf("3. Annuler cet article\n");
                printf("Votre choix : ");

                int choixStock;
                scanf("%d", &choixStock);
                clearBuffer();

                if (choixStock == 1) {
                    char nomClient[60];
                    if (v.client.id > 0) {
                        sprintf(nomClient, "%s %s", v.client.nom, v.client.prenom);
                    } else {
                        strcpy(nomClient, "Client inconnu");
                    }

                    printf("\nCommande de %d unites de %s\n",
                        l.quantite - m->stock_actuel, m->nom_commercial);
                    passerCommandeMedicamentManquant(v.client.id, nomClient);
                    medicamentsManquants++;

                    // Vendre ce qui est disponible
                    l.quantite = m->stock_actuel;
                    if (l.quantite == 0) {
                        printf("Aucun stock disponible pour vente immediate.\n");
                        continue;
                    }

                } else if (choixStock == 2) {
                    l.quantite = m->stock_actuel;

                } else {
                    continue;
                }
            }

            // Mise à jour du stock
            m->stock_actuel -= l.quantite;
            sauvegarderMed();

            // Enregistrer la ligne de vente
            l.prixUnitaire = m->prix_vente;
            l.sousTotal = l.quantite * l.prixUnitaire;

            v.lignes[v.nbLignes++] = l;
            v.montantTotal += l.sousTotal;

            printf("\n\033[1;32mSous-total : %.2f DH\033[0m\n", l.sousTotal);
            printf("\033[1;32mTotal actuel : %.2f DH\033[0m\n", v.montantTotal);
        }

        printf("\nAjouter un autre article ? (o/n) : ");
        scanf(" %c", &continuer);
        clearBuffer();
    }

    /* RÉSUMÉ DES COMMANDES PASSÉES */
    if (medicamentsManquants > 0) {
        printf("\n\033[1;33m========================================\033[0m\n");
        printf("\033[1;33m[INFO] %d commande(s) fournisseur passee(s)\033[0m\n",
            medicamentsManquants);
        printf("\033[1;33mLes medicaments seront disponibles apres reception.\033[0m\n");
        printf("\033[1;33m========================================\033[0m\n\n");
    }

    /* VÉRIFICATION : Y a-t-il des articles à vendre ? */
    if (v.nbLignes == 0) {
        if (medicamentsManquants > 0) {
            printf("\n\033[1;36mAucun article disponible pour vente immediate.\033[0m\n");
            printf("\033[1;36mLes commandes ont ete enregistrees.\033[0m\n");
            printf("\033[1;36mLe client sera notifie lors de la reception.\033[0m\n");
        } else {
            printf("\nAucun article ajoute. Vente annulee.\n");
        }

        Vente vide;
        memset(&vide, 0, sizeof(Vente));
        return vide;
    }

    /* ---- PAIEMENT ---- */
    printf("\n========== PAIEMENT ==========\n");
    printf("Montant total : %.2f DH\n", v.montantTotal);
    printf("Mode paiement :\n");
    printf("  1. Especes\n");
    printf("  2. Carte bancaire\n");
    printf("  3. Cheque\n");
    printf("Votre choix : ");

    int mode;
    scanf("%d", &mode);
    clearBuffer();

    if (mode == 1) {
        strcpy(v.modePaiement, "ESPECES");
        printf("Montant donne par le client : ");
        scanf("%f", &v.montantPaye);
        clearBuffer();

        if (v.montantPaye < v.montantTotal) {
            printf("Montant insuffisant ! Vente annulee.\n");

            // Restaurer les stocks
            for (int i = 0; i < v.nbLignes; i++) {
                int idx = estExiste(v.lignes[i].idMedicament);
                if (idx != -1) {
                    med[idx].stock_actuel += v.lignes[i].quantite;
                }
            }
            sauvegarderMed();

            Vente vide;
            memset(&vide, 0, sizeof(Vente));
            return vide;
        }

        v.monnaieRendue = v.montantPaye - v.montantTotal;
        printf("Monnaie a rendre : %.2f DH\n", v.monnaieRendue);

    } else if (mode == 2) {
        strcpy(v.modePaiement, "CARTE");
        v.montantPaye = v.montantTotal;
        v.monnaieRendue = 0;
        printf("Paiement par carte accepte.\n");

    } else if (mode == 3) {
        strcpy(v.modePaiement, "CHEQUE");
        v.montantPaye = v.montantTotal;
        v.monnaieRendue = 0;
        printf("Paiement par cheque accepte.\n");

    } else {
        printf("Mode de paiement invalide. Vente annulee.\n");

        // Restaurer les stocks
        for (int i = 0; i < v.nbLignes; i++) {
            int idx = estExiste(v.lignes[i].idMedicament);
            if (idx != -1) {
                med[idx].stock_actuel += v.lignes[i].quantite;
            }
        }
        sauvegarderMed();

        Vente vide;
        memset(&vide, 0, sizeof(Vente));
        return vide;
    }

    // Afficher le reçu
    afficherRecu(&v);

    if (medicamentsManquants > 0) {
        printf("\n\033[1;33m[RAPPEL] %d medicament(s) commande(s)\033[0m\n",
            medicamentsManquants);
        printf("\033[1;33mLe client sera contacte lors de la reception.\033[0m\n");
    }

    return v;
}

/* ============================================================================
   MODIFICATION DU MENU VENTES
   ============================================================================
   Remplacez le case 1 dans menuVentes() par :
*/

/*
case 1: {
    if (nbVentes >= MAX_VENTES) {
        ui_error("Capacite maximale atteinte.");
    } else {
        // Utiliser la nouvelle fonction avec gestion des commandes
        Vente v = effectuerNouvelleVenteAvecCommande();
        if (v.id != 0) {
            ventes[nbVentes++] = v;
            sauvegarderVentes();
            ui_success("Vente enregistree !");
        }
    }
    ui_pause();
    break;
}
*/

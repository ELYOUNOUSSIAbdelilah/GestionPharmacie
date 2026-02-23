#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "medicaments.h"
#include "utils.h"
#include "ui.h"
Medicament med[MAX_MED];
int nbMed = 0;

/* =================== Sauvegarde =================== */
void sauvegarderMed() {
    FILE *f = fopen("medicaments.txt", "w");
    if (!f) { ui_error("Erreur lors de la sauvegarde !"); return; }

    fprintf(f, "%d\n", nbMed);
    for (int i = 0; i < nbMed; i++) {
        fprintf(f,
            "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d;%d;%s;%.2f;%.2f;%s;%s\n",
            med[i].medicament_id,
            med[i].code_barre,
            med[i].nom_commercial,
            med[i].nom_scientifique,
            med[i].categorie,
            med[i].forme_pharmaceutique,
            med[i].dosage,
            med[i].unite_dosage,
            med[i].presentation,
            med[i].fabricant,
            med[i].pays_origine,
            med[i].date_fabrication,
            med[i].date_expiration,
            med[i].stock_actuel,
            med[i].stock_min,
            med[i].emplacement_stock,
            med[i].prix_achat,
            med[i].prix_vente,
            med[i].mode_administration,
            med[i].ordonnance_obligatoire
        );
    }
    fclose(f);
}

/* =================== Chargement =================== */
void chargerMed() {
    FILE *f = fopen("medicaments.txt", "r");
    if (!f) { nbMed = 0; return; }

    char line[512];
    if (fgets(line, sizeof(line), f) == NULL) { nbMed = 0; fclose(f); return; }
    nbMed = atoi(line);

    for (int i = 0; i < nbMed; i++) {
        if (fgets(line, sizeof(line), f) == NULL) break;

        char tmp_stock[10], tmp_stock_min[10], tmp_prix_achat[20], tmp_prix_vente[20];
        sscanf(line,
            "%d;%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];"
            "%49[^;];%49[^;];%49[^;];%49[^;];%[^;];%[^;];%49[^;];%[^;];%[^;];%49[^;];%49[^\n]",
            &med[i].medicament_id,
            med[i].code_barre,
            med[i].nom_commercial,
            med[i].nom_scientifique,
            med[i].categorie,
            med[i].forme_pharmaceutique,
            med[i].dosage,
            med[i].unite_dosage,
            med[i].presentation,
            med[i].fabricant,
            med[i].pays_origine,
            med[i].date_fabrication,
            med[i].date_expiration,
            tmp_stock,
            tmp_stock_min,
            med[i].emplacement_stock,
            tmp_prix_achat,
            tmp_prix_vente,
            med[i].mode_administration,
            med[i].ordonnance_obligatoire
        );

        med[i].stock_actuel = atoi(tmp_stock);
        med[i].stock_min = atoi(tmp_stock_min);
        med[i].prix_achat = atof(tmp_prix_achat);
        med[i].prix_vente = atof(tmp_prix_vente);
    }

    fclose(f);
}

/* =================== Vérifier existence =================== */
int estExiste(int id) {
    for (int i = 0; i < nbMed; i++)
        if (med[i].medicament_id == id) return i;
    return -1;
}

/* =================== Ajouter médicament =================== */
void ajouterMed() {
    if (nbMed >= MAX_MED) { ui_error("Stock plein !"); return; }

    Medicament nouveau;
    char tmp[50];

    ui_header("Ajout d'un Medicament");

    ui_input_prompt("ID du medicament");
    scanf("%d", &nouveau.medicament_id); getchar();

    int idx = estExiste(nouveau.medicament_id);
    if (idx != -1) {
        int q;
        printf("Ce medicament existe deja. Quantite a ajouter : ");
        scanf("%d", &q); getchar();
        med[idx].stock_actuel += q;
        sauvegarderMed();
        ui_success("Stock mis a jour !");
        return;
    }

    ui_input_prompt("Code barre"); lireString(nouveau.code_barre, MAX_CHAR);
    ui_input_prompt("Nom commercial"); lireString(nouveau.nom_commercial, MAX_CHAR);
    ui_input_prompt("Nom scientifique"); lireString(nouveau.nom_scientifique, MAX_CHAR);
    ui_input_prompt("Categorie"); lireString(nouveau.categorie, MAX_CHAR);
    ui_input_prompt("Forme pharmaceutique"); lireString(nouveau.forme_pharmaceutique, MAX_CHAR);
    ui_input_prompt("Dosage"); lireString(nouveau.dosage, MAX_CHAR);
    ui_input_prompt("Unite dosage"); lireString(nouveau.unite_dosage, MAX_CHAR);
    ui_input_prompt("Presentation"); lireString(nouveau.presentation, MAX_CHAR);
    ui_input_prompt("Fabricant"); lireString(nouveau.fabricant, MAX_CHAR);
    ui_input_prompt("Pays origine"); lireString(nouveau.pays_origine, MAX_CHAR);
    ui_input_prompt("Date fabrication"); lireString(nouveau.date_fabrication, MAX_CHAR);
    ui_input_prompt("Date expiration"); lireString(nouveau.date_expiration, MAX_CHAR);

    ui_input_prompt("Stock actuel"); lireString(tmp, 10); nouveau.stock_actuel = atoi(tmp);
    ui_input_prompt("Stock minimum"); lireString(tmp, 10); nouveau.stock_min = atoi(tmp);
    ui_input_prompt("Emplacement stock"); lireString(nouveau.emplacement_stock, MAX_CHAR);
    ui_input_prompt("Prix achat"); lireString(tmp, 20); nouveau.prix_achat = atof(tmp);
    ui_input_prompt("Prix vente"); lireString(tmp, 20); nouveau.prix_vente = atof(tmp);
    ui_input_prompt("Mode administration"); lireString(nouveau.mode_administration, MAX_CHAR);
    ui_input_prompt("Ordonnance obligatoire (oui/non)"); lireString(nouveau.ordonnance_obligatoire, MAX_CHAR);

    med[nbMed++] = nouveau;
    sauvegarderMed();
    ui_success("Medicament ajoute avec succes !");
}

/* =================== Affichage tableau =================== */
void afficherTousMed() {
    if (nbMed == 0) { ui_warning("Aucun medicament !"); return; }

    const char *columns[] = {
        "ID", "Nom", "Categorie", "Stock", "Stock min", "Prix achat", "Prix vente", "Mode admin", "Ordonnance"
    };
    int widths[] = {5, 20, 15, 7, 10, 10, 10, 15, 12};
    int colCount = 9;

    ui_table_header(columns, colCount, widths);

    char bufId[10], bufStock[10], bufStockMin[10], bufPrixA[10], bufPrixV[10];
    for (int i = 0; i < nbMed; i++) {
        char *values[colCount];

        snprintf(bufId, sizeof(bufId), "%d", med[i].medicament_id);
        snprintf(bufStock, sizeof(bufStock), "%d", med[i].stock_actuel);
        snprintf(bufStockMin, sizeof(bufStockMin), "%d", med[i].stock_min);
        snprintf(bufPrixA, sizeof(bufPrixA), "%.2f", med[i].prix_achat);
        snprintf(bufPrixV, sizeof(bufPrixV), "%.2f", med[i].prix_vente);

        values[0] = bufId;
        values[1] = med[i].nom_commercial;
        values[2] = med[i].categorie;
        values[3] = bufStock;
        values[4] = bufStockMin;
        values[5] = bufPrixA;
        values[6] = bufPrixV;
        values[7] = med[i].mode_administration;
        values[8] = med[i].ordonnance_obligatoire;

        ui_table_row(values, colCount, widths);
    }
    ui_table_separator(widths, colCount);
}

/* =================== Modifier medicament =================== */
void modifierMed() {
    int id;
    ui_input_prompt("ID du medicament a modifier");
    scanf("%d", &id); getchar();

    int idx = estExiste(id);
    if (idx == -1) { ui_error("Medicament non trouve !"); return; }

    ui_input_prompt("Modifier nom commercial"); lireString(med[idx].nom_commercial, MAX_CHAR);
    ui_input_prompt("Modifier categorie"); lireString(med[idx].categorie, MAX_CHAR);

    char tmp[20];
    ui_input_prompt("Modifier prix achat"); lireString(tmp, 20); med[idx].prix_achat = atof(tmp);
    ui_input_prompt("Modifier prix vente"); lireString(tmp, 20); med[idx].prix_vente = atof(tmp);

    sauvegarderMed();
    ui_success("Medicament modifie avec succes !");
}

/* =================== Rechercher =================== */
Medicament* chercherMedParId(int id) {
    int idx = estExiste(id);
    if (idx != -1) return &med[idx];
    return NULL;
}

void chercherMedParNom(char *nom) {
    int trouve = 0;
    for (int i = 0; i < nbMed; i++) {
        if (med[i].stock_actuel > 0 && strstr(med[i].nom_commercial, nom) != NULL) {
            ui_header("Medicament trouve");
            char buf[256];
            sprintf(buf,"ID: %d | Nom: %s | Categorie: %s | Stock: %d | Stock min: %d | Prix achat: %.2f | Prix vente: %.2f | Mode: %s | Ordonnance: %s",
                    med[i].medicament_id, med[i].nom_commercial, med[i].categorie,
                    med[i].stock_actuel, med[i].stock_min, med[i].prix_achat, med[i].prix_vente,
                    med[i].mode_administration, med[i].ordonnance_obligatoire);
            ui_info(buf);
            trouve = 1;
        }
    }
    if (!trouve) ui_warning("Aucun medicament trouve !");
}

/* =================== Statistiques =================== */
int nombreMedicaments() { return nbMed; }

int nombreMedParCat(char *cat) {
    int c = 0;
    for (int i = 0; i < nbMed; i++)
        if (strcmp(med[i].categorie, cat) == 0 && med[i].stock_actuel > 0) c++;
    return c;
}

float prixMoyenAchat() {
    if (nbMed == 0) return 0;
    float s = 0;
    int count=0;
    for (int i = 0; i < nbMed; i++) {
        if(med[i].stock_actuel>0){ s += med[i].prix_achat; count++; }
    }
    return count>0?s/count:0;
}

Medicament MedPlusCher() {
    Medicament *max = NULL;
    for (int i = 0; i < nbMed; i++) {
        if (med[i].stock_actuel > 0 && (!max || med[i].prix_achat > max->prix_achat))
            max = &med[i];
    }
    if (max) return *max;

    Medicament vide = {0};
    strcpy(vide.nom_commercial, "Aucun");
    return vide;
}

Medicament MedMoinsCher() {
    Medicament *min = NULL;
    for (int i = 0; i < nbMed; i++) {
        if (med[i].stock_actuel > 0 && (!min || med[i].prix_achat < min->prix_achat))
            min = &med[i];
    }
    if (min) return *min;

    Medicament vide = {0};
    strcpy(vide.nom_commercial, "Aucun");
    return vide;
}

/* =================== Menu gestion medicaments =================== */
void gestionMedicamentsMenu() {
    int medChoice;
    do {
        ui_clear();
        ui_logo();
        ui_title("Gestion Medicaments");

        const char *options[] = {
            "Afficher tous les medicaments",
            "Ajouter un medicament",
            "Modifier un medicament",
            "Rechercher un medicament par ID",
            "Rechercher un medicament par nom",
            "Statistiques",
            "Retour"
        };
        medChoice = ui_menu("Menu Medicaments", options, 7);

        switch(medChoice) {
            case 1:
                afficherTousMed();
                ui_pause();
                break;
            case 2:
                ajouterMed();
                ui_pause();
                break;
            case 3:
                modifierMed();
                ui_pause();
                break;
            case 4: {
                int id;
                ui_input_prompt("ID du medicament a rechercher");
                scanf("%d", &id); getchar();
                Medicament *m = chercherMedParId(id);
                if (m && m->stock_actuel>0) chercherMedParNom(m->nom_commercial);
                else ui_error("Medicament non trouve ou stock vide !");
                ui_pause();
                break;
            }
            case 5: {
                char nom[MAX_CHAR];
                ui_input_prompt("Nom commercial du medicament");
                fgets(nom, MAX_CHAR, stdin);
                nom[strcspn(nom, "\n")] = 0;
                chercherMedParNom(nom);
                ui_pause();
                break;
            }
            case 6: {
                ui_header("Statistiques Medicaments");

                char cat[MAX_CHAR];
                ui_input_prompt("Categorie a analyser");
                fgets(cat, MAX_CHAR, stdin);
                cat[strcspn(cat, "\n")] = 0;

                char buf[200];
                sprintf(buf, "Nombre total de medicaments: %d", nombreMedicaments());
                ui_info(buf);

                sprintf(buf, "Nombre de medicaments en stock pour categorie '%s': %d", cat, nombreMedParCat(cat));
                ui_info(buf);

                if (nbMed > 0) {
                    Medicament maxMed = MedPlusCher();
                    sprintf(buf, "Medicament le plus cher: %s (%.2f DH)", maxMed.nom_commercial, maxMed.prix_achat);
                    ui_info(buf);

                    Medicament minMed = MedMoinsCher();
                    sprintf(buf, "Medicament le moins cher: %s (%.2f DH)", minMed.nom_commercial, minMed.prix_achat);
                    ui_info(buf);

                    sprintf(buf, "Prix moyen d'achat des medicaments en stock: %.2f DH", prixMoyenAchat());
                    ui_info(buf);
                }
                ui_pause();
                break;
            }
            case 7:
                break;
            default:
                ui_warning("Choix invalide !");
                ui_pause();
        }

    } while(medChoice != 7);
}

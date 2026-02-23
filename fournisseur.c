#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fournisseurs.h"
#include "utils.h"
#include "ui.h"
Fournisseur fournisseurs[MAX_FOURNISSEURS];
int nbFournisseurs = 0;

/* =================== Sauvegarde =================== */
void sauvegarderF() {
    FILE *f = fopen("fournisseur.txt", "w");
    if (!f) { ui_error("Erreur lors de la sauvegarde !"); return; }

    fprintf(f, "%d\n", nbFournisseurs);

    for (int i = 0; i < nbFournisseurs; i++) {
        Fournisseur fr = fournisseurs[i];
        fprintf(f,
            "%d;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%d;%d;%s;",
            fr.fournisseur_id, fr.reference_code, fr.raison_sociale,
            fr.contact_prenom, fr.contact_nom, fr.poste, fr.telephone,
            fr.email, fr.site_web, fr.adresse_ligne, fr.ville,
            fr.province_code, fr.code_postal, fr.pays, fr.num_tva,
            fr.forme_juridique, fr.banque_nom, fr.banque_rib, fr.iban,
            fr.delai_livraison_jours, fr.actif, fr.date_inscription
        );

        fprintf(f, "%d;", fr.nbMedicaments);

        for (int j = 0; j < fr.nbMedicaments; j++) {
            fprintf(f, "%d", fr.medicaments_ids[j]);
            if (j < fr.nbMedicaments - 1) fprintf(f, ",");
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

/* =================== Chargement =================== */
void chargerF() {
    FILE *f = fopen("fournisseur.txt", "r");
    if (!f) { nbFournisseurs = 0; return; }

    fscanf(f, "%d\n", &nbFournisseurs);
    for (int i = 0; i < nbFournisseurs; i++) {
        Fournisseur *fr = &fournisseurs[i];
        fscanf(f,
            "%d;%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];"
            "%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];%49[^;];"
            "%49[^;];%49[^;];%d;%d;%49[^;];",
            &fr->fournisseur_id, fr->reference_code, fr->raison_sociale,
            fr->contact_prenom, fr->contact_nom, fr->poste, fr->telephone,
            fr->email, fr->site_web, fr->adresse_ligne, fr->ville,
            fr->province_code, fr->code_postal, fr->pays, fr->num_tva,
            fr->forme_juridique, fr->banque_nom, fr->banque_rib, fr->iban,
            &fr->delai_livraison_jours, &fr->actif, fr->date_inscription
        );

        fscanf(f, "%d;", &fr->nbMedicaments);
        char ligne[256];
        fgets(ligne, sizeof(ligne), f);
        char *token = strtok(ligne, ",\n");
        int j = 0;
        while (token && j < fr->nbMedicaments) {
            fr->medicaments_ids[j++] = atoi(token);
            token = strtok(NULL, ",\n");
        }
    }
    fclose(f);
}

/* =================== Ajouter Fournisseur =================== */
void AjouterF() {
    if (nbFournisseurs >= MAX_FOURNISSEURS) { ui_error("Table pleine !"); return; }

    Fournisseur f;
    ui_header("Ajout Fournisseur");

    ui_input_prompt("ID"); while (scanf("%d", &f.fournisseur_id) != 1) { ui_error("ID invalide !"); while(getchar()!='\n'); } getchar();
    ui_input_prompt("Reference code"); lireString(f.reference_code, MAX_CHAR);
    ui_input_prompt("Raison sociale"); lireString(f.raison_sociale, MAX_CHAR);
    ui_input_prompt("Prenom contact"); lireString(f.contact_prenom, MAX_CHAR);
    ui_input_prompt("Nom contact"); lireString(f.contact_nom, MAX_CHAR);

    do { ui_input_prompt("Telephone"); lireString(f.telephone, MAX_CHAR); } while(!numeroValide(f.telephone));
    do { ui_input_prompt("Email"); lireString(f.email, MAX_CHAR); } while(!isEmail(f.email));

    ui_input_prompt("Site web"); lireString(f.site_web, MAX_CHAR);
    ui_input_prompt("Adresse"); lireString(f.adresse_ligne, MAX_CHAR);
    ui_input_prompt("Ville"); lireString(f.ville, MAX_CHAR);
    ui_input_prompt("Province code"); lireString(f.province_code, MAX_CHAR);
    ui_input_prompt("Code postal"); lireString(f.code_postal, MAX_CHAR);
    ui_input_prompt("Pays"); lireString(f.pays, MAX_CHAR);
    ui_input_prompt("Num TVA"); lireString(f.num_tva, MAX_CHAR);
    ui_input_prompt("Forme juridique"); lireString(f.forme_juridique, MAX_CHAR);
    ui_input_prompt("Banque nom"); lireString(f.banque_nom, MAX_CHAR);
    ui_input_prompt("Banque RIB"); lireString(f.banque_rib, MAX_CHAR);
    ui_input_prompt("IBAN"); lireString(f.iban, MAX_CHAR);
    ui_input_prompt("Date inscription"); lireString(f.date_inscription, MAX_CHAR);

    f.nbMedicaments = 0;
    fournisseurs[nbFournisseurs++] = f;
    sauvegarderF();
    ui_success("Fournisseur ajoute avec succes !");
}

/* =================== Affichage tableau =================== */
void afficherTousF() {
    if (nbFournisseurs == 0) { ui_warning("Aucun fournisseur !"); return; }

    const char *columns[] = {"ID", "Raison sociale", "Contact", "Telephone", "Email", "Nb Medicaments"};
    int widths[] = {5, 20, 20, 15, 25, 8};

    ui_table_header(columns, 6, widths);

    char bufId[10], bufNbMed[10], bufContact[50];
    for (int i = 0; i < nbFournisseurs; i++) {
        Fournisseur fr = fournisseurs[i];
        snprintf(bufId, sizeof(bufId), "%d", fr.fournisseur_id);
        snprintf(bufNbMed, sizeof(bufNbMed), "%d", fr.nbMedicaments);
        snprintf(bufContact, sizeof(bufContact), "%s %s", fr.contact_prenom, fr.contact_nom);

        char *values[6] = {bufId, fr.raison_sociale, bufContact, fr.telephone, fr.email, bufNbMed};
        ui_table_row(values, 6, widths);
    }
    ui_table_separator(widths, 6);
}

/* =================== Modifier Fournisseur =================== */
void modifierF() {
    int id; ui_input_prompt("ID du fournisseur a modifier"); scanf("%d",&id); getchar();
    Fournisseur *f = rechercherF_parID(id);
    if (!f) { ui_error("Fournisseur non trouve !"); return; }

    ui_input_prompt("Modifier raison sociale"); lireString(f->raison_sociale, MAX_CHAR);
    ui_input_prompt("Modifier prenom contact"); lireString(f->contact_prenom, MAX_CHAR);
    ui_input_prompt("Modifier nom contact"); lireString(f->contact_nom, MAX_CHAR);

    do { ui_input_prompt("Modifier telephone"); lireString(f->telephone, MAX_CHAR); } while(!numeroValide(f->telephone));
    do { ui_input_prompt("Modifier email"); lireString(f->email, MAX_CHAR); } while(!isEmail(f->email));

    sauvegarderF();
    ui_success("Fournisseur modifie avec succes !");
}

/* =================== Recherche =================== */
Fournisseur* rechercherF_parID(int id) {
    for (int i = 0; i < nbFournisseurs; i++)
        if (fournisseurs[i].fournisseur_id == id) return &fournisseurs[i];
    return NULL;
}

/* =================== Lier Medicaments =================== */
void lierMedAFournisseur(int fournisseurID, int medicamentID) {
    Fournisseur *f = rechercherF_parID(fournisseurID);
    if (!f) { ui_error("Fournisseur introuvable !"); return; }
    if (f->nbMedicaments >= MAX_MED_PAR_FOURNISSEUR) { ui_warning("Limite atteinte !"); return; }
    f->medicaments_ids[f->nbMedicaments++] = medicamentID;
    sauvegarderF();
    ui_success("Medicament lie au fournisseur !");
}

/* =================== Menu Fournisseurs =================== */
void gestionFournisseurMenu() {
    int fChoice;
    const char *options[] = {
        "Ajouter Fournisseur",
        "Afficher Fournisseurs",
        "Modifier Fournisseur",
        "Supprimer Fournisseur",
        "Rechercher Fournisseur par ID",
        "Retour"
    };

    do {
        ui_clear(); ui_logo(); ui_title("Gestion Fournisseurs");
        fChoice = ui_menu("Menu Fournisseurs", options, 6);

        switch(fChoice) {
            case 1: AjouterF(); ui_pause(); break;
            case 2: afficherTousF(); ui_pause(); break;
            case 3: modifierF(); ui_pause(); break;
            case 4: {
                int id; ui_input_prompt("ID du fournisseur a supprimer"); scanf("%d",&id); getchar();
                Fournisseur *f = rechercherF_parID(id);
                if(f) {
                    int pos = f - fournisseurs;
                    for(int i=pos;i<nbFournisseurs-1;i++) fournisseurs[i]=fournisseurs[i+1];
                    nbFournisseurs--;
                    sauvegarderF();
                    ui_success("Fournisseur supprime !");
                } else ui_error("Fournisseur non trouve !");
                ui_pause();
                break;
            }
            case 5: {
                int id; ui_input_prompt("ID du fournisseur a rechercher"); scanf("%d",&id); getchar();
                Fournisseur *f = rechercherF_parID(id);
                if(f) {
                    ui_header("Fournisseur Trouve");
                    char buf[256];
                    sprintf(buf,"ID : %d", f->fournisseur_id); ui_info(buf);
                    sprintf(buf,"Reference : %s", f->reference_code); ui_info(buf);
                    sprintf(buf,"Raison sociale : %s", f->raison_sociale); ui_info(buf);
                    sprintf(buf,"Contact : %s %s", f->contact_prenom, f->contact_nom); ui_info(buf);
                    sprintf(buf,"Telephone : %s", f->telephone); ui_info(buf);
                    sprintf(buf,"Email : %s", f->email); ui_info(buf);
                    sprintf(buf,"Site web : %s", f->site_web); ui_info(buf);
                    sprintf(buf,"Adresse : %s, %s, %s, %s, %s", f->adresse_ligne,f->ville,f->province_code,f->code_postal,f->pays); ui_info(buf);
                    sprintf(buf,"Num TVA : %s", f->num_tva); ui_info(buf);
                    sprintf(buf,"Forme juridique : %s", f->forme_juridique); ui_info(buf);
                    sprintf(buf,"Banque : %s, RIB : %s, IBAN : %s", f->banque_nom,f->banque_rib,f->iban); ui_info(buf);
                    sprintf(buf,"Date inscription : %s", f->date_inscription); ui_info(buf);
                    sprintf(buf,"Nombre medicaments lies : %d", f->nbMedicaments); ui_info(buf);
                } else ui_error("Fournisseur non trouve !");
                ui_pause();
                break;
            }
            case 6: break;
            default: ui_warning("Choix invalide !"); ui_pause();
        }
    } while(fChoice != 6);
}

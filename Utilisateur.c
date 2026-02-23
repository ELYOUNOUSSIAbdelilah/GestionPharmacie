#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "ui.h"
#define MAX_USERS 100

typedef struct {
    char username[30];
    char password[30];
    int role; // 1 = admin, 0 = user
} User;

// Table globale
User users[MAX_USERS];
int usersCount = 0;
User loggedUser;

//------------------GESTION FICHIER------------------
void saveUsersToFile() {
    FILE *f = fopen("users.txt", "w");
    if (!f) {
        ui_error("Erreur lors de la sauvegarde !");
        return;
    }

    fprintf(f, "%d\n", usersCount);

    for (int i = 0; i < usersCount; i++) {
        fprintf(f, "%s %s %d\n",
                users[i].username,
                users[i].password,
                users[i].role);
    }

    fclose(f);
}

void loadUsersFromFile() {
    FILE *f = fopen("users.txt", "r");
    if (!f) {
        ui_warning("Fichier users.txt non trouve. Creation admin par defaut.");
        usersCount = 0;
        return;
    }

    if (fscanf(f, "%d", &usersCount) != 1) {
        ui_error("Erreur lecture usersCount !");
        usersCount = 0;
        fclose(f);
        return;
    }

    for (int i = 0; i < usersCount; i++) {
        if (fscanf(f, "%s %s %d",
                   users[i].username,
                   users[i].password,
                   &users[i].role) != 3) {
            ui_error("Erreur lecture d'un utilisateur !");
            usersCount = 0;
            break;
        }
    }

    fclose(f);
}

void initDefaultAdmin() {
    int adminExiste = 0;
    for(int i = 0; i < usersCount; i++) {
        if(users[i].role == 1) {
            adminExiste = 1;
            break;
        }
    }

    if(!adminExiste) {
        strcpy(users[0].username, "admin");
        strcpy(users[0].password, "admin");
        users[0].role = 1;
        usersCount = 1;
        saveUsersToFile();
        ui_success("Admin par defaut cree : admin/admin");
    }
}

//------------------CRUD UTILISATEUR------------------
void afficherUsers() {
    if(usersCount == 0) {
        ui_warning("Aucun utilisateur dans la base !");
        return;
    }

    const char *columns[] = {"#", "Username", "Role"};
    int widths[] = {5, 20, 10};

    ui_table_header(columns, 3, widths);

    char bufIndex[10], bufRole[10];
    for(int i = 0; i < usersCount; i++) {
        char *values[3];
        snprintf(bufIndex, sizeof(bufIndex), "%d", i + 1);
        snprintf(bufRole, sizeof(bufRole), "%s", users[i].role ? "Admin" : "User");

        values[0] = bufIndex;
        values[1] = users[i].username;
        values[2] = bufRole;

        ui_table_row(values, 3, widths);
    }
    ui_table_separator(widths, 3);
}

void ajouterUser() {
    if(usersCount >= MAX_USERS) {
        ui_error("Nombre maximal d'utilisateurs atteint !");
        return;
    }

    User newUser;
    char tmp[10];

    ui_header("Ajout d'un Utilisateur");

    ui_input_prompt("Nom d'utilisateur");
    lireString(newUser.username, sizeof(newUser.username));

    ui_input_prompt("Mot de passe");
    lireString(newUser.password, sizeof(newUser.password));

    do {
        ui_input_prompt("Role (1=Admin, 0=User)");
        lireString(tmp, sizeof(tmp));
    } while(!isNumber(tmp) || (atoi(tmp) != 0 && atoi(tmp) != 1));
    newUser.role = atoi(tmp);

    users[usersCount++] = newUser;
    saveUsersToFile();
    ui_success("Utilisateur ajoute avec succes !");
}

void supprimerUser() {
    char username[30];
    ui_input_prompt("Nom d'utilisateur a supprimer");
    lireString(username, sizeof(username));

    int found = 0;
    for(int i = 0; i < usersCount; i++) {
        if(strcmp(users[i].username, username) == 0) {
            found = 1;
            for(int j = i; j < usersCount - 1; j++)
                users[j] = users[j+1];
            usersCount--;
            saveUsersToFile();
            ui_success("Utilisateur supprime avec succes !");
            break;
        }
    }

    if(!found) ui_warning("Utilisateur non trouve !");
}

void modifierMonCompte(User *user) {
    ui_header("Modification de Mon Compte");

    ui_input_prompt("Ancien mot de passe");
    char oldPass[30];
    lireString(oldPass, sizeof(oldPass));

    if(strcmp(oldPass, user->password) != 0) {
        ui_error("Mot de passe incorrect !");
        return;
    }

    ui_input_prompt("Nouveau mot de passe");
    lireString(user->password, sizeof(user->password));

    for(int i = 0; i < usersCount; i++) {
        if(strcmp(users[i].username, user->username) == 0) {
            strcpy(users[i].password, user->password);
            break;
        }
    }

    saveUsersToFile();
    ui_success("Mot de passe modifie avec succes !");
}

//------------------LOGIN------------------
int login(User *user) {
    char username[30];
    char password[30];

    ui_input_prompt("Username");
    lireString(username, sizeof(username));
    ui_input_prompt("Password");
    lireString(password, sizeof(password));

    for(int i = 0; i < usersCount; i++) {
        if(strcmp(users[i].username, username) == 0 &&
           strcmp(users[i].password, password) == 0) {
            *user = users[i];
            return 1;
        }
    }
    return 0;
}

//------------------MENU------------------
void gestionUserMenu() {
    int userChoice;
    const char *options[] = {
        "Afficher Utilisateurs",
        "Ajouter Utilisateur",
        "Supprimer Utilisateur",
        "Modifier Mon Compte",
        "Retour"
    };

    do {
        ui_clear();
        ui_logo();
        ui_title("Gestion Utilisateurs");

        userChoice = ui_menu("Menu Utilisateurs", options, 5);

        switch(userChoice) {
            case 1:
                afficherUsers();
                ui_pause();
                break;
            case 2:
                ajouterUser();
                ui_pause();
                break;
            case 3:
                supprimerUser();
                ui_pause();
                break;
            case 4:
                modifierMonCompte(&loggedUser);
                ui_pause();
                break;
            case 5:
                break;
            default:
                ui_warning("Choix invalide !");
                ui_pause();
        }

    } while(userChoice != 5);
}

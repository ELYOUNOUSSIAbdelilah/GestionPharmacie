#ifndef UTILISATEUR_H_INCLUDED
#define UTILISATEUR_H_INCLUDED
typedef struct {
    char username[30];
    char password[30];
    int role;   // 1 = admin, 0 = user
} User;


extern User users[];
extern int usersCount;

// Prototype de la fonction login
int login(User *loggedUser);
void afficherUsers();
void ajouterUser();
void afficherUsers();
void supprimerUser();
void modifierMonCompte(User *user);
void initDefaultAdmin();
void gestionUserMenu();
#endif // UTILISATEUR_H_INCLUDED

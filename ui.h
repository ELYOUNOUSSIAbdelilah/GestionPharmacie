/*
 * ======================================================================================
 * UI.H - MODULE D'INTERFACE CONSOLE MULTIPLATEFORME
 * ======================================================================================
 * Description : Bibliothèque UI pour console avec support complet Windows/Linux
 * Style       : Futuriste / Parking System avec ASCII Art
 * Compatibilité : Windows (ANSI via SetConsoleMode) + Linux (ANSI natif)
 * ======================================================================================
 */

#ifndef UI_H
#define UI_H

/* ======================================================================================
   PROTOTYPES DES FONCTIONS UI
   ====================================================================================== */

/* --- Initialisation --- */
void ui_init(void);                              // Active les couleurs ANSI sur Windows
void ui_clear(void);                             // Efface l'écran
void ui_pause(void);                             // Attend une touche
void ui_sleep_ms(int milliseconds);              // Pause en millisecondes

/* --- Titres et sections --- */
void ui_logo(void);                              // Affiche le logo PHARMACIE
void ui_title(const char *text);                 // Titre de section stylisé
void ui_separator(int width);                    // Ligne de séparation
void ui_header(const char *text);                // En-tête avec bordures

/* --- Messages --- */
void ui_success(const char *message);            // Message de succès (vert)
void ui_error(const char *message);              // Message d'erreur (rouge)
void ui_warning(const char *message);            // Message d'avertissement (jaune)
void ui_info(const char *message);               // Message informatif (cyan)

/* --- Menus --- */
int ui_menu(const char *title, const char *options[], int count);
void ui_menu_option(int num, const char *text, int highlight);

/* --- Tableaux --- */
void ui_table_header(const char *columns[], int count, int widths[]);
void ui_table_row(const char *values[], int count, int widths[]);
void ui_table_separator(int widths[], int count);

/* --- Entrées utilisateur --- */
void ui_input_prompt(const char *label);
void ui_progress_bar(int current, int total, const char *label);

/* --- Utilitaires --- */
void ui_center_text(const char *text, int width);
void ui_clear();
void ui_logo();
void ui_title(const char *titre);
int ui_menu(const char *titre, const char *options[], int nbOptions);
void ui_warning(const char *msg);
void ui_error(const char *msg);
void ui_success(const char *msg);
void ui_pause();

#endif /* UI_H */

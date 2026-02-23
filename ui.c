/*
 * ======================================================================================
 * UI.C - VERSION OPTIMISEE POUR CODE::BLOCKS WINDOWS 10
 * ======================================================================================
 */

#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>

    /* Définir les constantes manquantes pour MinGW ancien */
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif

    #ifndef DISABLE_NEWLINE_AUTO_RETURN
        #define DISABLE_NEWLINE_AUTO_RETURN 0x0008
    #endif
#else
    #include <unistd.h>
#endif

/* Variable globale : état des couleurs ANSI */
static int ui_colors_enabled = 0;

/* ======================================================================================
   CODES COULEURS ANSI
   ====================================================================================== */
#define UI_RESET        "\033[0m"
#define UI_BOLD         "\033[1m"
#define UI_DIM          "\033[2m"

#define UI_BLACK        "\033[30m"
#define UI_RED          "\033[91m"
#define UI_GREEN        "\033[92m"
#define UI_YELLOW       "\033[93m"
#define UI_BLUE         "\033[94m"
#define UI_MAGENTA      "\033[95m"
#define UI_CYAN         "\033[96m"
#define UI_WHITE        "\033[97m"
#define UI_GRAY         "\033[90m"

#define UI_BG_BLACK     "\033[40m"

/* ======================================================================================
   INITIALISATION WINDOWS 10
   ====================================================================================== */
void ui_init(void) {
    #ifdef _WIN32
        /* Activer UTF-8 d'abord */
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        /* Obtenir le handle de la console */
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) {
            ui_colors_enabled = 0;
            printf("[INFO] Console handle invalide - mode texte simple active.\n");
            return;
        }

        /* Lire le mode actuel */
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) {
            ui_colors_enabled = 0;
            printf("[INFO] Impossible de lire le mode console - mode texte simple active.\n");
            return;
        }

        /* Activer le support ANSI */
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

        if (SetConsoleMode(hOut, dwMode)) {
            ui_colors_enabled = 1;
            /* Succès silencieux */
        } else {
            ui_colors_enabled = 0;
            printf("[INFO] Couleurs ANSI non disponibles - utilisation du mode texte simple.\n");
            printf("[CONSEIL] Utilisez Windows Terminal pour une meilleure experience.\n\n");
        }
    #else
        /* Linux/Unix : ANSI natif */
        ui_colors_enabled = 1;
    #endif
}

/* ======================================================================================
   GESTION ECRAN
   ====================================================================================== */
void ui_clear(void) {
    #ifdef _WIN32
        if (ui_colors_enabled) {
            printf("\033[2J\033[H");
            fflush(stdout);
        } else {
            system("cls");
        }
    #else
        if (ui_colors_enabled) {
            printf("\033[2J\033[H");
            fflush(stdout);
        } else {
            system("clear");
        }
    #endif
}

void ui_pause(void) {
    printf("\n[Appuyez sur ENTREE pour continuer...] ");
    fflush(stdout);

    #ifdef _WIN32
        /* Vider le buffer clavier */
        while (_kbhit()) _getch();
        _getch();
    #else
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    #endif
}

void ui_sleep_ms(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #else
        usleep(milliseconds * 1000);
    #endif
}

/* ======================================================================================
   LOGO ASCII ART
   ====================================================================================== */
void ui_logo(void) {
    if (ui_colors_enabled) {
        printf("%s%s\n", UI_CYAN, UI_BOLD);
    } else {
        printf("\n");
    }

    printf("          ██████╗ ██╗  ██╗ █████╗ ██████╗ ███╗   ███╗ █████╗  ██████╗██╗███████╗\n");
    printf("          ██╔══██╗██║  ██║██╔══██╗██╔══██╗████╗ ████║██╔══██╗██╔════╝██║██╔════╝\n");
    printf("          ██████╔╝███████║███████║██████╔╝██╔████╔██║███████║██║     ██║█████╗  \n");
    printf("          ██╔═══╝ ██╔══██║██╔══██║██╔══██╗██║╚██╔╝██║██╔══██║██║     ██║██╔══╝  \n");
    printf("          ██║     ██║  ██║██║  ██║██║  ██║██║ ╚═╝ ██║██║  ██║╚██████╗██║███████╗\n");
    printf("          ╚═╝     ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝ ╚═════╝╚═╝╚══════╝\n");

    if (ui_colors_enabled) {
        printf("%s\n", UI_RESET);
    } else {
        printf("\n");
    }
}

/* ======================================================================================
   TITRES ET SECTIONS
   ====================================================================================== */
void ui_separator(int width) {
    if (ui_colors_enabled) printf("%s", UI_CYAN);

    for (int i = 0; i < width; i++) {
        printf("-");
    }

    if (ui_colors_enabled) printf("%s", UI_RESET);
    printf("\n");
}

void ui_title(const char *text) {
    printf("\n");
    ui_separator(80);

    if (ui_colors_enabled) {
        printf("%s%s    %s%s\n", UI_GREEN, UI_BOLD, text, UI_RESET);
    } else {
        printf("    %s\n", text);
    }

    ui_separator(80);
    printf("\n");
}

void ui_header(const char *text) {
    printf("\n");

    if (ui_colors_enabled) printf("%s", UI_CYAN);
    printf("╔");
    for (int i = 0; i < 78; i++) printf("═");
    printf("╗");
    if (ui_colors_enabled) printf("%s", UI_RESET);
    printf("\n");

    if (ui_colors_enabled) {
        printf("%s║%s%s %-74s %s║%s\n",
               UI_CYAN, UI_GREEN, UI_BOLD, text, UI_CYAN, UI_RESET);
    } else {
        printf("║ %-74s ║\n", text);
    }

    if (ui_colors_enabled) printf("%s", UI_CYAN);
    printf("╚");
    for (int i = 0; i < 78; i++) printf("═");
    printf("╝");
    if (ui_colors_enabled) printf("%s", UI_RESET);
    printf("\n\n");
}

/* ======================================================================================
   MESSAGES UTILISATEUR
   ====================================================================================== */
void ui_success(const char *message) {
    printf("\n");
    if (ui_colors_enabled) {
        printf("%s%s    [OK] %s%s\n", UI_GREEN, UI_BOLD, message, UI_RESET);
    } else {
        printf("    [OK] %s\n", message);
    }
    fflush(stdout);
}

void ui_error(const char *message) {
    printf("\n");
    if (ui_colors_enabled) {
        printf("%s%s    [ERREUR] %s%s\n", UI_RED, UI_BOLD, message, UI_RESET);
    } else {
        printf("    [ERREUR] %s\n", message);
    }
    fflush(stdout);
}

void ui_warning(const char *message) {
    printf("\n");
    if (ui_colors_enabled) {
        printf("%s%s    [ATTENTION] %s%s\n", UI_YELLOW, UI_BOLD, message, UI_RESET);
    } else {
        printf("    [ATTENTION] %s\n", message);
    }
    fflush(stdout);
}

void ui_info(const char *message) {
    printf("\n");
    if (ui_colors_enabled) {
        printf("%s%s    [INFO] %s%s\n", UI_CYAN, UI_BOLD, message, UI_RESET);
    } else {
        printf("    [INFO] %s\n", message);
    }
    fflush(stdout);
}

/* ======================================================================================
   SYSTEME DE MENU
   ====================================================================================== */
int ui_menu(const char *title, const char *options[], int count) {
    ui_clear();
    ui_logo();
    ui_title(title);

    for (int i = 0; i < count; i++) {
        if (ui_colors_enabled) {
            printf("%s    %d. %s%s\n", UI_CYAN, i + 1, options[i], UI_RESET);
        } else {
            printf("    %d. %s\n", i + 1, options[i]);
        }
    }

    if (ui_colors_enabled) {
        printf("%s    0. Retour / Quitter%s\n", UI_GRAY, UI_RESET);
    } else {
        printf("    0. Retour / Quitter\n");
    }

    ui_separator(80);

    if (ui_colors_enabled) {
        printf("\n%s%s    Votre choix : %s", UI_CYAN, UI_BOLD, UI_WHITE);
    } else {
        printf("\n    Votre choix : ");
    }
    fflush(stdout);

    int choice;
    if (scanf("%d", &choice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return -1;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    if (ui_colors_enabled) printf("%s", UI_RESET);

    return choice;
}

/* ======================================================================================
   TABLEAUX
   ====================================================================================== */
void ui_table_header(const char *columns[], int count, int widths[]) {
    if (ui_colors_enabled) {
        printf("%s%s    ", UI_CYAN, UI_BOLD);
    } else {
        printf("    ");
    }

    for (int i = 0; i < count; i++) {
        printf("%-*s ", widths[i], columns[i]);
    }

    if (ui_colors_enabled) {
        printf("%s\n", UI_RESET);
    } else {
        printf("\n");
    }

    if (ui_colors_enabled) printf("%s    ", UI_GREEN);
    else printf("    ");

    int total = 0;
    for (int i = 0; i < count; i++) {
        total += widths[i] + 1;
    }

    for (int i = 0; i < total; i++) {
        printf("-");
    }

    if (ui_colors_enabled) {
        printf("%s\n", UI_RESET);
    } else {
        printf("\n");
    }
}

void ui_table_row(const char *values[], int count, int widths[]) {
    if (ui_colors_enabled) {
        printf("%s    ", UI_WHITE);
    } else {
        printf("    ");
    }

    for (int i = 0; i < count; i++) {
        printf("%-*s ", widths[i], values[i]);
    }

    if (ui_colors_enabled) {
        printf("%s\n", UI_RESET);
    } else {
        printf("\n");
    }
}

void ui_table_separator(int widths[], int count) {
    if (ui_colors_enabled) printf("%s    ", UI_GRAY);
    else printf("    ");

    int total = 0;
    for (int i = 0; i < count; i++) {
        total += widths[i] + 1;
    }

    for (int i = 0; i < total; i++) {
        printf("-");
    }

    if (ui_colors_enabled) {
        printf("%s\n", UI_RESET);
    } else {
        printf("\n");
    }
}

/* ======================================================================================
   ENTREES UTILISATEUR
   ====================================================================================== */
void ui_input_prompt(const char *label) {
    if (ui_colors_enabled) {
        printf("%s    %s : %s", UI_CYAN, label, UI_WHITE);
    } else {
        printf("    %s : ", label);
    }
    fflush(stdout);
}

void ui_progress_bar(int current, int total, const char *label) {
    int bar_width = 50;
    float progress = (float)current / total;
    int filled = (int)(bar_width * progress);

    printf("\r    %s [", label);

    for (int i = 0; i < filled; i++) {
        printf("#");
    }

    for (int i = filled; i < bar_width; i++) {
        printf(".");
    }

    printf("] %3d%%", (int)(progress * 100));
    fflush(stdout);

    if (current >= total) {
        printf("\n");
    }
}

/* ======================================================================================
   UTILITAIRES
   ====================================================================================== */
void ui_center_text(const char *text, int width) {
    int len = strlen(text);
    int padding = (width - len) / 2;

    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("%s\n", text);
}

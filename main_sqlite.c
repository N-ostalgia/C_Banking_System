#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <stdarg.h>
#include "database.h"

// Largeur de la boîte pour les bordures
#define BOX_WIDTH 78

// Definitions des couleurs
#define RESET   "\033[0m"
#define BLUE    "\033[34m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define WHITE   "\033[37m"
#define BLACK   "\033[30m"
#define MAGENTA "\033[35m"
#define PINK    "\033[95m"

// Definition des fichiers de demandes
const char *demande_carnet_cheque = "demande_carnet_cheque.txt";
const char *demande_recuperationfile = "demande_recuperation.txt";

// Structure de date
typedef struct {
    int jour, mois, annee;
} date;

// Structure d'information de client
typedef struct {
    char nom[20];
    char prenom[20];
    char CIN[15];
    char type_Compte[20];
    char password[20];
    date date_naissance;
    char tele[20];
    char email[30];
    bool compte_actif;
} infoclient;

// Structure de compte
typedef struct {
    int numero_compte;
    float solde;
    char RIB[25];
    infoclient Client;
} Compte;

// Structure de demande generique
typedef struct {
    int id_demande;
    int type_demande;
    bool valide;
    infoclient Client;
    float montant;
    char RIB_dest[25];
    char nouvelles_infos[100];
} Demande;

// Structure pour l'admin
typedef struct {
    int id_admin;
    char nom[20];
    char prenom[20];
    char tel[20];
    char password[20];
} Admin;

// Fonction pour effacer la console
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Fonction pour masquer le mot de passe
void read_password(char *password) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == 13) {
            password[i] = '\0';
            break;
        } else if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            password[i++] = ch;
            printf("*");
        }
    }
    printf("\n");
}

// Fonction pour afficher un texte au centre
void afficherCentre(const char *texte) {
    int largeur_console = BOX_WIDTH;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        largeur_console = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    int longueur_texte = strlen(texte);
    int espaces = (largeur_console - longueur_texte) / 2;
    if (espaces > 0) {
        for (int i = 0; i < espaces; i++) {
            printf(" ");
        }
    }
    printf("%s\n", texte);
}

// Fonction pour afficher un texte formaté au centre
void afficherCentreSF(const char *format, ...) {
    char texte[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(texte, sizeof(texte), format, args);
    va_end(args);

    int largeur_console = BOX_WIDTH;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        largeur_console = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    int longueur_texte = strlen(texte);
    int espaces = (largeur_console - longueur_texte) / 2;
    if (espaces > 0) {
        for (int i = 0; i < espaces; i++) {
            printf(" ");
        }
    }
    printf("%s\n", texte);
}

// Fonction pour changer la couleur
void changerCouleur(const char *couleur) {
    printf("%s", couleur);
}

// Fonction pour afficher un texte en vert sur fond blanc
void printGreenOnWhite(const char *text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttributes = csbi.wAttributes;
    SetConsoleTextAttribute(hConsole, (15 << 4) | 2);
    printf("%s", text);
    SetConsoleTextAttribute(hConsole, originalAttributes);
}

// Fonction pour afficher un texte en rouge sur fond blanc
void printRedOnWhite(const char *text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttributes = csbi.wAttributes;
    SetConsoleTextAttribute(hConsole, (15 << 4) | 4);
    printf("%s", text);
    SetConsoleTextAttribute(hConsole, originalAttributes);
}

// Fonction pour afficher du texte rouge formaté
void printFormattedRedOnWhite(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Sauvegarder les attributs actuels
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttrs = csbi.wAttributes;

    // Définir rouge sur blanc
    SetConsoleTextAttribute(hConsole, (15 << 4) | 4); // Fond blanc, texte rouge

    vprintf(format, args);

    // Restaurer les attributs originaux
    SetConsoleTextAttribute(hConsole, originalAttrs);

    va_end(args);
}

// Fonction pour afficher du texte vert formaté
void printFormattedGreenOnWhite(const char *format, ...) {
    va_list args;
    va_start(args, format);

    // Sauvegarder les attributs actuels
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttrs = csbi.wAttributes;

    // Définir vert sur blanc
    SetConsoleTextAttribute(hConsole, (15 << 4) | 2); // Fond blanc, texte vert

    vprintf(format, args);

    // Restaurer les attributs originaux
    SetConsoleTextAttribute(hConsole, originalAttrs);

    va_end(args);
}

// Menus
void menuprincipal() {
    const char *options[] = {
        "ESPACE ADMIN",
        "ESPACE CLIENT",
        "EXIT"
    };

    afficherMenuModerne("🏦 SAR BANK - MENU PRINCIPAL        ", options, 3);
    printf("\n\t\t\t\t\t ENTREZ VOTRE CHOIX : ");
}

void menuClient() {
    const char *options[] = {
        "S'INSCRIRE",
        "SE CONNECTER",
        "REVENIR AU MENU PRINCIPAL"
    };

    afficherMenuModerne("👤 ESPACE CLIENT - MENU", options, 3);
    printf("\n\t\t\t\t\t ENTREZ VOTRE CHOIX : ");
}

// Fonction pour afficher l'historique des transactions
void afficherhistoriqueclient(char rib_entre[25]) {
    printf("\n");
    afficherCentre("====================================== voici votre historique =========================================\n\n");
    db_get_transactions_by_rib(rib_entre);
}

// Fonction pour afficher les informations du compte
void afficherInformations(char rib_entre[25]) {
    char nom[50], prenom[50], cin[20], type_compte[20], telephone[20], email[30], date_naissance[20];
    double solde;
    int account_number;

    if (db_get_client_full_info(rib_entre, nom, prenom, cin, type_compte, telephone, email, date_naissance, &solde, &account_number)) {
        printf("\n");
        afficherCentre("====================================== INFORMATIONS DU COMPTE =========================================\n\n");
        printf("\t\t\t\t\tRIB: %s\n", rib_entre);
        printf("\t\t\t\t\tNom: %s\n", nom);
        printf("\t\t\t\t\tPrenom: %s\n", prenom);
        printf("\t\t\t\t\tCIN: %s\n", cin);
        printf("\t\t\t\t\tType de compte: %s\n", type_compte);
        printf("\t\t\t\t\tTelephone: %s\n", telephone);
        printf("\t\t\t\t\tEmail: %s\n", email);
        printf("\t\t\t\t\tDate de naissance: %s\n", date_naissance);
        printf("\t\t\t\t\tNumero de compte: %d\n", account_number);
        printf("\t\t\t\t\tSolde: %.2f DH\n", solde);
        afficherCentre("====================================================================================================\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Compte non trouve.\n");
    }
}

// Fonction pour effectuer un depot
void effectuerDepot(char rib_entre[25], float montant) {
    char nom[50], prenom[50];
    double solde_actuel;

    if (db_get_client_by_rib(rib_entre, nom, prenom, &solde_actuel)) {
        double nouveau_solde = solde_actuel + montant;
        if (db_update_client_solde(rib_entre, nouveau_solde)) {
            char *timestamp = db_get_current_timestamp();
            db_add_transaction(rib_entre, "DEPOT", montant, "Depot en especes", timestamp);
            free(timestamp);

            afficherSucces("Depot effectue avec succes!");
            printf("\t\t\t\t\t Nouveau solde: %.2f DH\n", nouveau_solde);
        } else {
            afficherErreur("Erreur lors du depot.");
        }
    } else {
        afficherErreur("Compte non trouve.");
    }
}

// Fonction pour effectuer un retrait
void effectuerRetrait(char rib_entre[25], float montant) {
    char nom[50], prenom[50];
    double solde_actuel;

    if (db_get_client_by_rib(rib_entre, nom, prenom, &solde_actuel)) {
        if (montant > solde_actuel) {
            afficherAvertissement("Solde insuffisant!");
            return;
        }

        double nouveau_solde = solde_actuel - montant;
        if (db_update_client_solde(rib_entre, nouveau_solde)) {
            char *timestamp = db_get_current_timestamp();
            db_add_transaction(rib_entre, "RETRAIT", montant, "Retrait en especes", timestamp);
            free(timestamp);

            afficherSucces("Retrait effectue avec succes!");
            printf("\t\t\t\t\t Nouveau solde: %.2f DH\n", nouveau_solde);
        } else {
            afficherErreur("Erreur lors du retrait.");
        }
    } else {
        afficherErreur("Compte non trouve.");
    }
}

// Fonction pour effectuer un virement
void effectuerVirement(char rib_entre[25], char rib_destination[25], float montant) {
    char nom[50], prenom[50];
    double solde_actuel;

    if (db_get_client_by_rib(rib_entre, nom, prenom, &solde_actuel)) {
        if (montant > solde_actuel) {
            printRedOnWhite("\t\t\t\t\t Solde insuffisant!\n");
            return;
        }

        // Verifier si le compte destinataire existe
        char nom_dest[50], prenom_dest[50];
        double solde_dest;
        if (!db_get_client_by_rib(rib_destination, nom_dest, prenom_dest, &solde_dest)) {
            printRedOnWhite("\t\t\t\t\t Compte destinataire non trouve.\n");
            return;
        }

        // Effectuer le virement
        double nouveau_solde_source = solde_actuel - montant;
        double nouveau_solde_dest = solde_dest + montant;

        if (db_update_client_solde(rib_entre, nouveau_solde_source) &&
            db_update_client_solde(rib_destination, nouveau_solde_dest)) {

            char *timestamp = db_get_current_timestamp();
            char description[100];
            sprintf(description, "Virement vers %s", rib_destination);
            db_add_transaction(rib_entre, "VIREMENT", montant, description, timestamp);

            sprintf(description, "Virement de %s", rib_entre);
            db_add_transaction(rib_destination, "VIREMENT", montant, description, timestamp);
            free(timestamp);

            printGreenOnWhite("\t\t\t\t\t Virement effectue avec succes!\n");
            printf("\t\t\t\t\t Nouveau solde: %.2f DH\n", nouveau_solde_source);
        } else {
            printRedOnWhite("\t\t\t\t\t Erreur lors du virement.\n");
        }
    } else {
        printRedOnWhite("\t\t\t\t\t Compte non trouve.\n");
    }
}

// Fonction pour afficher les statistiques
void afficherStatistiques(char rib_entre[25]) {
    double total_deposits, total_withdrawals, total_transfers;
    int total_transactions;

    if (db_get_transaction_statistics(rib_entre, &total_deposits, &total_withdrawals, &total_transfers, &total_transactions)) {
        printf("\n");
        afficherCentreSF("========== Statistiques pour le compte : %s ==========\n", rib_entre);
        printf("\t\t\t\t\tTotal des depots: %.2f DH\n", total_deposits);
        printf("\t\t\t\t\tTotal des retraits: %.2f DH\n", total_withdrawals);
        printf("\t\t\t\t\tTotal des virements: %.2f DH\n", total_transfers);
        printf("\t\t\t\t\tNombre total de transactions: %d\n", total_transactions);
        afficherCentre("========================================================\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors du calcul des statistiques.\n");
    }
}

// Fonction pour remplir une demande de creation
void remplirDemande_creation() {
    char rib[25], password[20], nom[20], prenom[20], cin[15], telephone[20], email[30], date_naissance[20], type_compte[20];

    printf("\n\n\n\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t RIB du compte          |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", rib);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t mot de passe           |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    read_password(password);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t Nom                    |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", nom);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t Prenom                 |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", prenom);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t CIN                    |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", cin);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t Telephone              |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", telephone);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t Email                  |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", email);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t Date de naissance      |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", date_naissance);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t Type de compte         |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", type_compte);

    int account_number = db_get_next_account_number();
    if (db_add_client(rib, password, account_number, nom, prenom, cin, telephone, email, date_naissance, type_compte, 0.0)) {
        printGreenOnWhite("\t\t\t\t\t Demande de creation envoyee avec succes!\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de l'envoi de la demande.\n");
    }
}

// Fonction de login client
void login(char RIB[25], char password[20]) {
    char rib_entre[25];
    char motdepass[20];
    int tentative = 0;

    do {
        printf("\n\n\n\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t RIB du compte          |\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t  ");
        scanf("%s", rib_entre);

        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t mot de passe           |\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t  ");
        read_password(motdepass);

        if (db_verify_client_login(rib_entre, motdepass)) {
            char nom[50], prenom[50];
            double solde;
            db_get_client_by_rib(rib_entre, nom, prenom, &solde);

            getchar();
            clearConsole();
            printGreenOnWhite("\n\n\n\n\t\t\t\t\t CONNEXION REUSSIE DANS SAR BANQUE !! ");

            // Enregistrer l'operation dans l'historique
            char *timestamp = db_get_current_timestamp();
            db_add_transaction(rib_entre, "CONNEXION", 0, "Connexion au compte", timestamp);
            free(timestamp);

            Sleep(2600);
            clearConsole();
            changerCouleur(MAGENTA);
            printf("\n\n\t\t\t\t\t*^*BIENVENUE DANS VOTRE COMPTE, %s !*^*\n", nom);
            changerCouleur(BLACK);

            Menu_princ(rib_entre);
            return;
        } else {
            printRedOnWhite("\n\t\t\t\t\t RIB ou mot de passe incorrect !\n");
            tentative++;

            if (tentative < 3) {
                afficherCentre("Voulez-vous ressaisir les informations ? (1. Oui / 2. Mot de passe oublie)\n");
                int choix_ressaisir;
                printf("\t\t\t\t\t\t\t");
                scanf("%d", &choix_ressaisir);
                clearConsole();

                if (choix_ressaisir == 2) {
                    afficherCentre("vous devez faire une demande de recuperation de mot de passe\n");
                    afficherCentre("veuillez remplir ces informations_\n");
                    remplirDemande_recuperation();
                    break;
                }
            } else {
                printRedOnWhite("\t\t\t\t\t Trois tentatives echouees. Veuillez essayer plus tard.\n");
                Sleep(7000);
                clearConsole();
                return;
            }
        }
    } while (tentative < 3);
}

// Fonction pour remplir une demande de recuperation
void remplirDemande_recuperation() {
    char rib[25], email[30];

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t RIB du compte          |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", rib);

    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t Email                  |\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t+-------------------------------+\n");
    printf("\t\t\t\t\t\t|\t  ");
    scanf("%s", email);

    if (db_add_demand(rib, "RECUPERATION", 0, "", email)) {
        printGreenOnWhite("\t\t\t\t\t Demande de recuperation envoyee avec succes!\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de l'envoi de la demande.\n");
    }
}

// Fonction pour remplir une demande de suppression
void remplirDemande_suppression(char rib_entre[25]) {
    char nom[20], prenom[20], cin[15], type_compte[20];

    printf("\t\t\t\t\t Entrez votre nom : ");
    scanf("%s", nom);

    printf("\t\t\t\t\t Entrez votre prenom : ");
    scanf("%s", prenom);

    printf("\t\t\t\t\t Entrez votre CIN : ");
    scanf("%s", cin);

    printf("\t\t\t\t\t Entrez le type de compte (par ex. 'Epargne', 'Courant') : ");
    scanf("%s", type_compte);

    char nouvelles_infos[100];
    snprintf(nouvelles_infos, sizeof(nouvelles_infos), "Suppression - Nom: %s, Prenom: %s, CIN: %s, Type: %s",
            nom, prenom, cin, type_compte);

    if (db_add_demand(rib_entre, "SUPPRESSION", 0, "", nouvelles_infos)) {
        printGreenOnWhite("\t\t\t\t\t Demande de suppression enregistree avec succes !\n");
        Sleep(3000);
        clearConsole();

        // Enregistrer dans l'historique
        char *timestamp = db_get_current_timestamp();
        db_add_transaction(rib_entre, "DEMANDE", 0, "Demande de suppression du compte", timestamp);
        free(timestamp);
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de l'enregistrement de la demande.\n");
    }
}

// Fonction pour remplir une demande de modification
void remplirDemande_modification(char rib_entre[25]) {
    char nom[20], prenom[20], info_a_modifier[50], nouvelle_info[50];

    printf("\t\t\t\t\t veuillez ecrire en minuscule\n");
    printf("\t\t\t\t\t Entrez votre nom : ");
    scanf("%s", nom);

    printf("\t\t\t\t\t Entrez votre prenom : ");
    scanf("%s", prenom);

    printf("\t\t\t\t\t Entrez l'information a modifier : ");
    scanf("%s", info_a_modifier);

    printf("\t\t\t\t\t Entrez la nouvelle information : ");
    scanf("%s", nouvelle_info);

    char nouvelles_infos[100];
    snprintf(nouvelles_infos, sizeof(nouvelles_infos), "Modification %s: %s -> %s (Nom: %s, Prenom: %s)",
            info_a_modifier, info_a_modifier, nouvelle_info, nom, prenom);

    if (db_add_demand(rib_entre, "MODIFICATION", 0, "", nouvelles_infos)) {
        printGreenOnWhite("\t\t\t\t\t Demande de modification enregistree avec succes! ");
        Sleep(5000);
        clearConsole();

        // Enregistrer dans l'historique
        char *timestamp = db_get_current_timestamp();
        char description[100];
        snprintf(description, sizeof(description), "Demande de modification de %s par %s", info_a_modifier, nouvelle_info);
        db_add_transaction(rib_entre, "DEMANDE", 0, description, timestamp);
        free(timestamp);
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de l'enregistrement de la demande.\n");
    }
}

// Fonction pour remplir une demande de carnet de cheques
void remplirDemande_carnet(char rib_entre[25]) {
    int nombre_feuilles, quantite;

    printf("\t\t\t\t\t Entrez le nombre de feuilles par carnet : ");
    scanf("%d", &nombre_feuilles);

    printf("\t\t\t\t\t Entrez la quantite de carnets souhaitee : ");
    scanf("%d", &quantite);

    char nouvelles_infos[100];
    snprintf(nouvelles_infos, sizeof(nouvelles_infos), "Carnet de cheques - %d feuilles, %d carnets", nombre_feuilles, quantite);

    if (db_add_demand(rib_entre, "CARNET", 0, "", nouvelles_infos)) {
        printGreenOnWhite("\t\t\t\t\t Demande de carnet de cheques enregistree avec succes !\n");
        Sleep(3000);
        clearConsole();

        // Enregistrer dans l'historique
        char *timestamp = db_get_current_timestamp();
        char description[100];
        snprintf(description, sizeof(description), "Demande de carnet de cheques (%d feuilles, %d carnets)", nombre_feuilles, quantite);
        db_add_transaction(rib_entre, "DEMANDE", 0, description, timestamp);
        free(timestamp);
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de l'enregistrement de la demande.\n");
    }
}

// Menu principal du client
void Menu_princ(char rib_entre[25]) {
    int choix;
    do {
        const char *options[] = {
            "AFFICHER INFORMATIONS",
            "EFFECTUER DEPOT",
            "EFFECTUER RETRAIT",
            "EFFECTUER VIREMENT",
            "AFFICHER HISTORIQUE",
            "AFFICHER STATISTIQUES",
            "PAYER FACTURE",
            "EFFECTUER RECHARGE",
            "PAYER VIGNETTE",
            "BOITE MESSAGE",
            "DEMANDER SUPPRESSION",
            "DEMANDER MODIFICATION",
            "DEMANDER CARNET DE CHEQUES",
            "DECONNEXION"
        };

        afficherMenuModerne("🏦 MENU PRINCIPAL CLIENT", options, 14);
        printf("\n\t\t\t\t\t ENTREZ VOTRE CHOIX : ");
        scanf("%d", &choix);
        clearConsole();

        switch (choix) {
            case 1:
                afficherInformations(rib_entre);
                break;
            case 2: {
                float montant;
                printf("\t\t\t\t\t Montant a deposer: ");
                scanf("%f", &montant);
                effectuerDepot(rib_entre, montant);
                break;
            }
            case 3: {
                float montant;
                printf("\t\t\t\t\t Montant a retirer: ");
                scanf("%f", &montant);
                effectuerRetrait(rib_entre, montant);
                break;
            }
            case 4: {
                char rib_dest[25];
                float montant;
                printf("\t\t\t\t\t RIB destinataire: ");
                scanf("%s", rib_dest);
                printf("\t\t\t\t\t Montant: ");
                scanf("%f", &montant);
                effectuerVirement(rib_entre, rib_dest, montant);
                break;
            }
            case 5:
                afficherhistoriqueclient(rib_entre);
                break;
            case 6:
                afficherStatistiques(rib_entre);
                break;
            case 7: {
                payerFacture(rib_entre, 0);
                break;
            }
            case 8: {
                effectuerRecharge(rib_entre, 0, NULL);
                break;
            }
            case 9: {
                payerVignette(rib_entre, 0);
                break;
            }
            case 10:
                afficherboitemessage(rib_entre);
                break;
            case 11:
                remplirDemande_suppression(rib_entre);
                break;
            case 12:
                remplirDemande_modification(rib_entre);
                break;
            case 13:
                remplirDemande_carnet(rib_entre);
                break;
            case 14:
                printGreenOnWhite("\t\t\t\t\t Deconnexion reussie!\n");
                return;
            default:
                printRedOnWhite("\t\t\t\t\t Choix invalide!\n");
        }

        if (choix != 14) {
            printf("\n\t\t\t\t\t Appuyez sur Entree pour continuer...");
            getchar();
            getchar();
            clearConsole();
        }
    } while (choix != 14);
}

// Fonction de login admin
void login_admin() {
    char nom[20], password[20];
    int tentative = 0;

    do {
        printf("\n\n\n\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t Nom d'admin            |\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t  ");
        scanf("%s", nom);

        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t Mot de passe           |\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t  ");
        read_password(password);

        if (db_verify_admin_login(nom, password)) {
            char prenom[20], telephone[20];
            db_get_admin_by_name(nom, prenom, telephone);

            getchar();
            clearConsole();
            printGreenOnWhite("\n\n\n\n\t\t\t\t\t CONNEXION ADMIN REUSSIE !! ");
            Sleep(2600);
            clearConsole();
            changerCouleur(MAGENTA);
            printf("\n\n\t\t\t\t\t*^*BIENVENUE ADMIN %s !*^*\n", nom);
            changerCouleur(BLACK);

            afficherMenuAdmin();
            return;
        } else {
            printRedOnWhite("\n\t\t\t\t\t Nom ou mot de passe incorrect !\n");
            tentative++;

            if (tentative >= 3) {
                printRedOnWhite("\t\t\t\t\t Trois tentatives echouees.\n");
                Sleep(3000);
                clearConsole();
                return;
            }
        }
    } while (tentative < 3);
}

// Menu admin
void afficherMenuAdmin() {
    int choix;
    do {
        const char *options[] = {
            "CREER COMPTE",
            "MODIFIER COMPTE",
            "SUPPRIMER COMPTE",
            "AFFICHER TOUS LES COMPTES",
            "AFFICHER INFO COMPTE",
            "GERER DEMANDES",
            "AJOUTER ADMIN",
            "STATISTIQUES",
            "STATISTIQUES COMPTES",
            "GENERER GRAPHIQUES",
            "DECONNEXION"
        };

        afficherMenuModerne("👨‍💼 MENU ADMINISTRATEUR", options, 11);
        printf("\n\t\t\t\t\t ENTREZ VOTRE CHOIX : ");
        scanf("%d", &choix);
        clearConsole();

        switch (choix) {
            case 1:
                creerCompte();
                break;
            case 2:
                modifierCompte();
                break;
            case 3:
                supprimerCompte();
                break;
            case 4:
                db_get_all_clients();
                break;
            case 5:
                afficherInfoCompte();
                break;
            case 6:
                gererDemandes();
                break;
            case 7:
                ajouterAdmin();
                break;
            case 8:
                afficherStatistiquesAdmin();
                break;
            case 9:
                statistiquesComptes();
                break;
            case 10:
                genererGraphiques();
                break;
            case 11:
                printGreenOnWhite("\t\t\t\t\t Deconnexion admin reussie!\n");
                return;
            default:
                printRedOnWhite("\t\t\t\t\t Choix invalide!\n");
        }

        if (choix != 11) {
            printf("\n\t\t\t\t\t Appuyez sur Entree pour continuer...");
            getchar();
            getchar();
            clearConsole();
        }
    } while (choix != 11);
}

// Fonction pour creer un compte
void creerCompte() {
    char rib[25], password[20], nom[20], prenom[20], cin[15], telephone[20], email[30], date_naissance[20], type_compte[20];
    float solde_initial;

    printf("\t\t\t\t\t RIB: ");
    scanf("%s", rib);
    printf("\t\t\t\t\t Mot de passe: ");
    read_password(password);
    printf("\t\t\t\t\t Nom: ");
    scanf("%s", nom);
    printf("\t\t\t\t\t Prenom: ");
    scanf("%s", prenom);
    printf("\t\t\t\t\t CIN: ");
    scanf("%s", cin);
    printf("\t\t\t\t\t Telephone: ");
    scanf("%s", telephone);
    printf("\t\t\t\t\t Email: ");
    scanf("%s", email);
    printf("\t\t\t\t\t Date de naissance: ");
    scanf("%s", date_naissance);
    printf("\t\t\t\t\t Type de compte: ");
    scanf("%s", type_compte);
    printf("\t\t\t\t\t Solde initial: ");
    scanf("%f", &solde_initial);

    int account_number = db_get_next_account_number();
    if (db_add_client(rib, password, account_number, nom, prenom, cin, telephone, email, date_naissance, type_compte, solde_initial)) {
        printGreenOnWhite("\t\t\t\t\t Compte cree avec succes!\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de la creation du compte.\n");
    }
}

// Fonction pour modifier un compte
void modifierCompte() {
    char rib[25], champ[20], nouvelle_valeur[100];

    printf("\t\t\t\t\t RIB du compte: ");
    scanf("%s", rib);
    printf("\t\t\t\t\t Champ a modifier (nom/prenom/telephone/email): ");
    scanf("%s", champ);
    printf("\t\t\t\t\t Nouvelle valeur: ");
    scanf("%s", nouvelle_valeur);

    if (db_modify_client_field(rib, champ, nouvelle_valeur)) {
        printGreenOnWhite("\t\t\t\t\t Compte modifie avec succes!\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de la modification.\n");
    }
}

// Fonction pour supprimer un compte
void supprimerCompte() {
    char rib[25];

    printf("\t\t\t\t\t RIB du compte a supprimer: ");
    scanf("%s", rib);

    if (db_delete_client(rib)) {
        printGreenOnWhite("\t\t\t\t\t Compte supprime avec succes!\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de la suppression.\n");
    }
}

// Fonction pour gerer les demandes
void gererDemandes() {
    int choix;

    while (1) {
    changerCouleur(MAGENTA);
    afficherCentre("\n");
    afficherCentre("************************* Menu Gestion des Demandes *************************\n");
    changerCouleur(BLACK);
    afficherCentre("*                                                                           *\n");
    afficherCentre("*             1. Gerer les Demandes de creation de compte                   *\n");
    afficherCentre("*             2. Gerer les Demandes de suppression de compte                *\n");
    afficherCentre("*             3. Gerer les Demandes de modification de compte               *\n");
    afficherCentre("*             4.Gerer les demandes de carnets de cheque                     *\n");
    afficherCentre("*             5. Gerer les Demandes de recuperation de mot de passe         *\n");
    afficherCentre("*             6. Quitter                                                    *\n");
    afficherCentre("*                                                                           *\n");
    changerCouleur(MAGENTA);
    afficherCentre("*****************************************************************************\n");
    printf("\t\t\t\t\t Entrez votre choix...");
        scanf("%d", &choix);
        changerCouleur(BLACK);
        getchar();
        clearConsole();
        switch (choix) {
      case 1: { // Gestion des demandes de création
        FILE *file = fopen("demande_creation.txt", "r");
        if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier demande_creation.txt\n");
        return;
        }

        FILE *temp = fopen("temp_creation.txt", "w");
        if (!temp) {
        printf("Erreur : Impossible de créer un fichier temporaire\n");
        fclose(file);
        return;
    }
    char ligne[300];
    char nom[50], prenom[50], cin[20], rib[25], type_compte[20], email[50], tele[15], password [20];
    char date [12] ;
    float solde_initial = 0;
    char id[10] ;
    bool demande_complete = false;
     bool demande_trouvee = false;
    while (fgets(ligne, sizeof(ligne), file)) {
        // Lecture des champs ligne par ligne
        if (strncmp(ligne, "ID demande : ", 13) == 0) {
            sscanf(ligne, "ID demande : %s", id);
            demande_trouvee = true;
        } else if (strncmp(ligne, "Nom : ", 6) == 0) {
            sscanf(ligne, "Nom : %s", nom);
        } else if (strncmp(ligne, "Prenom : ", 9) == 0) {
            sscanf(ligne, "Prenom : %s", prenom);
        } else if (strncmp(ligne, "Mot de passe : ",15) == 0) {
            sscanf(ligne, "Mot de passe : %s", password);
        }else if (strncmp(ligne, "CIN : ", 5) == 0) {
            sscanf(ligne, "CIN : %s", cin);
        } else if (strncmp(ligne, "Type de compte : ", 17) == 0) {
            sscanf(ligne, "Type de compte : %s", type_compte);
        } else if (strncmp(ligne, "Email : ", 8) == 0) {
            sscanf(ligne, "Email : %s", email);
        } else if (strncmp(ligne, "Telephone : ", 12) == 0) {
            sscanf(ligne, "Telephone : %s", tele);
        }  else if (strncmp(ligne, "Date de naissance : ", 20) == 0) {
            sscanf(ligne, "Date de naissance : %s", date);
        } else if (strncmp(ligne, "---", 3) == 0) { // Fin d'une demande
            demande_complete = true;
        }

        // Traiter la demande complète
        if (demande_complete) {
            printf("\t\t\t\t\t Demande ID : %s\n", id);
            printf("\t\t\t\t\t Nom : %s\n\t\t\t\t\t Prenom : %s\n\t\t\t\t\t CIN : %s\n", nom, prenom, cin);
            printf("\t\t\t\t\t mot de passe : %s\n\t\t\t\t\t Type : %s\n", password,type_compte);
            printf("\t\t\t\t\t Email : %s\n\t\t\t\t\t Telephone : %s\n", email, tele);
            printf("\t\t\t\t\t Date de naissance : %s\n", date);
            printf("\t\t\t\t\t Solde initial : %.2f\n", solde_initial);
            printf("\t\t\t\t\t 1. Approuver\n\t\t\t\t\t 2. Rejeter\n\t\t\t\t\t 3. Ignorer\n");
            int action;
            scanf("%d", &action);

            if (action == 1) {
                    printf("\t\t\t\t\t donner un rib a ce compte :");
                    scanf("%s",rib);
                // Créer le compte dans la base de données SQLite
                if (db_create_client_account(rib, nom, prenom, cin, type_compte, email, tele, date, password, solde_initial)) {
                    printFormattedGreenOnWhite("\t\t\t\t Compte créé avec succès : RIB %s.\n", rib);
                } else {
                    printRedOnWhite("\t\t\t\t\t Erreur lors de la création du compte.\n");
                }
                getchar();
                getchar();
                clearConsole();

            } else if (action == 2) {
                printf("\t\t\t\t\t Demande rejetée.\n");
            } else {
                // Réécrire la demande ignorée dans le fichier temporaire
                        fprintf(temp, "ID demande : %s\n", id);
                        fprintf(temp, "RIB : %s\n", rib);
                        fprintf(temp, "Mot de passe : %s\n", password);
                        fprintf(temp, "Nom : %s\n", nom);
                        fprintf(temp, "Prenom : %s\n", prenom);
                        fprintf(temp, "CIN : %s\n", cin);
                        fprintf(temp, "Telephone : %s\n", tele);
                        fprintf(temp, "Email : %s\n",email);
                        fprintf(temp, "Date de naissance : %s\n",date);
                        fprintf(temp, "Type de compte : %s\n",type_compte);
                        fprintf(temp, "Solde initial : %.2f\n", solde_initial);
                        fprintf(temp, "---------------------------------------------\n");
            }

            // Réinitialiser les variables pour la prochaine demande
            demande_complete = false;
        }
    }
                if (!demande_trouvee) {
                   printRedOnWhite("\t\t\t\t\t Aucune demande en attente.\n");
                }
    fclose(file);
    fclose(temp);
    remove("demande_creation.txt");
    rename("temp_creation.txt", "demande_creation.txt");
    break;
}

            case 2: { // Gestion des demandes de suppression
                char  rib[25] ;
                char id[10];
                bool  demande_complete ;
                bool demande_trouvee = false;
                 FILE *file = fopen("demande_suppression.txt", "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier demande_suppression.txt\n");
                return;
                }

                FILE *temp = fopen("temp_suppression.txt", "w");
                if (!temp) {
                printf("Erreur : Impossible de créer un fichier temporaire\n");
                fclose(file);
                return;
                    }
            char ligne[200];


            while (fgets(ligne, sizeof(ligne), file)) {
            if (strncmp(ligne, "ID demande : ", 13) == 0) {
            sscanf(ligne, "ID demande : %s", id);
            demande_trouvee = true ;
            }else if (strncmp(ligne, "RIB : ", 6) == 0) {
            sscanf(ligne, "RIB : %s", rib);
            }else if (strncmp(ligne, "---", 3) == 0) {
            demande_complete = true;
            }
            if (demande_complete) {
            printf("\n\t\t\t\t\t Demande ID : %s\n", id);
            printf("\t\t\t\t\t RIB : %s\n", rib);
            printf("\t\t\t\t\t 1. Approuver\n\t\t\t\t\t 2. Rejeter\n\t\t\t\t\t 3. Ignorer\n");
            int action;
                    scanf("%d", &action);

                        if (action == 1) {
                            printGreenOnWhite("\t\t\t\t\t Demande approuvée, suppression du compte.\n");
                            db_delete_client(rib);

                        } else if (action == 2) {
                            afficherCentre("Demande rejetée.\n");
                            // Ajouter un message dans la boîte de réception
                            db_add_message(rib, "Votre demande de suppression a été rejetée");

                        } else {
                            // Ajouter un message dans la boîte de réception
                            db_add_message(rib, "Votre demande de suppression est en cours de traitement");
                            fprintf(temp, "ID demande : %s\n", id);
                            fprintf(temp, "RIB : %s\n", rib);
                        }
                    }

            demande_complete = false;
        }
                    if (!demande_trouvee) {
                    printRedOnWhite("\t\t\t\t\t Aucune demande en attente.\n");
                }
                fclose(file);
                fclose(temp);
                remove("demande_suppression.txt");
                rename("temp_suppression.txt", "demande_suppression.txt");
                break;
            }

            case 3: { // Gestion des demandes de modification
                bool  demande_complete ;
                bool demande_trouvee = false;
                FILE *file = fopen("demande_modification.txt", "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier demande_modification.txt\n");
                return;
                }

                FILE *temp = fopen("temp_modification.txt", "w");
                if (!temp) {
                printf("Erreur : Impossible de créer un fichier temporaire\n");
                fclose(file);
                return;
                    }
                 char id[10] ;
                char rib[25], champ[20], nouvelle_valeur[50];
                char ligne[200];
                while (fgets(ligne, sizeof(ligne), file)) {
                    if (strncmp(ligne, "ID demande : ", 13) == 0) {
                        sscanf(ligne, "ID demande : %s", id);
                          demande_trouvee = true;
                    }else if (strncmp(ligne, "RIB : ", 6) == 0) {
                        sscanf(ligne, "RIB : %s", rib);
                    }else if (strncmp(ligne, "Information a modifier : ", 25) == 0) {
                        sscanf(ligne, "Information a modifier : %s", champ);
                    }else if (strncmp(ligne, "Nouvelle information : ", 23) == 0) {
                        sscanf(ligne, "Nouvelle information  : %s", nouvelle_valeur);
                    }else if (strncmp(ligne, "---", 3) == 0) {
                    demande_complete = true;
                    }
                     if (demande_complete){
printf("\n\t\t\t\t\t ID demande : %s\n\t\t\t\t\t RIB : %s\n\t\t\t\t\t Information a modifier : %s\n\t\t\t\t\t Nouvelle valeur : %s\n",id, rib, champ, nouvelle_valeur);
                        printf("\t\t\t\t\t 1. Approuver\n\t\t\t\t\t 2. Rejeter\n\t\t\t\t\t 3. Ignorer\n");
                        int action;
                        scanf("%d", &action);

                        if (action == 1) {
                           printGreenOnWhite("\t\t\t\t\t Demande approuvée, modification du compte en cours.\n");
                            db_update_client_field(rib, champ, nouvelle_valeur);
                            db_add_message(rib, "Votre demande de modification a été approuvée et effectuée avec succès");
                            }
                         else if (action == 2) {
                            printf("Demande rejetée.\n");
                            db_add_message(rib, "Votre demande de modification a été rejetée");
                        } else {
                            db_add_message(rib, "Votre demande de modification est en cours de traitement");
                            fprintf(temp, "\nID demande : %s\n",id);
                            fprintf(temp, "RIB : %s\n",rib);
                            fprintf(temp, "Information a modifier : %s\n",champ);
                            fprintf(temp, "Nouvelle information : %s\n",nouvelle_valeur);
                        }
                    }

                demande_complete = false;
                }
                   if (!demande_trouvee) {
                    printRedOnWhite("\t\t\t\t\t Aucune demande en attente.\n");
                }
                fclose(file);
                fclose(temp);
                remove("demande_modification.txt");
                rename("temp_modification.txt", "demande_modification.txt");

                break;
            }
            case 4:{// Gestion des demandes de carnet cheque
                 FILE *file = fopen("demande_carnet_cheque.txt", "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier demande_carnet_cheque.txt\n");
                return;
                }
                FILE *temp = fopen("temp_carnet_cheque.txt", "w");
                if (!temp) {
                printf("Erreur : Impossible de créer un fichier temporaire\n");
                fclose(file);
                return;
                    }
            char ligne[200];
            char  rib[25];
            char id[10], Nombre[6] , quantite[6];
            bool  demande_complete ,demande_trouvee = false ;
            while (fgets(ligne, sizeof(ligne), file)) {
            if (strncmp(ligne, "ID demande : ", 13) == 0) {
            sscanf(ligne, "ID demande : %s", id);
            demande_trouvee=true ;
            }else if (strncmp(ligne, "RIB : ", 6) == 0) {
            sscanf(ligne, "RIB : %s", rib);
            }else if (strncmp(ligne, "Nombre de feuilles par carnet : ", 32) == 0) {
            sscanf(ligne, "Nombre de feuilles par carnet : %s", Nombre);
            }else if (strncmp(ligne, "Quantite de carnets : ", 22) == 0) {
            sscanf(ligne, "Quantite de carnets : %s", quantite);
            }else if (strncmp(ligne, "---", 3) == 0) { // Fin d'une demande
            demande_complete = true;
            }
            if (demande_complete) {
            printf("\t\t\t\t\t Demande ID : %s\n", id);
            printf("\t\t\t\t\t RIB : %s\n", rib);
            printf("\t\t\t\t\t Nombre de feuilles par carnet : %s\n", Nombre);
            printf("\t\t\t\t\t Quantite de carnets : %s\n", quantite );
            printf("\t\t\t\t\t 1. Approuver\n\t\t\t\t\t 2. Rejeter\n\t\t\t\t\t 3.Ignorer\n");
            int action;
                    scanf("%d", &action);

                        if (action == 1) {
                            afficherCentre("Demande approuvée.\n");
                            db_add_message(rib, "Votre demande de carnet de chèques a été validée, veuillez visiter notre agence pour le recevoir");
                        } else if(action==2) {
                            afficherCentre("Demande rejetée.\n");
                            db_add_message(rib, "Votre demande de carnet de chèques a été rejetée");
                        }
                        else {
                            db_add_message(rib, "Votre demande de carnet de chèques est en cours de traitement");
                            fprintf(temp, "ID demande : %s\n", id);
                            fprintf(temp, "RIB : %s\n", rib);
                            fprintf(temp, "Nombre de feuilles par carnet : %s\n", Nombre);
                            fprintf(temp, "Quantite de carnets : %s\n", quantite);
                        }
                    }

            demande_complete = false;
        }
         if (!demande_trouvee) {
                   printRedOnWhite("\t\t\t\t\t Aucune demande en attente.\n");
                }

                fclose(file);
                fclose(temp);
                remove("demande_carnet_cheque.txt");
                rename("temp_carnet_cheque.txt", "demande_carnet_cheque.txt");
                break;
            }

                case 5: {// Gestion des demandes de récupération de mot de passe

                FILE *file = fopen("demande_recuperation.txt", "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier demande_recuperation.txt\n");
                return;
                }
                FILE *temp = fopen("temp_recuperation.txt", "w");
                if (!temp) {
                printf("Erreur : Impossible de créer un fichier temporaire\n");
                fclose(file);
                return;
                    }

    char ligne[200];
    char rib[25], email[50];
    char id[10];
    bool demande_complete = false,demande_trouvee=false;

    while (fgets(ligne, sizeof(ligne), file)) {
        // Lire les champs de la demande
        if (strncmp(ligne, "ID demande : ", 13) == 0) {
            sscanf(ligne, "ID demande : %s", &id);
            demande_trouvee= true;
        } else if (strncmp(ligne, "RIB : ", 6) == 0) {
            sscanf(ligne, "RIB : %s", rib);
        } else if (strncmp(ligne, "Email : ", 8) == 0) {
            sscanf(ligne, "Email : %s", email);
        } else if (strncmp(ligne, "---", 3) == 0) {
            demande_complete = true;
        }

        if (demande_complete) {
            printf(" \n\t\t\t\t\t Demande ID : %s\n", id);
            printf("\t\t\t\t\t RIB : %s\n\t\t\t\t\t Email : %s\n", rib, email);
            printf("\t\t\t\t\t 1. Approuver\n\t\t\t\t\t 2. Rejeter\n\t\t\t\t\t 3. Ignorer\n");
            int action;
            scanf("%d", &action);

            if (action == 1) {
                printf("\t\t\t\t\t Demande approuvée, récupération du mot de passe en cours.\n");

                // Récupérer le mot de passe depuis la base de données
                char password[20];
                if (db_get_client_password(rib, password)) {
                    printf("\t\t\t\t\t Mot de passe envoyé à l'email %s : %s\n", email, password);
                } else {
                    printFormattedRedOnWhite("\t\t\t\t\t Erreur : Aucun mot de passe trouvé pour le RIB %s.\n", rib);
                }
            } else if (action == 2) {
                afficherCentre("Demande rejetée.\n");
            } else {
                // Réécrire la demande ignorée dans le fichier temporaire
                fprintf(temp, "ID demande : %s\n", id);
                fprintf(temp, "RIB : %s\n", rib);
                fprintf(temp, "Email : %s\n", email);
                fprintf(temp, "---\n");
            }

            // Réinitialiser pour la prochaine demande
            demande_complete = false;
        }
    }
     if (!demande_trouvee) {
                    printRedOnWhite("\t\t\t\t\t Aucune demande en attente.\n");
                }

    fclose(file);
    fclose(temp);
    remove("demande_recuperation.txt");
    rename("temp_recuperation.txt", "demande_recuperation.txt");
    break;
}


            case 6:{
                afficherCentre("Quitter.\n");
                return;
            }
            default:
                printRedOnWhite("\t\t\t\t\t Choix invalide.\n");
        }
    }
}

// Fonction pour payer une facture
void payerFacture(char rib_entre[25], float montant) {
    int type_facture;
    changerCouleur(MAGENTA);
    afficherCentre("\n\n\n\n");
    afficherCentre("********************************************************\n");
    afficherCentre("*                                                      *\n");
    afficherCentre("*         LES TYPES DE FACTURE A PAYER :               *\n\n");
    afficherCentre("*                                                      *\n");
    afficherCentre("*          [1]- TELEPHONE ET INTERNET                  *\n");
    afficherCentre("*          [2]- EAU ET ELECTRICITE                     *\n");
    afficherCentre("*          [3]- TAXES ET ADMINISTRATIONS               *\n");
    afficherCentre("*                                                      *\n");
    afficherCentre("*                                                      *\n");
    afficherCentre("********************************************************\n\n");
    printf("\t\t\t\t\t CHOISISSEZ LE TYPE DE FACTURE A PAYER : ");
    scanf("%d", &type_facture);
    changerCouleur(BLACK);
    clearConsole();

    switch (type_facture) {
        case 1: {
            char societe[50];
            printf("\t\t\t\t\tNom de la societe de telecommunications : ");
            scanf("%s", societe);
            printf("\t\t\t\t\tMontant de la facture : ");
            scanf("%f", &montant);

            printf("\t\t\tPaiement de %.2fDH a la societe '%s' en cours...\n", montant, societe);
            Sleep(9000); //attendre 9 secondes

            if (db_pay_bill(rib_entre, montant)) {
                printGreenOnWhite("\t\t\t\t\t Facture payee avec succes!\n");
                printf("\t\t\t\t\t Montant debite: %.2f DH\n", montant);
            } else {
                printRedOnWhite("\t\t\t\t\t Erreur lors du paiement de la facture.\n");
            }
            Sleep(3000);//Attendre pour que le message apparait
            printf("\n\t\t\t\t\t Appuyez sur Entree pour revenir au menu...");
            getchar(); // Consommer le \n restant
            getchar(); // Attendre l'appui sur Entree
            clearConsole();
            break;
        }
        case 2: {
            int sous_type;
            char societe[50];
            char code_contrat[50];

            printf("\t\t\t\t\t Type de facture :\n\t\t\t\t\t [1] EAU\n\t\t\t\t\t [2] ELECTRICITE\n\n\t\t\t\t\t VOTRE CHOIX: ");
            scanf("%d", &sous_type);
            printf("\t\t\t\t\tNom de la societe de service : ");
            scanf("%s", societe);
            printf("\t\t\t\t\t Code de contrat : ");
            scanf("%s", code_contrat);
            printf("\t\t\t\t\t Montant de la facture : ");
            scanf("%f", &montant);

            printf("\t\t\tPaiement de %.2fDH a la societe '%s' pour le contrat '%s' en cours...\n", montant, societe, code_contrat);
            Sleep(9000);

            if (db_pay_bill(rib_entre, montant)) {
                printGreenOnWhite("\t\t\t\t\t Facture payee avec succes!\n");
                printf("\t\t\t\t\t Montant debite: %.2f DH\n", montant);
            } else {
                printRedOnWhite("\t\t\t\t\t Erreur lors du paiement de la facture.\n");
            }
            Sleep(3000);//Attendre pour que le message apparait
            printf("\n\t\t\t\t\t Appuyez sur Entree pour revenir au menu...");
            getchar(); // Consommer le \n restant
            getchar(); // Attendre l'appui sur Entree
            clearConsole();
            break;
        }
        case 3: {
            char administration[50];
            printf("\t\t\t\t\t Nom de l'administration : ");
            scanf("%s", administration);
            printf("\t\t\t\t\t Montant de la facture : ");
            scanf("%f", &montant);

            printf("\t\t\tPaiement de %.2fDH a l'administration '%s' en cours...\n", montant, administration);
            Sleep(9000);

            if (db_pay_bill(rib_entre, montant)) {
                printGreenOnWhite("\t\t\t\t\t Facture payee avec succes!\n");
                printf("\t\t\t\t\t Montant debite: %.2f DH\n", montant);
            } else {
                printRedOnWhite("\t\t\t\t\t Erreur lors du paiement de la facture.\n");
            }
            Sleep(3000);//Attendre pour que le message apparait
            printf("\n\t\t\t\t\t Appuyez sur Entree pour revenir au menu...");
            getchar(); // Consommer le \n restant
            getchar(); // Attendre l'appui sur Entree
            clearConsole();
            break;
        }
        default:
            printRedOnWhite("\t\t\t\t\t Type de facture invalide.\n");
            Sleep(3000);//Attendre pour que le message apparait
            printf("\n\t\t\t\t\t Appuyez sur Entree pour revenir au menu...");
            getchar(); // Consommer le \n restant
            getchar(); // Attendre l'appui sur Entree
            clearConsole();
    }
}

// Fonction pour effectuer une recharge
void effectuerRecharge(char rib_entre[25], float montant, char numero_telephone[15]) {
    int operateur;
    int typerecharge;
    char numero_telephone_local[15]; // Variable locale pour stocker le numéro

    // Afficher la liste des opérateurs
    afficherCentre("\n\n\n\n");
    afficherCentre("********************************************************\n");
    afficherCentre("*                                                      *\n");
    afficherCentre("*           LISTE DES OPERATEURS                       *\n\n");
    afficherCentre("*            [1] - MAROC TELECOM                       *\n");
    afficherCentre("*            [2] - ORANGE MAROC                        *\n");
    afficherCentre("*            [3] - INWI                                *\n");
    afficherCentre("*                                                      *\n");
    afficherCentre("********************************************************\n");
    printf("\t\t\t\t\t VOTRE CHOIX : ");
    scanf("%d", &operateur);
    clearConsole();

    // Valider le choix de l'opérateur
    if (operateur < 1 || operateur > 3) {
        printRedOnWhite("\t\t\t\t Erreur : choix d'operateur invalide ! Veuillez reessayer.\n");
        return;
    }

    // Demander le numéro de téléphone
    printf("\t\t\t\t\t Entrez le numero de telephone : ");
    scanf("%s", numero_telephone_local);

    // Demander le montant de recharge
    afficherCentre("10DH/20DH/25DH/30DH/50DH/100DH/200DH/300DH/500DH/1000DH/\n");
    printf("\t\t\t\t\t Entrez le montant de recharge : ");
    scanf("%f", &montant);

    // Valider le montant de recharge
    if (montant != 10 && montant != 20 && montant != 25 && montant != 30 &&
        montant != 50 && montant != 100 && montant != 200 && montant != 300 &&
        montant != 500 && montant != 1000) {
        printRedOnWhite("\t\t\t\tErreur : montant invalide ! Veuillez entrer un montant parmi les choix proposes.\n");
        return;
    }

    // Afficher les types de recharge
    afficherCentre("*-----------------------------------------------*\n");
    afficherCentre("*           1. Recharge Normale                 *\n");
    afficherCentre("*           2. Pass SMS *1                      *\n");
    afficherCentre("*           3. Pass National et Internet *2     *\n");
    afficherCentre("*           4. Pass National *22                *\n");
    afficherCentre("*           5. Pass Internet *3                 *\n");
    afficherCentre("*           6. Pass Tout en un *5               *\n");
    afficherCentre("*           7. Pass Réseaux Sociaux *6          *\n");
    afficherCentre("*           8. Pass Premium *9                  *\n");
    afficherCentre("*           9. Pass Gaming *88                  *\n");
    afficherCentre("*-----------------------------------------------*\n");
    printGreenOnWhite("\t\t\t\t\t Entrez le type de recharge : ");
    scanf("%d", &typerecharge);
    clearConsole();

    // Valider le type de recharge
    if (typerecharge < 1 || typerecharge > 9) {
        printRedOnWhite("\n\n\n\n\t\t\t\tErreur : type de recharge invalide ! Veuillez selectionner un type entre 1 et 9.\n");
        Sleep(3000);
        clearConsole();
        return;
    }

    // Effectuer la recharge
    if (db_mobile_recharge(rib_entre, montant, numero_telephone_local)) {
        printFormattedGreenOnWhite("\n\n\n\t\t\t\tRecharge effectuee avec succes pour le numero %s !\n", numero_telephone_local);
    } else {
        printRedOnWhite("\t\t\t\t\t Recharge echouee.\n");
    }

    Sleep(3000);
    clearConsole();
}

// Fonction pour payer une vignette
void payerVignette(char rib_entre[25], float montant) {
    int type_carte;
    char matricule[20];
    int puissance_fiscale;
    int carburant;
    char email[50];
    char numero_telephone[15];
    int annee_vignette;

    // Type de carte grise
    afficherCentre("\n\n\n\n");
    afficherCentre("************************************************************\n");
    afficherCentre("*                                                          *\n");
    afficherCentre("*           CHOISISSEZ LE TYPE DE CARTE GRISE :            *\n");
    afficherCentre("*                                                          *\n");
    afficherCentre("*          [1]- ELECTRONIQUE                               *\n");
    afficherCentre("*          [2]- CARTONNEE                                  *\n");
    afficherCentre("*                                                          *\n");
    afficherCentre("*                                                          *\n");
    afficherCentre("************************************************************\n");
    printf("\t\t\t\t\t DONNEZ VOTRE CHOIX : ");
    scanf("%d", &type_carte);
    clearConsole();

    // Valider le type de carte grise
    if (type_carte != 1 && type_carte != 2) {
        printRedOnWhite("\t\t\t\t\t Erreur : choix de carte grise invalide.\n");
        return;
    }

    // Matricule
    printf("\t\t\t\t\t Entrez le matricule : ");
    scanf("%s", matricule);
    printf("\t\t\t\t\t Entrez la puissance fiscale : ");
    scanf("%d", &puissance_fiscale);

    // Type de carburant
    afficherCentre("\n\n\n\n");
    afficherCentre("*************************************************************\n");
    afficherCentre("*                                                           *\n");
    afficherCentre("*             CHOISISSEZ LE TYPE DE CARBURANT :             *\n");
    afficherCentre("*                                                           *\n");
    afficherCentre("*                  [1]- ESSENCE                             *\n");
    afficherCentre("*                  [2]- DIESEL                              *\n");
    afficherCentre("*                  [3]- HYBRIDE                             *\n");
    afficherCentre("*                  [4]- ELECTRIQUE                          *\n");
    afficherCentre("*                                                           *\n");
    afficherCentre("*                                                           *\n");
    afficherCentre("*************************************************************\n");
    printf("\t\t\t\t\t VOTRE CHOIX : ");
    scanf("%d", &carburant);
    clearConsole();

    // Valider le choix de carburant
    if (carburant < 1 || carburant > 4) {
        printRedOnWhite("\t\t\t\t\t Erreur : choix de carburant invalide.\n");
        return;
    }

    printf("\t\t\t\t\t Entrez votre email : ");
    scanf("%s", email);
    printf("\t\t\t\t\t Entrez votre numero de telephone : ");
    scanf("%s", numero_telephone);

    // Année de vignette
    printf("\t\t\t\t\t Entrez l'annee de la vignette : ");
    scanf("%d", &annee_vignette);

    // Montant
    printf("\t\t\t\t\t Entrez le montant a payer : ");
    scanf("%f", &montant);

    // Effectuer le paiement
    if (db_pay_stamp(rib_entre, montant)) {
        printGreenOnWhite("\t\t\t\tPaiement de la vignette effectue avec succes. Solde mis a jour.\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Paiement echoue.\n");
    }

    Sleep(3000);
    clearConsole();
}

// Fonction pour afficher la boîte de messages
void afficherboitemessage(char rib_entre[25]) {
    printf("\n");
    afficherCentre("********************************************************\n");
    afficherCentre("*                  BOITE DE MESSAGES                   *\n");
    afficherCentre("********************************************************\n");

    // Récupérer et afficher les messages depuis la base de données
    if (!db_get_messages(rib_entre)) {
        printf("\t\t\t\t\t Aucun message disponible.\n");
    }

    afficherCentre("********************************************************\n");
}

// Fonctions d'interface A-G (pour l'affichage du logo/interface)
void A() {
    printf("\n\n\n\n");
    afficherCentre("********************************************************\n");
}

void B() {
    afficherCentre("*                                                      *\n");
}

void C() {
    afficherCentre("*              SYSTEME BANCAIRE SQLITE                *\n");
}

void D() {
    afficherCentre("*                                                      *\n");
}

void E() {
    afficherCentre("*                  GESTION DES COMPTES                *\n");
}

void F() {
    afficherCentre("*                                                      *\n");
}

void G() {
    afficherCentre("********************************************************\n");
}

// Fonction principale
int main() {
    // Configurer la taille de la console
    system("mode con: cols=90 lines=30");

    // Mettre la console en UTF-8 AVANT tout affichage
    SetConsoleOutputCP(65001);

    // Initialiser la base de donnees
    if (!db_open("banking.db")) {
        afficherErreur("Impossible d'ouvrir la base de donnees!");
        return 1;
    }

    // Configuration de la console (couleurs)
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (15 << 4) | 0); // Fond blanc, texte noir

    // Afficher le logo moderne et animé (lettre par lettre)
    afficherLogoSARAnime();

    int choix;
    char RIB[25], password[20];

    do {
        menuprincipal();
        scanf("%d", &choix);
        clearConsole();

        if (choix == 1) {
            login_admin();
        } else if (choix == 2) {
            menuClient();
            int choix_client;
            scanf("%d", &choix_client);
            clearConsole();

            if (choix_client == 1) {
                remplirDemande_creation();
            } else if (choix_client == 2) {
                login(RIB, password);
            }
        }
    } while (choix != 3);

    // Fermer la base de donnees
    db_close();
    afficherSucces("Merci d'avoir utilise SAR BANK!");
    return 0;
}

// Fonction pour afficher les informations d'un compte
void afficherInfoCompte() {
    char rib[25];
    printf("\t\t\t\t\t Entrez le RIB du compte : ");
    scanf("%s", rib);

    char nom[50], prenom[50], cin[20], type_compte[20], email[50], tele[15], date[12];
    double solde;
    int account_number;

    if (db_get_client_full_info(rib, nom, prenom, cin, type_compte, tele, email, date, &solde, &account_number)) {
        printf("\n\t\t\t\t\t === INFORMATIONS DU COMPTE ===\n");
        printf("\t\t\t\t\t RIB: %s\n", rib);
        printf("\t\t\t\t\t Numéro de compte: %d\n", account_number);
        printf("\t\t\t\t\t Nom: %s\n", nom);
        printf("\t\t\t\t\t Prénom: %s\n", prenom);
        printf("\t\t\t\t\t CIN: %s\n", cin);
        printf("\t\t\t\t\t Type de compte: %s\n", type_compte);
        printf("\t\t\t\t\t Téléphone: %s\n", tele);
        printf("\t\t\t\t\t Email: %s\n", email);
        printf("\t\t\t\t\t Date de naissance: %s\n", date);
        printf("\t\t\t\t\t Solde: %.2f DH\n", solde);
        printf("\t\t\t\t\t ===============================\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Compte non trouvé.\n");
    }

    printf("\n\t\t\t\t\t Appuyez sur Entrée pour continuer...");
    getchar();
    getchar();
    clearConsole();
}

// Fonction pour ajouter un administrateur
void ajouterAdmin() {
    char nom[20], prenom[20], tel[20], password[20];

    printf("\t\t\t\t\t Entrez le nom d'utilisateur : ");
    scanf("%s", nom);

    printf("\t\t\t\t\t Entrez le prénom : ");
    scanf("%s", prenom);

    printf("\t\t\t\t\t Entrez le téléphone : ");
    scanf("%s", tel);

    printf("\t\t\t\t\t Entrez le mot de passe : ");
    scanf("%s", password);

    if (db_add_admin(nom, prenom, tel, password)) {
        printGreenOnWhite("\t\t\t\t\t Administrateur ajouté avec succès !\n");
    } else {
        printRedOnWhite("\t\t\t\t\t Erreur lors de l'ajout de l'administrateur.\n");
    }

    printf("\n\t\t\t\t\t Appuyez sur Entrée pour continuer...");
    getchar();
    getchar();
    clearConsole();
}

// Fonction pour afficher les statistiques admin
void afficherStatistiquesAdmin() {
    printf("\n\t\t\t\t\t === STATISTIQUES GÉNÉRALES ===\n");

    double total_balance = db_get_total_balance();
    printf("\t\t\t\t\t Solde total de la banque: %.2f DH\n", total_balance);

    printf("\n\t\t\t\t\t Distribution par type de compte:\n");
    db_get_account_type_distribution();

    printf("\n\t\t\t\t\t Distribution par type de transaction:\n");
    db_get_transaction_type_distribution();

    printf("\n\t\t\t\t\t Appuyez sur Entrée pour continuer...");
    getchar();
    getchar();
    clearConsole();
}

// Fonction pour les statistiques des comptes
void statistiquesComptes() {
    printf("\n\t\t\t\t\t === STATISTIQUES DES COMPTES ===\n");
    db_get_all_clients();

    printf("\n\t\t\t\t\t Appuyez sur Entrée pour continuer...");
    getchar();
    getchar();
    clearConsole();
}

// Fonction pour générer les graphiques
void genererGraphiques() {
    printf("\n\t\t\t\t\t Génération des graphiques en cours...\n");

    // Exporter les données pour les graphiques
    export_transactions_mensuelles();
    export_types_comptes();

    // Générer les scripts Gnuplot
    FILE *gnuplot_script = fopen("graphique_transactions.gp", "w");
    if (gnuplot_script) {
        fprintf(gnuplot_script, "set terminal png size 800,600\n");
        fprintf(gnuplot_script, "set output 'transactions_mensuelles.png'\n");
        fprintf(gnuplot_script, "set title 'Transactions Mensuelles'\n");
        fprintf(gnuplot_script, "set xlabel 'Mois'\n");
        fprintf(gnuplot_script, "set ylabel 'Montant (DH)'\n");
        fprintf(gnuplot_script, "plot 'transactions_mensuelles.txt' using 1:2 with lines title 'Transactions'\n");
        fclose(gnuplot_script);
    }

    FILE *gnuplot_script2 = fopen("graphique_types.gp", "w");
    if (gnuplot_script2) {
        fprintf(gnuplot_script2, "set terminal png size 800,600\n");
        fprintf(gnuplot_script2, "set output 'types_comptes.png'\n");
        fprintf(gnuplot_script2, "set title 'Distribution des Types de Comptes'\n");
        fprintf(gnuplot_script2, "set xlabel 'Type de Compte'\n");
        fprintf(gnuplot_script2, "set ylabel 'Nombre de Comptes'\n");
        fprintf(gnuplot_script2, "plot 'types_comptes.txt' using 1:2 with boxes title 'Comptes'\n");
        fclose(gnuplot_script2);
    }

    printGreenOnWhite("\t\t\t\t\t Graphiques générés avec succès !\n");
    printf("\t\t\t\t\t Fichiers créés:\n");
    printf("\t\t\t\t\t - transactions_mensuelles.png\n");
    printf("\t\t\t\t\t - types_comptes.png\n");

    printf("\n\t\t\t\t\t Appuyez sur Entrée pour continuer...");
    getchar();
    getchar();
    clearConsole();
}

// ========== NOUVELLES FONCTIONS POUR INTERFACE MODERNE ==========

// Fonction pour afficher une ligne de séparation moderne
void afficherLigneSeparation() {
    printf("╔");
    for (int i = 0; i < BOX_WIDTH - 2; i++) {
        printf("═");
    }
    printf("╗\n");
}

// Fonction pour afficher une ligne vide avec bordures
void afficherLigneVide() {
    printf("║");
    for (int i = 0; i < BOX_WIDTH - 2; i++) {
        printf(" ");
    }
    printf("║\n");
}

// Fonction pour afficher une ligne de fermeture
void afficherLigneFermeture() {
    printf("╚");
    for (int i = 0; i < BOX_WIDTH - 2; i++) {
        printf("═");
    }
    printf("╝\n");
}

// Fonction pour afficher un titre centré dans une boîte
void afficherTitreBoite(const char *titre) {
    int longueur_titre = strlen(titre);
    int espaces_gauche = (BOX_WIDTH - 2 - longueur_titre) / 2;
    int espaces_droite = BOX_WIDTH - 2 - longueur_titre - espaces_gauche;

    printf("║");
    for (int i = 0; i < espaces_gauche; i++) printf(" ");
    printf("%s", titre);
    for (int i = 0; i < espaces_droite; i++) printf(" ");
    printf("║\n");
}

// Fonction pour afficher un menu moderne
void afficherMenuModerne(const char *titre, const char *options[], int nb_options) {
    clearConsole();
    afficherLigneSeparation();
    afficherTitreBoite(titre);
    afficherLigneVide();

    for (int i = 0; i < nb_options; i++) {
        char ligne[BOX_WIDTH];
        int n = snprintf(ligne, sizeof(ligne), "  [%d] %s ", i + 1, options[i]);
        printf("║");
        printf("%s", ligne);
        for (int j = n; j < BOX_WIDTH - 2; j++) printf(" ");
        printf("║\n");
    }

    afficherLigneVide();
    afficherLigneFermeture();
}

// Fonction pour afficher un message de succès stylé
void afficherSucces(const char *message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttrs = csbi.wAttributes;

    SetConsoleTextAttribute(hConsole, (15 << 4) | 2); // Fond blanc, texte vert
    printf("✅ %s\n", message);

    SetConsoleTextAttribute(hConsole, originalAttrs);
}

// Fonction pour afficher un message d'erreur stylé
void afficherErreur(const char *message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttrs = csbi.wAttributes;

    SetConsoleTextAttribute(hConsole, (15 << 4) | 4); // Fond blanc, texte rouge
    printf("❌ %s\n", message);

    SetConsoleTextAttribute(hConsole, originalAttrs);
}

// Fonction pour afficher un message d'information stylé
void afficherInfo(const char *message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttrs = csbi.wAttributes;

    SetConsoleTextAttribute(hConsole, (15 << 4) | 1); // Fond blanc, texte bleu
    printf("ℹ️  %s\n", message);

    SetConsoleTextAttribute(hConsole, originalAttrs);
}

// Fonction pour afficher un message d'avertissement stylé
void afficherAvertissement(const char *message) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttrs = csbi.wAttributes;

    SetConsoleTextAttribute(hConsole, (15 << 4) | 6); // Fond blanc, texte jaune
    printf("⚠️  %s\n", message);

    SetConsoleTextAttribute(hConsole, originalAttrs);
}

// Fonction pour créer une boîte d'information
void afficherBoiteInfo(const char *titre, const char *contenu) {
    clearConsole();
    afficherLigneSeparation();
    afficherTitreBoite(titre);
    afficherLigneVide();

    // Diviser le contenu en lignes de BOX_WIDTH-4 caractères maximum
    char ligne[BOX_WIDTH];
    int pos = 0;
    int len = strlen(contenu);

    for (int i = 0; i < len; i++) {
        ligne[pos++] = contenu[i];

        if (pos >= BOX_WIDTH - 4 || contenu[i] == '\n' || i == len - 1) {
            ligne[pos] = '\0';
            printf("║  %-*s ║\n", BOX_WIDTH - 4, ligne);
            pos = 0;
        }
    }

    afficherLigneVide();
    afficherLigneFermeture();
    printf("\nAppuyez sur Entrée pour continuer...");
    getchar();
    getchar();
}

// Nouveau logo ASCII art géant, animé et centré verticalement et horizontalement
void afficherLogoSARAnime() {
    clearConsole();
    // Grand logo ASCII art pour 'SAR BANK'
    const char *logo[] = {
    "                       ███████╗ █████╗ ██████╗     ██████╗  █████╗ ███╗   ██╗██╗  ██╗",
    "                       ██╔════╝██╔══██╗██╔══██╗    ██╔══██╗██╔══██╗████╗  ██║██║ ██╔╝",
    "                       ███████╗███████║██████╔╝    ██████╔╝███████║██╔██╗ ██║█████╔╝ ",
    "                       ╚════██║██╔══██║██╔══██╗    ██╔══██╗██╔══██║██║╚██╗██║██╔═██╗ ",
    "                       ███████║██║  ██║██║  ██║    ██████╔╝██║  ██║██║ ╚████║██║  ██╗",
    "                       ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝"
};
    int lignes = 6;
    int largeur_console = BOX_WIDTH, hauteur_console = 25;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        largeur_console = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        hauteur_console = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    WORD originalAttrs = csbi.wAttributes;

    // Calcul du nombre de lignes vides à afficher avant le logo
    int lignes_avant = (hauteur_console - lignes - 2) / 2; // -2 pour le slogan
    if (lignes_avant < 0) lignes_avant = 0;
    for (int i = 0; i < lignes_avant; i++) printf("\n");

    // Affichage du logo
    for (int l = 0; l < lignes; l++) {
        int len = strlen(logo[l]);
        int espaces = (largeur_console - len) / 2;
        if (espaces < 0) espaces = 0;
        for (int i = 0; i < espaces; i++) printf(" ");
        for (int c = 0; c < len; c++) {
            SetConsoleTextAttribute(hConsole, (15 << 4) | 5); // Violet sur blanc
            printf("%c", logo[l][c]);
            SetConsoleTextAttribute(hConsole, originalAttrs);
            fflush(stdout);
            Sleep(2);
        }
        printf("\n");
    }
    printf("\n");
    // Slogan centré
    const char *slogan = "Votre banque moderne et sécurisée";
    int slen = strlen(slogan);
    int espaces = (largeur_console - slen) / 2;
    if (espaces < 0) espaces = 0;
    for (int i = 0; i < espaces; i++) printf(" ");
    SetConsoleTextAttribute(hConsole, (15 << 4) | 2); // Vert sur blanc
    for (int i = 0; i < slen; i++) {
        printf("%c", slogan[i]);
        fflush(stdout);
        Sleep(15);
    }
    SetConsoleTextAttribute(hConsole, originalAttrs);
    printf("\n\n");
    Sleep(2000); // Pause de 2 secondes pour laisser le logo visible
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include<windows.h>
#include <time.h>
#define MAX_COMPTES 100
#define MAX_DEMANDES 100
#define MAX_ADMINS 10
#include <conio.h> // Pour getch()
#ifdef _WIN32
#else
#include <sys/ioctl.h>
#endif
#include <stdarg.h> // Pour va_list et fonctions associées
// Définitions des couleurs
#define RESET   "\033[0m"
#define BLUE    "\033[34m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define WHITE   "\033[37m"
#define BLACK   "\033[30m"
#define MAGENTA "\033[35m"
#define PINK "\033[95m" // Code ANSI pour une couleur rose clair
// Definition des fichiers
const char *Clientfile = "client.txt";
const char *demande_creationfile = "demande_creation.txt";
const char *demande_suppressionfile = "demande_suppression.txt";
const char *demande_recuperationfile = "demande_recuperation.txt";
const char *demande_modification = "demande_modification.txt";
const char *demande_carnet_cheque = "demande_carnet_cheque.txt";
const char *historique_clientfile = "historique_client.txt";
const char *historique_transactionsfile = "historique_transactions.txt";
const char *adminfile = "admins.txt";
const char *LoginAdminfile = "loginAdmin.txt";
const char *LoginClientfile = "login.txt";
const char *boite_messagefile = "boite_message.txt";
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
    int type_demande;       // Type de la demande (1: Virement, 2: Suppression, 3: Recuperation mot de passe)
    bool valide;
    infoclient Client;
    float montant;          // Montant pour virement (si applicable)
    char RIB_dest[25]; // Numero de compte destinataire (si applicable)
    char nouvelles_infos[100]; // Informations mises a jour pour une demande de modification (si applicable)
} Demande;

// Structure pour l'admin
typedef struct {
    int id_admin;
    char nom[20];
    char prenom[20];
    char tel[20];
    char password[20];
} Admin;
//CLIENT
//fonction pour effacer la console
void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void menuClient() {

    changerCouleur(MAGENTA);
    afficherCentre("\n\n\n\n");
    afficherCentre("********************************************************\n");
    afficherCentre("*          BIENVENUE SUR LE MENU CLIENT                *\n");
    changerCouleur(BLACK);
    afficherCentre("*                                                      *\n");
    afficherCentre("*          [1]- S'INSCRIRE                             *\n");//demande cree dans le fichier demande
    afficherCentre("*          [2]- SE CONNECTER                           *\n");// fonction login
    afficherCentre("*          [0]- REVENIR AU MENU PRINCIPAL              *\n");
    changerCouleur(MAGENTA);
    afficherCentre("*                                                      *\n");
    afficherCentre("********************************************************\n\n");
    printf("\t\t\t\t\t TENTREZ VOTRE CHOIX : ");
     changerCouleur(BLACK);
}
void menuprincipal(){
    changerCouleur(MAGENTA);
    afficherCentre("\n\n\n\n");
    afficherCentre("********************************************************\n");
    afficherCentre("*             BIENVENUE DANS SAR BANK                  *\n");
    afficherCentre("********************************************************\n");
    changerCouleur(BLACK);
    afficherCentre("*                                                      *\n");
    afficherCentre("*               [1]-ESPACE ADMIN                       *\n");
    afficherCentre("*               [2]-ESPACE CLIENT                      *\n");
    afficherCentre("*               [0]- EXIT                              *\n");
    changerCouleur(MAGENTA);
    afficherCentre("*                                                      *\n");
    afficherCentre("********************************************************\n\n");
    printf("\t\t\t\t\t ENTREZ VOTRE CHOIX ...");
    changerCouleur(BLACK);
}
void read_password(char *password) { // fonction pour masquer  le mot de passe
    int i = 0;
    char ch;

    while (1) {
        ch = getch();  // Lit un caractère sans l'afficher

        if (ch == 13) {  // Si la touche Entrée (caractère 13) est pressée
            password[i] = '\0';  // Termine la chaîne de caractères
            break;
        } else if (ch == 8) {  // Si la touche Backspace (caractère 8) est pressée
            if (i > 0) {
                i--;
                printf("\b \b");  // Efface l'étoile
            }
        } else {
            password[i++] = ch;
            printf("*");  // Affiche une étoile
        }
    }
    printf("\n");
}
void afficherhistoriqueclient(char rib_entre[25]) {

    FILE *f = fopen(historique_clientfile, "r");
    if (f == NULL) {
        printRedOnWhite("\t\t\t\t\t Aucun compte trouve.\n");
        return;
    }

    char ligne[200];
    bool compte_trouve = false;
    printf("\n");
   afficherCentre("====================================== voici votre historique =========================================\n\n");
    while (fgets(ligne, sizeof(ligne), f)) {
        char rub[25];
        if (sscanf(ligne, "RIB : %s", &rub) == 1 && strcmp(rub, rib_entre) == 0 ) {
            compte_trouve = true;
            int i;
            for ( i = 0; i < 2; i++) { // Lire les 2 lignes suivantes
                if (fgets(ligne, sizeof(ligne), f)) {
            afficherCentreSF("%s", ligne);
                }
            }
			afficherCentre("--------------------------------------------------------------------\n");
        }

    }

    fclose(f);

    if (!compte_trouve) {
        printRedOnWhite("\t\t\t\t\t Aucun historique trouve .\n");
    }


}



void afficherboitemessage(char rib_entre[25]) {

    FILE *f = fopen(boite_messagefile, "r");
    if (f == NULL) {
        printRedOnWhite("\t\t\t\t\t Aucun compte trouve.\n");
        return;
    }

    char ligne[200];
    bool compte_trouve = false;
    printf("\n");
   afficherCentre("======================= boite message =========================\n\n");
    while (fgets(ligne, sizeof(ligne), f)) {
        char rub[25];

        if (sscanf(ligne, "RIB : %s", &rub) == 1 && strcmp(rub, rib_entre) == 0 ) {
            compte_trouve = true;
            int i;
            for ( i = 0; i < 2; i++) { // Lire les 2 lignes suivantes
                if (fgets(ligne, sizeof(ligne), f)) {
           afficherCentreSF("%s", ligne);
                }
            }
			afficherCentre("--------------------------------------------------------------------\n");
        }

    }

    fclose(f);

    if (!compte_trouve) {
        printRedOnWhite("\t\t\t\t\t\tAucun message trouve .\n");
    }
}
void afficherInformations(char rib_entre[25]) {

    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printRedOnWhite("\t\t\t\t\t Aucun compte trouve.\n");
        return;
    }

    char ligne[200];
    bool compte_trouve = false;
    while (fgets(ligne, sizeof(ligne), f)) {
        char rub[25];
        if (sscanf(ligne, "RIB : %s", &rub) == 1 && strcmp(rub, rib_entre) == 0 ) {
            compte_trouve = true;
            printf("\n");
            afficherCentre("------------------------------------------- Informations de votre compte -------------------------------------------------\n");
            afficherCentreSF("%s", ligne);
            int i;
            for ( i = 0; i < 10; i++) { // Lire les 10 lignes suivantes
                if (fgets(ligne, sizeof(ligne), f)) {
            afficherCentreSF("%s", ligne);
                }
            }
           afficherCentre("--------------------------------\n");
            break;
        }
    }

    fclose(f);

    if (!compte_trouve) {
        printRedOnWhite("\t\t\t\t\t ERREUR!veuillez-nous contactez!.\n");
    }
}

void effectuerDepot(char rib_entre[25], float montant) {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier client.\n");
        return;
    }

    FILE *temp = fopen("temp_client.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de creer le fichier temporaire.\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_trouve = false;

    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_lu[25];

        // Vérifier si la ligne contient un RIB
        if (sscanf(ligne, "RIB : %s", rib_lu) == 1) {
            fprintf(temp, "%s", ligne); // Copier la ligne du RIB dans le fichier temporaire
            if (strcmp(rib_lu, rib_entre) == 0) {
                // Compte trouvé, sauter les lignes jusqu'au solde
                int i;
                for (i = 0; i < 9; i++) { // Ajuster le nombre selon la structure du fichier
                    fgets(ligne, sizeof(ligne), f);
                    fprintf(temp, "%s", ligne); // Copier ces lignes dans le fichier temporaire
                }

                // Lire et modifier la ligne du solde
                if (fgets(ligne, sizeof(ligne), f)) {
                    float solde;
                    if (sscanf(ligne, "Solde initial : %f", &solde) == 1) {
                        solde += montant; // Ajouter le montant au solde
                        compte_trouve = true;
                        fprintf(temp, "Solde initial : %.2f\n", solde); // Écrire le nouveau solde
                    } else {
                        // Si ce n'est pas la ligne du solde, on l'écrit normalement
                        fprintf(temp, "%s", ligne);
                    }
                }
            }
        } else {
            // Écrire les autres lignes dans le fichier temporaire normalement
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    if (compte_trouve) {
        // Remplacer l'ancien fichier par le fichier temporaire
        remove(Clientfile); // Supprimer l'ancien fichier
        rename("temp_client.txt", Clientfile); // Renommer le fichier temporaire
        printGreenOnWhite("\t\t\t\tDepot effectue avec succes. Nouveau solde mis a jour.\n");

        // Enregistrer l'opération dans le fichier historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(historique, "Vous avez effectue un depot de : %.2f DH\n\n", montant); // Ligne 3 : Message personnalisé

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }
    } else {
        printRedOnWhite("\t\t\t\t Compte introuvable. Depot annule.\n");
        remove("temp_client.txt"); // Supprimer le fichier temporaire inutilisé
    }
}

void effectuerRetrait(char rib_entre[25], float montant) {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier client.\n");
        return;
    }

    FILE *temp = fopen("temp_client.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de creer le fichier temporaire.\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_trouve = false;
    bool solde_suffisant = false;

    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_lu[25];

        // Vérifier si la ligne contient un RIB
        if (sscanf(ligne, "RIB : %s", rib_lu) == 1) {
            fprintf(temp, "%s", ligne); // Copier la ligne du RIB dans le fichier temporaire
            if (strcmp(rib_lu, rib_entre) == 0) {
                // Compte trouvé, sauter les lignes jusqu'au solde
                int i;
                for (i = 0; i < 9; i++) { // Ajuster le nombre selon la structure du fichier
                    fgets(ligne, sizeof(ligne), f);
                    fprintf(temp, "%s", ligne); // Copier ces lignes dans le fichier temporaire
                }

                // Lire et modifier la ligne du solde
                if (fgets(ligne, sizeof(ligne), f)) {
                    float solde;
                    if (sscanf(ligne, "Solde initial : %f", &solde) == 1) {
                        if (solde >= montant) {
                            // Si le solde est suffisant pour le retrait
                            solde -= montant; // Effectuer le retrait
                            solde_suffisant = true;
                            compte_trouve = true;
                            fprintf(temp, "Solde initial : %.2f\n", solde); // Écrire le nouveau solde
                        } else {
                            // Si le solde est insuffisant
                            printRedOnWhite("\t\t\t\t Solde insuffisant pour effectuer le retrait.\n");
                            solde_suffisant = false;
                            fprintf(temp, "%s", ligne); // Conserver la ligne de solde initial
                        }
                    } else {
                        // Si ce n'est pas la ligne du solde, on l'écrit normalement
                        fprintf(temp, "%s", ligne);
                    }
                }
            }
        } else {
            // Écrire les autres lignes dans le fichier temporaire normalement
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    if (compte_trouve && solde_suffisant) {
        // Remplacer l'ancien fichier par le fichier temporaire
        remove(Clientfile); // Supprimer l'ancien fichier
        rename("temp_client.txt", Clientfile); // Renommer le fichier temporaire
        printGreenOnWhite("\t\t\t\t\t Retrait effectue avec succes. Nouveau solde mis a jour.\n");
        // Enregistrer l'opération dans le fichier historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre);
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(historique, "Vous avez effectue un retrait de : %.2f DH\n\n", montant);

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }

    } else if (!compte_trouve) {
        printRedOnWhite("\t\t\t\t\tCompte introuvable. Retrait annule.\n");
        remove("temp_client.txt"); // Supprimer le fichier temporaire inutilisé
    }else if (compte_trouve && !solde_suffisant){

     printRedOnWhite("\t\t\t\t\t Retrait annule.\n");
        remove("temp_client.txt"); // Supprimer le fichier temporaire inutilisé
    }
}

void effectuerVirement(char rib_entre[25], char rib_destination[25], float montant) {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier client.\n");
        return;
    }

    FILE *temp = fopen("temp_client.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de creer le fichier temporaire.\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_emetteur_trouve = false;
    bool compte_destinataire_trouve = false;
    bool solde_suffisant = false;

    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_lu[25];

        // Vérifier si la ligne contient un RIB
        if (sscanf(ligne, "RIB : %s", rib_lu) == 1) {
            fprintf(temp, "%s", ligne); // Copier la ligne du RIB dans le fichier temporaire

            if (strcmp(rib_lu, rib_entre) == 0) {
                // Compte émetteur trouvé, sauter les lignes jusqu'au solde
                compte_emetteur_trouve = true;
                int i;
                for (i = 0; i < 9; i++) {
                    fgets(ligne, sizeof(ligne), f);
                    fprintf(temp, "%s", ligne); // Copier ces lignes
                }

                // Lire et modifier la ligne du solde
                if (fgets(ligne, sizeof(ligne), f)) {
                    float solde_emetteur;
                    if (sscanf(ligne, "Solde initial : %f", &solde_emetteur) == 1) {
                        if (solde_emetteur >= montant) {
                            solde_emetteur -= montant;
                            solde_suffisant = true;
                        }
                        fprintf(temp, "Solde initial : %.2f\n", solde_emetteur);
                    }
                }
            } else if (strcmp(rib_lu, rib_destination) == 0) {
                // Compte destinataire trouvé, sauter les lignes jusqu'au solde
                compte_destinataire_trouve = true;
                int i;
                for (i = 0; i < 9; i++) {
                    fgets(ligne, sizeof(ligne), f);
                    fprintf(temp, "%s", ligne); // Copier ces lignes
                }

                // Lire et modifier la ligne du solde
                if (fgets(ligne, sizeof(ligne), f)) {
                    float solde_destinataire;
                    if (sscanf(ligne, "Solde initial : %f", &solde_destinataire) == 1 && solde_suffisant) {
                        solde_destinataire += montant;
                        fprintf(temp, "Solde initial : %.2f\n", solde_destinataire);
                    } else {
                        fprintf(temp, "%s", ligne); // Conserver le solde si non modifiable
                    }
                }
            }
        } else {
            // Copier toutes les autres lignes
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    if (compte_emetteur_trouve && compte_destinataire_trouve && solde_suffisant) {
        // Remplacer l'ancien fichier par le fichier temporaire
        remove(Clientfile);
        rename("temp_client.txt", Clientfile);
        printGreenOnWhite("\t\t\t\t\t Virement effectue avec succes. Solde mis a jour.\n");
        // Enregistrer l'opération dans le fichier historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(historique, "Vous avez effectue un virement de : %.2f DH au compte %s\n\n", montant,rib_destination); // Ligne 3 : Message personnalisé

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }
    } else {
            printRedOnWhite("\t\t\t\t\t Virement echoue.\n");
        if (!compte_emetteur_trouve) {
            printRedOnWhite("\t\t\t\t\t Compte emetteur introuvable.\n");
        }
        if (!compte_destinataire_trouve) {
            printRedOnWhite("\t\t\t\t\t Compte destinataire introuvable.\n");
        }
        if (compte_emetteur_trouve && !solde_suffisant) {
           printRedOnWhite("\t\t\t\t Solde insuffisant pour effectuer le virement.\n");
        }
        remove("temp_client.txt"); // Supprimer le fichier temporaire inutilisé
    }
}
void payerFacture(char rib_entre[25], float montant) {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier client.\n");
        return;
    }

    FILE *temp = fopen("temp_client.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de creer le fichier temporaire.\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_trouve = false;
    bool solde_suffisant = false;

    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_lu[25];

        // Vérifier si la ligne contient un RIB
        if (sscanf(ligne, "RIB : %s", rib_lu) == 1) {
            fprintf(temp, "%s", ligne); // Copier la ligne du RIB

            if (strcmp(rib_lu, rib_entre) == 0) {
                compte_trouve = true;

                // Copier les lignes suivantes jusqu'au solde
                int i;
                for ( i = 0; i < 9; i++) {
                    fgets(ligne, sizeof(ligne), f);
                    fprintf(temp, "%s", ligne);
                }

                // Lire et modifier le solde
                if (fgets(ligne, sizeof(ligne), f)) {
                    float solde;
                    if (sscanf(ligne, "Solde initial : %f", &solde) == 1) {
                        if (solde >= montant) {
                            solde -= montant;
                            solde_suffisant = true;
                            fprintf(temp, "Solde initial : %.2f\n", solde);
                        } else {
                            fprintf(temp, "%s", ligne); // Ne pas modifier si le solde est insuffisant
                        }
                    }
                }
            }
        } else {
            // Copier toutes les autres lignes
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    if (compte_trouve && solde_suffisant) {
        remove(Clientfile);
        rename("temp_client.txt", Clientfile);
        printGreenOnWhite("\t\t\t\tPaiement effectue avec succes. Solde mis a jour.\n");
        // Écrire les informations dans l'historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(historique, "Vous avez payer une facture de montant : %.2f DH \n\n", montant);

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }
    } else {
            printf("\t\t\t\t\t  Paiement echoue.\n");
        if (!compte_trouve) {
            printf("\t\t\t\t\t Compte introuvable.\n");
        }
        if (compte_trouve && !solde_suffisant) {
           printRedOnWhite("\t\t\t\tSolde insuffisant pour effectuer le paiement.\n");
        }
        remove("temp_client.txt");
    }
}

void effectuerRecharge(char rib_entre[25], float montant,char numero_telephone[15]) {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier client.\n");
        return;
    }

    FILE *temp = fopen("temp_client.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de créer le fichier temporaire.\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_trouve = false;
    bool solde_suffisant = false;

    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_lu[25];

        // Vérifier si la ligne contient un RIB
        if (sscanf(ligne, "RIB : %s", rib_lu) == 1) {
            fprintf(temp, "%s", ligne); // Copier la ligne du RIB dans le fichier temporaire

            if (strcmp(rib_lu, rib_entre) == 0) {
                // Compte trouvé, sauter les lignes jusqu'au solde
                compte_trouve = true;
                int i;
                for (i = 0; i < 9; i++) {
                    fgets(ligne, sizeof(ligne), f);
                    fprintf(temp, "%s", ligne); // Copier ces lignes
                }

                // Lire et modifier la ligne du solde
                if (fgets(ligne, sizeof(ligne), f)) {
                    float solde;
                    if (sscanf(ligne, "Solde initial : %f", &solde) == 1) {
                        if (solde >= montant) {
                            solde -= montant;
                            solde_suffisant = true;
                        }
                        fprintf(temp, "Solde initial : %.2f\n", solde);
                    }
                }
            }
        } else {
            // Copier toutes les autres lignes
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    if (compte_trouve && solde_suffisant) {
        // Remplacer l'ancien fichier par le fichier temporaire
        remove(Clientfile);
        rename("temp_client.txt", Clientfile);
       printFormattedGreenOnWhite("\n\n\n\t\t\t\tRecharge effectuee avec succes pour le numero %s !\n", numero_telephone);

         FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(historique, "Vous avez effectuer une recharge de montant : %.2f DH \n\n", montant);

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }
    } else {
        printRedOnWhite("\t\t\t\t\t Recharge echouee.\n");
        if (!compte_trouve) {
        printRedOnWhite("\t\t\t\t\t Compte introuvable.\n");
        }
        if (compte_trouve && !solde_suffisant) {
            printRedOnWhite("\t\t\t\tSolde insuffisant pour effectuer la recharge.\n");
        }
        remove("temp_client.txt"); // Supprimer le fichier temporaire inutilisé
    }
}

void payerVignette(char rib_entre[25], float montant) {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier client.\n");
        return;
    }

    FILE *temp = fopen("temp_client.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de creer le fichier temporaire.\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_trouve = false;
    bool solde_suffisant = false;

    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_lu[25];

        // Vérifier si la ligne contient un RIB
        if (sscanf(ligne, "RIB : %s", rib_lu) == 1) {
            fprintf(temp, "%s", ligne); // Copier la ligne du RIB dans le fichier temporaire

            if (strcmp(rib_lu, rib_entre) == 0) {
                // Compte trouvé
                compte_trouve = true;
                int i;
                for (i = 0; i < 9; i++) {
                    fgets(ligne, sizeof(ligne), f);
                    fprintf(temp, "%s", ligne); // Copier ces lignes
                }

                // Lire et modifier la ligne du solde
                if (fgets(ligne, sizeof(ligne), f)) {
                    float solde;
                    if (sscanf(ligne, "Solde initial : %f", &solde) == 1) {
                        if (solde >= montant) {
                            solde -= montant;
                            solde_suffisant = true;
                        }
                        fprintf(temp, "Solde initial : %.2f\n", solde);
                    }
                }
            }
        } else {
            // Copier toutes les autres lignes
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    if (compte_trouve && solde_suffisant) {
        // Remplacer l'ancien fichier par le fichier temporaire
        remove(Clientfile);
        rename("temp_client.txt", Clientfile);
        printGreenOnWhite("\t\t\t\tPaiement de la vignette effectue avec succes. Solde mis a jour.\n");


        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(historique, "Vous avez paye une vignette de montant : %.2f DH \n\n", montant);

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }
    } else {
        printRedOnWhite("\t\t\t\t\t Paiement echoue.\n");
        if (!compte_trouve) {
            printRedOnWhite("\t\t\t\t\t Compte introuvable.\n");
        }
        if (compte_trouve && !solde_suffisant) {
            printRedOnWhite("\t\t\t\t Solde insuffisant pour effectuer le paiement.\n");
        }
        remove("temp_client.txt"); // Supprimer le fichier temporaire inutilisé
    }
}

void remplirDemande_creation() {
    Demande demande;
    int dernier_demande = 0;
    FILE *f = fopen(demande_creationfile, "r");
    if (f != NULL) {
        char ligne[200];
        while (fgets(ligne, sizeof(ligne), f)) {
            if (sscanf(ligne,"ID demande : %d\n" , &dernier_demande) == 1) {
                // Trouver le dernier numero de demande
            }
        }
        fclose(f);
    }

    demande.id_demande = dernier_demande + 1;
    // Saisie des informations
    printf("\t\t\t\t\t Entrez le nom : ");
    scanf("%s", demande.Client.nom);

    printf("\t\t\t\t\t Entrez le prenom : ");
    scanf("%s", demande.Client.prenom);

    printf("\t\t\t\t\t Entrez le CIN : ");
    scanf("%s", demande.Client.CIN);

    printf("\t\t\t\t\t Entrez le type de compte : ");
    scanf("%s", demande.Client.type_Compte);

    printf("\t\t\t\t\t Entrez l'email : ");
    scanf("%s", demande.Client.email);

    printf("\t\t\t\t\t Entrez le telephone : ");
    scanf("%s", demande.Client.tele);

    printf("\t\t\t\t\t Date de naissance :\n");
    printf("\t\t\t\t\t jour : ");
    scanf("%d",&demande.Client.date_naissance.jour);
    printf("\t\t\t\t\t mois : ");
    scanf("%d",&demande.Client.date_naissance.mois);
    printf("\t\t\t\t\t annee : ");
    scanf("%d",&demande.Client.date_naissance.annee);

    printf("\t\t\t\t\t Entrer le mot de passe de votre compte:\n");
    printf("\t\t\t\t\t ");
	 scanf("%s",demande.Client.password);

    demande.valide = false; // La demande n'est pas encore validée

    // Ouverture du fichier pour ajouter la demande
    FILE *file = fopen(demande_creationfile, "a");
    if (file != NULL) {
        fprintf(file, "ID demande : %d\n", demande.id_demande);
        fprintf(file, "Nom : %s\n", demande.Client.nom);
        fprintf(file, "Prenom : %s\n", demande.Client.prenom);
        fprintf(file, "CIN : %s\n", demande.Client.CIN);
        fprintf(file, "Type de compte : %s\n", demande.Client.type_Compte);
        fprintf(file, "Email : %s\n", demande.Client.email);
        fprintf(file, "Telephone : %s\n", demande.Client.tele);
        fprintf(file, "Date de naissance : %02d/%02d/%04d\n",demande.Client.date_naissance.jour,
                demande.Client.date_naissance.mois,
                demande.Client.date_naissance.annee );
        fprintf(file, "Mot de passe : %s\n", demande.Client.password);
           // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
        fprintf(file, "Date de demande : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
        fprintf(file, "-----------------------------\n");
        fclose(file);
        printGreenOnWhite("\t\t\t\t\t Demande de creation enregistree avec succes \n");
        Sleep(3000);//Attendre pour que le message apparait et soit lu par le client
        clearConsole();

    } else {
        printf("Erreur lors de l'ouverture du fichier %s.\n", demande_creationfile);
    }
}


void remplirDemande_suppression(char rib_entre[25]) {
    Demande demande;
    Compte c;
    int dernier_demande = 0;
    FILE *f = fopen(demande_suppressionfile, "r");
    if (f != NULL) {
        char ligne[200];
        while (fgets(ligne, sizeof(ligne), f)) {
            if (sscanf(ligne,"ID demande : %d\n" , &dernier_demande) == 1) {
                // Trouver le dernier numero de demande
            }
        }
        fclose(f);
    }

    demande.id_demande = dernier_demande + 1;
    // Saisie des informations
    printf("\t\t\t\t\t Entrez votre RIB : ");
    scanf("%s",c.RIB);

    printf("\t\t\t\t\t Entrez votre nom : ");
    scanf("%s", demande.Client.nom);

    printf("\t\t\t\t\t Entrez votre prenom : ");
    scanf("%s", demande.Client.prenom);

    printf("\t\t\t\t\t Entrez votre CIN : ");
    scanf("%s", demande.Client.CIN);

    printf("\t\t\t\t\t Entrez le type de compte (par ex. 'Epargne', 'Courant') : ");
    scanf("%s", demande.Client.type_Compte);


    // Ouverture du fichier pour ajouter la demande
    FILE *file = fopen(demande_suppressionfile, "a");
    if (file != NULL) {
        fprintf(file, "ID demande : %d\n", demande.id_demande);
        fprintf(file, "RIB : %s\n", c.RIB);
        fprintf(file, "Nom : %s\n", demande.Client.nom);
        fprintf(file, "Prénom : %s\n", demande.Client.prenom);
        fprintf(file, "CIN : %s\n", demande.Client.CIN);
        fprintf(file, "Type de compte : %s\n", demande.Client.type_Compte);
        // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
        fprintf(file, "Date de demande : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
        fprintf(file, "-----------------------------\n");
        fclose(file);
        printGreenOnWhite("\t\t\t\tDemande de suppression enregistree avec succes !\n");
        Sleep(3000);//Attendre pour que le message apparait et soit lu par le client
        clearConsole();
        // Écrire les informations dans l'historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date :%02d/%02d/%04d %02d:%02d:%02d\n ",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec
                ); // Ligne 2 : Date
            fprintf(historique, "Vous avez effectuer une demande de suppression du compte . \n\n");

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }
    }else {
        printf("Erreur lors de l'ouverture du fichier %s.\n", demande_suppressionfile);
    }
}

void remplirDemande_recuperation() {
    Demande demande;
    Compte c;
    int dernier_demande = 0;
    FILE *f = fopen(demande_recuperationfile, "r");
    if (f != NULL) {
        char ligne[200];
        while (fgets(ligne, sizeof(ligne), f)) {
            if (sscanf(ligne,"ID demande : %d\n" , &dernier_demande) == 1) {
                // Trouver le dernier numero de demande
            }
        }
        fclose(f);
    }
    demande.id_demande = dernier_demande + 1;
    // Saisie des informations
    printf("\t\t\t\t\t  Entrez votre RIB : ");
    scanf("%s", c.RIB);

    printf("\t\t\t\t\t  Entrez votre email : ");
    scanf("%s", demande.Client.email);

    printf("\t\t\t\t\t  Entrez votre numero de telephone : ");
    scanf("%s", demande.Client.tele);

    // Ouverture du fichier pour ajouter la demande
    FILE *file = fopen(demande_recuperationfile, "a");
    if (file != NULL) {
        fprintf(file, "ID demande : %d\n", demande.id_demande);
        fprintf(file, "RIB : %s\n", c.RIB);
        fprintf(file, "Email : %s\n", demande.Client.email);
        fprintf(file, "Téléphone : %s\n", demande.Client.tele);
        // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
        fprintf(file, "Date de demande : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
        fprintf(file, "-----------------------------\n");
        fclose(file);
        printGreenOnWhite("\t\t\t\tDemande de recuperation enregistree avec succes !\n");
        printf("\t\t\t\tUn mot de passe de recuperation sera envoye par e-mail .\n");
        printf("\t\t\t\t veuillez consulter votre email.\n");
        Sleep(8000);//Attendre pour que le message apparait et soit lu par le client
        clearConsole();

    }else {
        printf("Erreur lors de l'ouverture du fichier %s.\n", demande_recuperationfile);
    }
}


void remplirDemande_modification(char rib_entre[25]) {
    Demande demande;
    Compte c;
    int dernier_demande = 0;
    FILE *f = fopen(demande_modification, "r");
    if (f != NULL) {
        char ligne[200];
        while (fgets(ligne, sizeof(ligne), f)) {
            if (sscanf(ligne,"ID demande : %d\n" , &dernier_demande) == 1) {
                // Trouver le dernier numero de demande
            }
        }
        fclose(f);
    }

    demande.id_demande = dernier_demande + 1;
    // Saisie des informations
    printf("\t\t\t\t\t veuillez ecrire en miniscule\n ");
    printf("\t\t\t\t\t Entrez votre RIB : ");
    scanf("%s", c.RIB);
    printf("\t\t\t\t\t Entrez votre nom : ");
    scanf("%s", demande.Client.nom);

    printf("\t\t\t\t\t Entrez votre prenom : ");
    scanf("%s", demande.Client.prenom);

    char info_a_modifier[50];
    printf("\t\t\t\t\t Entrez l'information a modifier : ");
    scanf(" %s", info_a_modifier);
    printf("\t\t\t\t\t Entrez la nouvelle information : ");
    scanf(" %s", demande.nouvelles_infos);
// Ouverture du fichier pour ajouter la demande
    FILE *file = fopen(demande_modification, "a");
    if (file != NULL) {
        fprintf(file, "ID demande : %d\n", demande.id_demande);
        fprintf(file, "RIB : %s\n", c.RIB);
    // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

        fprintf(file, "Information a modifier : %s\n", info_a_modifier);
        fprintf(file, "Nouvelle information : %s\n", demande.nouvelles_infos);
        fprintf(file, "Date de demande : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
        fprintf(file, "-----------------------------\n");
        fclose(file);
       printGreenOnWhite("\t\t\t\t\t Demande de modification enregistree avec succes! ");
       Sleep(5000);
       clearConsole();
        // Écrire les informations dans l'historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
             // Ligne 2 : Date
            fprintf(historique, "Vous avez effectuer une demande de modification de %s par %s . \n\n",info_a_modifier,demande.nouvelles_infos);

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }

    } else {
        printf("Erreur lors de l'ouverture du fichier %s.\n", demande_modification);
    }
}

void remplirDemande_carnet(char rib_entre[25]) {
    Demande demande;
    Compte c;
    int dernier_demande = 0;
    FILE *f = fopen(demande_carnet_cheque, "r");
    if (f != NULL) {
        char ligne[200];
        while (fgets(ligne, sizeof(ligne), f)) {
            if (sscanf(ligne,"ID demande : %d\n" , &dernier_demande) == 1) {
                // Trouver le dernier numero de demande
            }
        }
        fclose(f);
    }

    demande.id_demande = dernier_demande + 1;
    // Saisie des informations nécessaires
    printf("\t\t\t\t\t Entrez votre RIB : ");
    scanf("%s",c.RIB);  // Stockage du RIB dans Client.RIB

    int nombre_feuilles;
    printf("\t\t\t\t\t Entrez le nombre de feuilles par carnet : ");
    scanf("%d", &nombre_feuilles);

    int quantite;
    printf("\t\t\t\t\t Entrez la quantite de carnets souhaitee : ");
    scanf("%d", &quantite);

    // Ouverture du fichier pour ajouter la demande
    FILE *file = fopen(demande_carnet_cheque, "a");
    if (file != NULL) {
        fprintf(file, "ID demande : %d\n", demande.id_demande);
        fprintf(file, "RIB : %s\n",c.RIB);
         // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
        fprintf(file, "Date de demande : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
        fprintf(file, "Nombre de feuilles par carnet : %d\n", nombre_feuilles);
        fprintf(file, "Quantite de carnets : %d\n", quantite);
        fprintf(file, "-----------------------------\n");
        fclose(file);
        printGreenOnWhite("\t\t\t\tDemande de carnet de cheques enregistree avec succes !\n");
        Sleep(3000);//Attendre pour que le message apparait et soit lu par le client
        clearConsole();
        // Écrire les informations dans l'historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec);
                // Ligne 2 : Date
            fprintf(historique, "Vous avez effectuer une demande de %d carnet(s) de cheque avec %d feuilles . \n\n",quantite,nombre_feuilles);

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }

    } else {
        printf("Erreur lors de l'ouverture du fichier %s.\n", demande_carnet_cheque);
    }
}


void Menu_princ(char rib_entre[25]){
    changerCouleur(MAGENTA);
    int choix;
    do {
    afficherCentre("********************************************************\n");
    afficherCentre("*           VOICI VOTRE MENU :                         *\n");
    changerCouleur(BLACK);
    afficherCentre("*          [1]- AFFICHAGE D'INFORMATIONS               *\n");
    afficherCentre("*          [2]- EFFECTUER UN DEPOT                     *\n");
    afficherCentre("*          [3]- EFFECTUER UN RETRAIT                   *\n");
    afficherCentre("*          [4]- EFFECTUER UN VIREMENT                  *\n");
    afficherCentre("*          [5]- PAYER FACTURE                          *\n");
    afficherCentre("*          [6]- PAYER VIGNETTE                         *\n");
    afficherCentre("*          [7]- FAIRE UNE DEMANDE                      *\n");
	afficherCentre("*          [8]- EFFECTUER UNE RECHARGE                 *\n");
	afficherCentre("*          [9]- AFFICHAGE D'HISTORIQUE                 *\n");
	afficherCentre("*          [10]- BOITE MESSAGE                         *\n");
	afficherCentre("*          [0]- REVENIR AU MENU PRINCIPAL              *\n");
	changerCouleur(MAGENTA);
    afficherCentre("********************************************************\n");
    printf("\t\t\t\t\t ENTREZ VOTRE CHOIX : ");
    scanf("%d",&choix);
    changerCouleur(BLACK);
    clearConsole();
        switch (choix) {
            case 1:
                afficherInformations(rib_entre);
afficherCentre("appuyer sur entree pour revenir au menu:\n");
                 getchar();//consomme l'entree \n apres  scanf("%d",&choix);
                 getchar();//Attend une entrée utilisateur
                 clearConsole();
                break;
            case 2:{
                float montant;
                printf("\t\t\t\t\t Montant a deposer : ");
                scanf("%f", &montant);
                effectuerDepot(rib_entre,montant);
                Sleep(5000);
                 clearConsole();
                break;}
            case 3:{
                float montant;
                printf("\t\t\t\t\t Montant a retirer : ");
                scanf("%f", &montant);
                effectuerRetrait(rib_entre,montant);
                 Sleep(5000);
                 clearConsole();
                break;}
            case 4:{
            	float montant;
            	char rib_destination[25];
                printf("\t\t\t\t\t Numero du RIB du compte destination : ");
                scanf("%s",rib_destination);
                printf("\t\t\t\t\t Montant a virer : ");
                scanf("%f", &montant);
                effectuerVirement(rib_entre,rib_destination,montant);
                 Sleep(5000);
                 clearConsole();
                break;}
            case 5:{int type_facture;
                    float montant;
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

                                printf("\t\t\tPaiement de %.2fDH a la societe '%s' en cours...\n", montant, societe);/////////////////////////////////////////sleep???
                                 Sleep(9000); //attendre 9 secondes
                                 payerFacture(rib_entre, montant);
                                 Sleep(3000);//Attendre pour que le message apparait
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
                                  payerFacture(rib_entre, montant);
                                  Sleep(3000);//Attendre pour que le message apparait
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
                                 payerFacture(rib_entre, montant);
                                 Sleep(3000);//Attendre pour que le message apparait
                                 clearConsole();
                                  break;
                                }
                            default:
                               printRedOnWhite("\t\t\t\t\t Type de facture invalide.\n");
                               Sleep(3000);//Attendre pour que le message apparait
                               clearConsole();
                               }
                       break;
			    }
			    case 6: {
                        int type_carte;
                        char matricule[20];
                        int puissance_fiscale;
                        int carburant;
                        char email[50];
                        char numero_telephone[15];
                        int annee_vignette;
                        float montant;
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
                            break;
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
                       break;
                              }
                       printf("\t\t\t\t\t Entrez votre email : ");// Email
                        scanf("%s", email);
                       printf("\t\t\t\t\t Entrez votre numero de telephone : ");
                       scanf("%s", numero_telephone);
                // Année de vignette
                       printf("\t\t\t\t\t Entrez l'annee de la vignette : ");
                      scanf("%d", &annee_vignette);
                   // Montant
                       printf("\t\t\t\t\t Entrez le montant a payer : ");
                       scanf("%f", &montant);
                  // Appeler la fonction pour payer la vignette
                       payerVignette(rib_entre, montant);
                       Sleep(3000);//Attendre pour que le message apparait
                       clearConsole();
                        break;
                      }

				 case 7:{
				 int type_demande;
				 afficherCentre("\n\n\n\n");
				 afficherCentre("********************************************************\n");
                 afficherCentre("*                                                      *\n");
                 afficherCentre("*               TYPE DE DEMANDE :                      *\n\n");
                 afficherCentre("*            [1] - SUPPRESSION DU COMPTE               *\n");
                 afficherCentre("*            [2] - MODIFICATION D'INFORMATIONS         *\n");
                 afficherCentre("*            [3] - DEMANDE DE CARNET DE CHEQUE         *\n");
                 afficherCentre("*                                                      *\n");
                 afficherCentre("********************************************************");
                 printf("\t\t\t\t\t CHOISISSEZ VOTRE DEMMANDE : ");
                 scanf("%d", &type_demande);
                 clearConsole();

                switch (type_demande) {
                          case 1:
                          	   remplirDemande_suppression(rib_entre);
                          	    break;
                          case 2:
                          	    remplirDemande_modification(rib_entre);
                          	    break;
                          case 3:
                                remplirDemande_carnet(rib_entre);
                                break;

                         default:
                printRedOnWhite("\t\t\t\t\t Type de demande invalide.\n");
                       }

				  break;
			        }

			        case 8: {
                         int operateur;
                         char numero_telephone[15];
                         int typerecharge;
                         float montant;

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
                         break;
                           }

                       // Demander le numéro de téléphone
                         printf("\t\t\t\t\t Entrez le numero de telephone : ");
                         scanf("%s", numero_telephone);

                      // Demander le montant de recharge
                         afficherCentre("10DH/20DH/25DH/30DH/50DH/100DH/200DH/300DH/500DH/1000DH/\n");
                         printf("\t\t\t\t\t Entrez le montant de recharge : ");
                         scanf("%f", &montant);

                     // Valider le montant de recharge
                    if (montant != 10 && montant != 20 && montant != 25 && montant != 30 &&
                    montant != 50 && montant != 100 && montant != 200 && montant != 300 &&
                    montant != 500 && montant != 1000) {
                    printRedOnWhite("\t\t\t\tErreur : montant invalide ! Veuillez entrer un montant parmi les choix proposes.\n");
                    break;
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
            Sleep(3000);//Attendre pour que le message apparait
                   clearConsole();
                   break;
                   }

                     // Appeler la fonction pour effectuer la recharge
                   effectuerRecharge(rib_entre, montant,numero_telephone);

                   Sleep(3000);//Attendre pour que le message apparait
                   clearConsole();
                   break;
                }
			 case 9:{
			     changerCouleur(MAGENTA);

                afficherhistoriqueclient(rib_entre);
                 afficherCentre("--------------------------------\n");
			 	 afficherCentre("appuyer sur entree pour revenir au menu:\n");
			 	 changerCouleur(BLACK);
                 getchar();//consomme l'entree \n apres  scanf("%d",&choix);
                 getchar();//Attend une entrée utilisateur
                 clearConsole();
				break;
			 }
			  case 10:{
                changerCouleur(MAGENTA);
			    afficherboitemessage(rib_entre);
			     afficherCentre("--------------------------------\n");
			 	 afficherCentre("appuyer sur entree pour revenir au menu:\n");
			 	 changerCouleur(BLACK);
                 getchar();//consomme l'entree \n apres  scanf("%d",&choix);
                 getchar();//Attend une entrée utilisateur
                 clearConsole();
				break;
			  }
          case 0:{
              changerCouleur(PINK);
          	printf("\n\n");
          	afficherCentre("---------_ Diriger vers le menu principal  en cours...--------_-\n");
          	Sleep(3000);//Attendre pour que le message apparait
            clearConsole();
            changerCouleur(BLACK);
			break;
		  }
            default:
                printRedOnWhite("\t\t\t\t\t Choix invalide!\n");
        }
    } while (choix != 0);
}

void login(char RIB[25], char password[20]) {
    char rib_entre[25];
    char motdepass[20];
    bool compte_trouve = false;
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
        printf("\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
       printf("\t\t\t\t\t\t|\t mot de passe           |\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        printf("\t\t\t\t\t\t|\t  ");
        read_password( motdepass);//fonction pour masquer le mot de passe
        printf("\t\t\t\t\t\t+-------------------------------+\n");
        FILE *l = fopen(LoginClientfile, "r");
        if (l == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier %s\n", Clientfile);
            return;
        }

        char rib_fichier[25];
        char motdepass_fichier[20];

        while (fscanf(l, "%s %s", rib_fichier, motdepass_fichier) == 2) {
    // Comparer le RIB et le mot de passe
    if (strcmp(rib_entre, rib_fichier) == 0 && strcmp(motdepass, motdepass_fichier) == 0) {
        compte_trouve = true;

        // Connexion réussie, chercher le nom dans Clientfile
        FILE *f = fopen(Clientfile, "r");
        if (f == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier %s\n", Clientfile);
            fclose(l);
            return;
        }

        char ligne[200];
        char nom_client[50];
        bool nom_trouve = false;

        while (fgets(ligne, sizeof(ligne), f)) {
            char rib_temp[25];
            if (sscanf(ligne, "RIB : %s", rib_temp) == 1 && strcmp(rib_temp, rib_entre) == 0) {
                // Lire le nom du client
                fgets(ligne, sizeof(ligne), f); // Ignorer la ligne suivante (mot de passe)
                fgets(ligne, sizeof(ligne), f); // Ignorer la ligne suivante (numéro de compte)
                fgets(ligne, sizeof(ligne), f); // Lire la ligne contenant le nom
                sscanf(ligne, "Nom : %s", nom_client);

                nom_trouve = true;
                break;
            }
        }

        fclose(f);

        if (nom_trouve) {
                getchar();
                clearConsole();
          printGreenOnWhite("\n\n\n\n\t\t\t\t\t CONNEXION REUSSIE DANS SAR BANQUE !! ");
            // Enregistrer l'opération dans le fichier historique
        FILE *historique = fopen(historique_clientfile, "a");
        if (historique != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'historique
            fprintf(historique, "RIB : %s\n", rib_entre); // Ligne 1 : RIB
            fprintf(historique, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(historique, "Vous vous etes authentifie a votre compte. \n\n"); // Ligne 3 : Message personnalisé

            fclose(historique);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        }
            Sleep(2600); // Attendre pour afficher la connexion réussie
            clearConsole();
            changerCouleur(MAGENTA);
       printf("\n\n\t\t\t\t\t*^*BIENVENUE DANS VOTRE COMPTE, %s !*^*\n", nom_client);
       changerCouleur(BLACK);
        } else {
            printRedOnWhite("\t\t\t\t\t Erreur : Impossible de trouver le nom du client.\n");
        }


        Menu_princ(rib_entre); // Diriger vers le menu principal
        return; // Quitter la fonction après connexion réussie
    }
  }
        fclose(l);

        if (!compte_trouve) {
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
                	remplirDemande_recuperation();//demande pour envoyer un mot de passe de recuperation par email
                    break;
                }
            } else {
               printRedOnWhite("\t\t\t\t\t Trois tentatives echouees. Veuillez essayer plus tard.\n");
                Sleep(7000); // Attendre 5 secondes
                clearConsole();

                 return;
            }
        }
    } while (tentative < 3 && !compte_trouve);
}
//ADMIN
/*
void envoyerEmail(const char *email, const char *message) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        struct curl_slist *recipients = NULL;

        // Configuration de l'email
        curl_easy_setopt(curl, CURLOPT_USERNAME, "votre_email@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "votre_mot_de_passe");
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.gmail.com:587");

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "<votre_email@gmail.com>");

        // Destinataires
        recipients = curl_slist_append(recipients, email);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Contenu du message
        char data[500];
        snprintf(data, sizeof(data), "To: %s\r\nSubject: Récupération de mot de passe\r\n\r\n%s\r\n", email, message);
        curl_easy_setopt(curl, CURLOPT_READDATA, data);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        // Envoi
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Erreur lors de l'envoi de l'email : %s\n", curl_easy_strerror(res));
        } else {
            printf("Email envoyé avec succès.\n");
        }

        // Libération des ressources
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    } else {
        printf("Erreur : Impossible d'initialiser libcurl.\n");
    }
}
*/
// Liste des administrateurs
Admin admins[MAX_ADMINS];
int nb_admins = 0;

// Liste des demandes
Demande demandes[MAX_DEMANDES];
int nb_demandes = 0;
void afficherStatistiques(char rib_entre[25]) {
    FILE *historique = fopen(historique_clientfile, "r");
    if (historique == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        return;
    }

    float total_depots = 0, total_retraits = 0, total_virements = 0;
    int nombre_transactions = 0;
    char ligne[200];
    bool compte_trouve = false;
    printf("\n");
    afficherCentreSF("========== Statistiques pour le compte : %s ==========\n", rib_entre);

    while (fgets(ligne, sizeof(ligne), historique)) {
        char rib_historique[25];
        if (sscanf(ligne, "RIB : %s", rib_historique) == 1 && strcmp(rib_historique, rib_entre) == 0) {
            compte_trouve = true;
            // Lire les détails des transactions
            while (fgets(ligne, sizeof(ligne), historique)) {
                if (strstr(ligne, "depot")) {
                    float montant;
                    if (sscanf(ligne, "Vous avez effectue un depot de : %f DH", &montant) == 1) {
                        total_depots += montant;
                        nombre_transactions++;
                    }
                } else if (strstr(ligne, "retrait")) {
                    float montant;
                    if (sscanf(ligne, "Vous avez effectue un retrait de : %f DH", &montant) == 1) {
                        total_retraits += montant;
                        nombre_transactions++;
                    }
                } else if (strstr(ligne, "virement")) {
                    float montant;
                    if (sscanf(ligne, "Vous avez effectue un virement de : %f DH", &montant) == 1) {
                        total_virements += montant;
                        nombre_transactions++;
                    }
                } else if (strstr(ligne, "-----------------------------")) {
                    break; // Fin des transactions pour ce compte
                }
            }
        }
    }

    fclose(historique);

    if (!compte_trouve) {
        afficherCentre("Aucun historique trouve pour ce compte.\n");
        return;
    }

    // Afficher les statistiques
    printf("\t\t\t\t\t Total des depots : %.2f DH\n", total_depots);
    printf("\t\t\t\t\t Total des retraits : %.2f DH\n", total_retraits);
    printf("\t\t\t\t\t Total des virements : %.2f DH\n", total_virements);
    printf("\t\t\t\t\t Nombre total de transactions : %d\n", nombre_transactions);

    if (nombre_transactions > 0) {
        float moyenne_transaction = (total_depots + total_retraits + total_virements) / nombre_transactions;
    printf("\t\t\t\t\t Montant moyen par transaction : %.2f DH\n", moyenne_transaction);
    }
    afficherCentre("======================================================\n");
}
int extractMonth(const char *date) {
    int day, month, year;
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) == 3) {
        return month;
    }
    return -1; // Invalid date

}

// statistiques
void statistiquesComptes() {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", Clientfile);
        return;
    }

    // Déclaration des variables pour le fichier des clients
    int total_comptes = 0;
    float total_solde = 0.0, solde_max = -1.0, solde_min = 1e9;
    char compte_max[25], compte_min[25];
    char type_comptes[10][20];
    int compte_type_count[10] = {0};
    int type_count = 0;
    char ligne[200], rib_cc[25];

    // Lecture des comptes clients
    while (fgets(ligne, sizeof(ligne), f)) {
        if (strncmp(ligne, "RIB :", 5) == 0) {
            sscanf(ligne, "RIB : %s", rib_cc);
            total_comptes++;
        }
        else if (strncmp(ligne, "Solde initial :", 15) == 0) {
            float solde;
            sscanf(ligne, "Solde initial : %f", &solde);
            total_solde += solde;

            if (solde > solde_max) {
                solde_max = solde;
                strcpy(compte_max, rib_cc);
            }
            if (solde < solde_min) {
                solde_min = solde;
                strcpy(compte_min, rib_cc);
            }
        }
        else if (strncmp(ligne, "Type de compte :", 15) == 0) {
            char type[20];
            sscanf(ligne, "Type de compte : %s", type);

            // Recherche si le type existe déjà
            int found = 0;
            for (int i = 0; i < type_count; i++) {
                if (strcmp(type_comptes[i], type) == 0) {
                    compte_type_count[i]++;
                    found = 1;
                    break;
                }
            }
            // Nouveau type de compte
            if (!found) {
                strcpy(type_comptes[type_count], type);
                compte_type_count[type_count]++;
                type_count++;
            }
        }
    }
    fclose(f);

    // Déclaration des variables pour le fichier historique
    FILE *historique = fopen(historique_clientfile, "r");
    if (historique == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier historique.\n");
        return;
    }

    float total_depots = 0, total_retraits = 0, total_virements = 0;
    int authentifier = 0, demande = 0, nombre_transactions = 0;
    int monthly_totals[12] = {0}; // Transactions par mois

    // Lecture du fichier historique
    while (fgets(ligne, sizeof(ligne), historique)) {
        char date[20];

        if (strstr(ligne, "Date :")) {
            sscanf(ligne, "Date : %s", date);
            int month = extractMonth(date);
            if (month >= 1 && month <= 12) {
                monthly_totals[month - 1]++;
            }
        }

        if (strstr(ligne, "depot")) {
            float montant;
            if (sscanf(ligne, "Vous avez effectue un depot de : %f DH", &montant) == 1) {
                total_depots += montant;
                nombre_transactions++;
            }
        }
        else if (strstr(ligne, "retrait")) {
            float montant;
            if (sscanf(ligne, "Vous avez effectue un retrait de : %f DH", &montant) == 1) {
                total_retraits += montant;
                nombre_transactions++;
            }
        }
        else if (strstr(ligne, "virement")) {
            float montant;
            if (sscanf(ligne, "Vous avez effectue un virement de : %f DH", &montant) == 1) {
                total_virements += montant;
                nombre_transactions++;
            }
        }
        else if (strstr(ligne, "authentifie")) {
            authentifier++;
        }
        else if (strstr(ligne, "demande")) {
            demande++;
        }
    }
    fclose(historique);

    // Calcul des statistiques
    float solde_moyen = total_comptes > 0 ? total_solde / total_comptes : 0;

    // Création des fichiers pour Gnuplot
    FILE *file_monthly = fopen("transactions_mensuelles.txt", "w");
    for (int i = 0; i < 12; i++) {
        fprintf(file_monthly, "%d %d\n", i + 1, monthly_totals[i]);
    }
    fclose(file_monthly);

    FILE *file_types = fopen("types_comptes.txt", "w");
    for (int i = 0; i < type_count; i++) {
        fprintf(file_types, "%s %d\n", type_comptes[i], compte_type_count[i]);
    }
    fclose(file_types);

    // Affichage des statistiques
    printf("\n");
    afficherCentre("=========================================== Statistiques ===========================================\n");
    printf("\t\t\t\t\t Nombre total de comptes : %d\n", total_comptes);
    printf("\t\t\t\t\t Solde total : %.2f MAD\n", total_solde);
    printf("\t\t\t\t\t Solde moyen : %.2f MAD\n", solde_moyen);
    printf("\t\t\t\t\t Total des depots : %.2f DH\n", total_depots);
    printf("\t\t\t\t\t Total des retraits : %.2f DH\n", total_retraits);
    printf("\t\t\t\t\t Total des virements : %.2f DH\n", total_virements);
    printf("\t\t\t\t\t Nombre total de demandes : %d\n", demande);
    printf("\t\t\t\t\t Nombre total d'authentifications : %d\n", authentifier);

    if (total_comptes > 0) {
    printf("\t\t\t\t\t Compte avec le solde maximum : %.2f MAD (RIB : %s)\n", solde_max, compte_max);
    printf("\t\t\t\t\t Compte avec le solde minimum : %.2f MAD (RIB : %s)\n", solde_min, compte_min);
    }

    printf("\n\t\t\t\t\t Repartition des types de comptes :\n");
    for (int i = 0; i < type_count; i++) {
        printf("\t\t\t\t\t- %s : %d comptes\n", type_comptes[i], compte_type_count[i]);
    }

    printf("\n\t\t\t\t\t Transactions mensuelles :\n");
    for (int i = 0; i < 12; i++) {
        printf("\t\t\t\t\t Mois %d : %d transactions\n", i + 1, monthly_totals[i]);
    }

    afficherCentre("====================================================================================================\n");
}

void genererGraphiques() {
    // Création d'un script Gnuplot
    FILE *gnuplotScript = fopen("statistiques.plt", "w");
    if (gnuplotScript == NULL) {
        printf("Erreur : Impossible de créer le fichier Gnuplot.\n");
        return;
    }

    // Script pour le graphique des transactions mensuelles
    fprintf(gnuplotScript, "set terminal png size 800,600\n");
    fprintf(gnuplotScript, "set output 'transactions_mensuelles.png'\n");
    fprintf(gnuplotScript, "set title 'Transactions Mensuelles'\n");
    fprintf(gnuplotScript, "set xlabel 'Mois'\n");
    fprintf(gnuplotScript, "set ylabel 'Nombre de Transactions'\n");
    fprintf(gnuplotScript, "set xtics 1\n");
    fprintf(gnuplotScript, "set grid\n");
    fprintf(gnuplotScript, "plot 'transactions_mensuelles.txt' using 1:2 with linespoints title 'Transactions'\n");

    // Script pour le graphique de répartition des types de comptes
    fprintf(gnuplotScript, "set output 'types_comptes.png'\n");
    fprintf(gnuplotScript, "set title 'Répartition des Types de Comptes'\n");
    fprintf(gnuplotScript, "set xlabel 'Type de Compte'\n");
    fprintf(gnuplotScript, "set ylabel 'Nombre de Comptes'\n");
    fprintf(gnuplotScript, "set style data histogram\n");
    fprintf(gnuplotScript, "set style fill solid\n");
    fprintf(gnuplotScript, "set boxwidth 0.5\n");
    fprintf(gnuplotScript, "plot 'types_comptes.txt' using 2:xtic(1) title 'Types'\n");

    fclose(gnuplotScript);

    // Appel de Gnuplot pour exécuter le script
    printf("Generation des graphiques...\n");
    system("gnuplot statistiques.plt");
    printf("Graphiques generes : transactions_mensuelles.png et types_comptes.png\n");

    // Ouvrir automatiquement les graphiques
#ifdef _WIN32
    system("start transactions_mensuelles.png");
    system("start types_comptes.png");
#else
    system("xdg-open transactions_mensuelles.png");
    system("xdg-open types_comptes.png");
#endif
}
// Fonction pour afficher le menu principal de l'admin
void afficherMenuAdmin() {
    changerCouleur(MAGENTA);
    afficherCentre("\n");
    afficherCentre("***************************** MENU ADMIN *****************************\n");
    changerCouleur(BLACK);
    afficherCentre("*                 [1] CREER UN NOUVEAU COMPTE                        *\n");
    afficherCentre("*                 [2] MODIFIER UN COMPTE                             *\n");
    afficherCentre("*                 [3] SUPPRIMER UN COMPTE                            *\n");
    afficherCentre("*                 [4] AFFICHER LES INFORMATIONS D'UN COMPTE          *\n");
    afficherCentre("*                 [5] AFFICHER LES INFORMATIONS DE TOUS LES COMPTES  *\n");
    afficherCentre("*                 [6] AJOUTER UN ADMINISTRATEUR                      *\n");
    afficherCentre("*                 [7] GERER LES DEMANDES                             *\n");
    afficherCentre("*                 [8] STATISTIQUES DES CLIENTS                       *\n");
    afficherCentre("*                 [9] STATISTIQUES TOTALE                            *\n");
    afficherCentre("*                 [0] REVENIR AU MENU PRINCIPALE                     *\n");
    afficherCentre("*                                                                    *\n");
    changerCouleur(MAGENTA);
    afficherCentre("**********************************************************************\n");
    printf("\t\t\t\t\t ENTREZ VOTRE CHOIX : ");
    changerCouleur(BLACK);
}

// Fonction pour creer un compte par l'admin
void creerCompte() {
    Compte c;
    infoclient info;
    date d;

    // Lire le dernier numéro de compte
    int dernier_numero = 0;
    FILE *f = fopen(Clientfile, "r");
     FILE *l = fopen(LoginClientfile, "r");
    if (f != NULL) {
        char ligne[200];
        while (fgets(ligne, sizeof(ligne), f)) {
            if (sscanf(ligne, "Numero de compte : %d", &dernier_numero) == 1) {
                // Trouver le dernier numéro de compte
            }
        }
        fclose(f);
    }

    c.numero_compte = dernier_numero + 1;

    // Saisie des informations du client
    printf("\t\t\t\t\t  Nom : ");
    scanf("%s", info.nom);
    printf("\t\t\t\t\t  Prenom : ");
    scanf("%s", info.prenom);
    printf("\t\t\t\t\t  CIN : ");
    scanf("%s", info.CIN);
    printf("\t\t\t\t\t  RIB : ");
    scanf("%s", c.RIB);
    printf("\t\t\t\t\t  Numero de tel : ");
    scanf("%s", info.tele);
    printf("\t\t\t\t\t  Compte email : ");
    scanf("%s", info.email);
    printf("\t\t\t\t\t  Date de naissance :\n");
    printf("\t\t\t\t\t  Jour : ");
    scanf("%d", &d.jour);
    printf("\t\t\t\t\t  Mois : ");
    scanf("%d", &d.mois);
    printf("\t\t\t\t\t  Annee : ");
    scanf("%d", &d.annee);
    printf("\t\t\t\t\t  Type de compte : ");
    scanf("%s", info.type_Compte);
    printf("\t\t\t\t\t  Mot de passe : ");
    scanf("%s", info.password);
    c.solde = 0.0;
    c.Client = info;

    // Enregistrer le compte dans le fichier
    f = fopen(Clientfile, "a");
    if (f == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", Clientfile);
        return;
    }
     l = fopen(LoginClientfile, "a");
    if (l == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", Clientfile);
        return;
    }
    fprintf(f, "RIB : %s\n", c.RIB);
    fprintf(f, "Mot de passe : %s\n", info.password);
    fprintf(f, "Numero de compte : %d\n", c.numero_compte);
    fprintf(f, "Nom : %s\n", c.Client.nom);
    fprintf(f, "Prenom : %s\n", c.Client.prenom);
    fprintf(f, "CIN : %s\n", c.Client.CIN);
    fprintf(f, "Telephone : %s\n", c.Client.tele);
    fprintf(f, "Email : %s\n", c.Client.email);
    fprintf(f, "Date de naissance : %02d/%02d/%d\n", d.jour, d.mois, d.annee);
    fprintf(f, "Type de compte : %s\n", c.Client.type_Compte);
    fprintf(f, "Solde initial : %.2f\n", c.solde);
    fprintf(f, "---------------------------------------------\n");
    fclose(f);
    fprintf(l, "%s\t", c.RIB);
    fprintf(l, "%s\n", info.password);
    fclose(l);
    printFormattedGreenOnWhite("\t\t\t\tCompte cree avec succes, numero de compte : %d\n", c.numero_compte);
    Sleep(4000); // Attendre pour afficher la connexion réussie
    clearConsole();
}
// fonction de modification des cpmptes

void modifierCompte() {
    char rib[25];
    printf("\t\t\t\t Entrez le RIB du compte a modifier : ");
    scanf("%s", rib);

    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", Clientfile);
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de créer un fichier temporaire\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_trouve = false;

    // Variables pour les modifications
    char new_nom[50] = "";
    char new_prenom[50] = "";
    char new_tele[20] = "";
    char new_email[50] = "";
    bool modifier_nom = false, modifier_prenom = false, modifier_tele = false, modifier_email = false;

    // Parcourir le fichier ligne par ligne
    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_fichier[25];

        // Si on trouve le RIB du compte, commencer la modification
        if (sscanf(ligne, "RIB : %s", rib_fichier) == 1 && strcmp(rib, rib_fichier) == 0) {
            compte_trouve = true;
            fprintf(temp, "%s", ligne); // Écrire le RIB actuel

            int choix;
            do {
            changerCouleur(MAGENTA);
            afficherCentre("\n");
            afficherCentre("*================= Menu de Modificationn =================*\n");
            changerCouleur(BLACK);
            afficherCentre("*                                                         *\n");
            afficherCentre("*                 1. Modifier le nom                      *\n");
            afficherCentre("*                 2. Modifier le prenom                   *\n");
            afficherCentre("*                 3. Modifier le telephone                *\n");
            afficherCentre("*                 4. Modifier l'email                     *\n");
            afficherCentre("*                 5. Confirmer et terminer                *\n");
            afficherCentre("*                                                         *\n");
            changerCouleur(MAGENTA);
            afficherCentre("*=========================================================*\n");
            printf("\t\t\t\t\t Entrez votre choix : ");
               scanf("%d", &choix);
               clearConsole();
               changerCouleur(BLACK);
                switch (choix) {
                    case 1:
                        printf("\t\t\t\t Entrez le nouveau nom : ");
                        scanf("%s", new_nom);
                        modifier_nom = true;
                        break;
                    case 2:
                        printf("\t\t\t\t Entrez le nouveau prenom : ");
                        scanf("%s", new_prenom);
                        modifier_prenom = true;
                        break;
                    case 3:
                       printf("\t\t\t\t Entrez le nouveau telephone : ");
                        scanf("%s", new_tele);
                        modifier_tele = true;
                        break;
                    case 4:
                        printf("\t\t\t\t Entrez le nouvel email : ");
                        scanf("%s", new_email);
                        modifier_email = true;
                        break;
                    case 5:
             printGreenOnWhite("\t\t\t\t Modification terminee.\n");
                        break;

                    default:
             printRedOnWhite("\t\t\t\t   Choix invalide.\n");
                }
            } while (choix != 5);

        } else if (compte_trouve && modifier_nom && strncmp(ligne, "Nom : ", 6) == 0) {
            fprintf(temp, "Nom : %s\n", new_nom);
            modifier_nom = false;
        } else if (compte_trouve && modifier_prenom && strncmp(ligne, "Prenom : ", 9) == 0) {
            fprintf(temp, "Prenom : %s\n", new_prenom);
            modifier_prenom = false;
        } else if (compte_trouve && modifier_tele && strncmp(ligne, "Telephone : ", 12) == 0) {
            fprintf(temp, "Telephone : %s\n", new_tele);
            modifier_tele = false;
        } else if (compte_trouve && modifier_email && strncmp(ligne, "Email : ", 8) == 0) {
            fprintf(temp, "Email : %s\n", new_email);
            modifier_email = false;
        } else {
            // Copier toutes les autres lignes inchangées
            fprintf(temp, "%s", ligne);
        }
    }

    fclose(f);
    fclose(temp);

    // Si le compte a été trouvé et modifié, on remplace le fichier original
    if (compte_trouve) {
        remove(Clientfile);  // Supprimer l'ancien fichier
        rename("temp.txt", Clientfile);  // Renommer le fichier temporaire en fichier d'origine
    printGreenOnWhite("\t\t\t\t Modifications enregistrees avec succes !\n");
        Sleep(4000); // Attendre pour afficher la connexion réussie
        clearConsole();
    } else {
        remove("temp.txt");  // Supprimer le fichier temporaire si le compte n'a pas été trouvé
      printFormattedRedOnWhite("\t\t\t\t Compte avec le RIB %s non trouve.\n", rib);
       Sleep(3000); // Attendre pour afficher la connexion réussie
        clearConsole();
    }
}

// Fonction pour supprimer un compte

void supprimerCompte() {
    char rib[25];
    printf("\t\t\t\t\t Entrez le RIB du compte a supprimer : ");
    scanf("%s", rib);

    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", Clientfile);
        return;
    }

    char lignes[1000][200]; // Supposons qu'il y a au maximum 1000 lignes
    int nb_lignes = 0;
    bool compte_trouve = false;
    int lignes_a_sauter = 0;

    // Lire les lignes du fichier Clientfile
    while (fgets(lignes[nb_lignes], sizeof(lignes[nb_lignes]), f)) {
        if (lignes_a_sauter > 0) {
            lignes_a_sauter--;
            continue;
        }

        char rib_fichier[25];
        if (sscanf(lignes[nb_lignes], "RIB : %s", rib_fichier) == 1 && strcmp(rib, rib_fichier) == 0) {
            compte_trouve = true;
            lignes_a_sauter = 11; // Ignorer les 11 lignes suivantes
            continue;
        }

        nb_lignes++;
    }
    fclose(f);

    if (!compte_trouve) {
        printFormattedRedOnWhite("\t\t\t\t\t Aucun compte trouve avec le RIB \"%s\".\n", rib);
        return;
    }

    // Réécrire le fichier Clientfile avec renumérotation
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de creer un fichier temporaire\n");
        return;
    }

    int numero_compte = 1; // Recommencer la numérotation
    int i;
    for ( i = 0; i < nb_lignes; i++) {
        if (strncmp(lignes[i], "Numero de compte : ", 19) == 0) {
            fprintf(temp, "Numero de compte : %d\n", numero_compte++);
        } else {
            fputs(lignes[i], temp);
        }
    }

    fclose(temp);

    // Remplacer le fichier original par le fichier temporaire
    if (remove(Clientfile) != 0 || rename("temp.txt", Clientfile) != 0) {
        perror("Erreur lors de la mise a jour du fichier Clientfile");
    } else {
       printFormattedGreenOnWhite("\t\t\t\t\tLe compte avec le RIB \"%s\" a ete supprime avec succes!.\n", rib);
        Sleep(3000); // Attendre pour afficher le message
        clearConsole();
    }

    // Supprimer également le compte dans loginclientfile
    FILE *login_file = fopen(LoginClientfile, "r");
    if (login_file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", LoginClientfile);
        return;
    }

    FILE *login_temp = fopen("login_temp.txt", "w");
    if (login_temp == NULL) {
        printf("Erreur : Impossible de creer un fichier temporaire pour loginClientfile\n");
        fclose(login_file);
        return;
    }

    char login_ligne[200];
    while (fgets(login_ligne, sizeof(login_ligne), login_file)) {
        char rib_login[25];
        if (sscanf(login_ligne, "%s", rib_login) == 1 && strcmp(rib_login, rib) == 0) {
            // Ne pas écrire cette ligne dans le fichier temporaire
            continue;
        }
        fputs(login_ligne, login_temp);
    }

    fclose(login_file);
    fclose(login_temp);

    // Remplacer le fichier loginclientfile par le fichier temporaire
    if (remove(LoginClientfile) != 0 || rename("login_temp.txt", LoginClientfile) != 0) {
        perror("Erreur lors de la mise a jour du fichier loginclientfile");
    }
}
// fonction pour afficher toutes les infos de toutes les comptes
void afficherTousComptes() {
    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printRedOnWhite("\t\t\t\t\t Aucun compte a afficher.\n");
        return;
    }

    char ligne[200];
    afficherCentre("\n\t\t\t\t\t ===== Informations de tous les comptes =====\n");
    while (fgets(ligne, sizeof(ligne), f)) {
        afficherCentreSF("%s", ligne);
    }
    fclose(f);
}

void afficherInfoCompte() {
    char rib[25];
    printf("\t\t\t\t\t Entrez le RIB du compte a afficher : ");
    scanf("%s", rib);

    FILE *f = fopen(Clientfile, "r");
    if (f == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", Clientfile);
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        printf("Erreur : Impossible de creer un fichier temporaire\n");
        fclose(f);
        return;
    }

    char ligne[200];
    bool compte_trouve = false;

    // Parcourir le fichier ligne par ligne
    while (fgets(ligne, sizeof(ligne), f)) {
        char rib_fichier[25];

        // Si on trouve le RIB du compte, commencer la modification
        if (sscanf(ligne, "RIB : %s", rib_fichier) == 1 && strcmp(rib, rib_fichier) == 0) {
            compte_trouve = true;
            afficherCentreSF("%s", ligne);
            int i;
            for ( i = 0; i < 11; i++) {
                if (fgets(ligne, sizeof(ligne), f)) {
            afficherCentreSF("%s", ligne);
                }
            }
            break;
        }
    }

    fclose(f);

    if (!compte_trouve) {
    printFormattedRedOnWhite("\t\t\t\t\t Compte du RIB %d non trouve.\n", rib);
    }
}

// Fonction pour ajouter un administrateur
void ajouterAdmin() {
    if (nb_admins >= MAX_ADMINS) {
        printRedOnWhite("\t\t\t\t\t Limite d'administrateurs atteinte !\n");
        return;
    }

    Admin a;
    printf("\t\t\t\t\t Nom de l'administrateur : ");
    scanf("%s", a.nom);
    printf("\t\t\t\t\t Prenom de l'administrateur : ");
    scanf("%s", a.prenom);
    printf("\t\t\t\t\t Telephone de l'administrateur : ");
    scanf("%s", a.tel);
    printf("\t\t\t\t\t Mot de passe de l'administrateur : ");
    scanf("%s", a.password);

    FILE *f = fopen(adminfile, "a");
    if (f == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", adminfile);
        return;
    }

    FILE *g = fopen(LoginAdminfile, "a");
    if (g == NULL) {
        printf("Erreur d'ouverture du fichier %s\n", LoginAdminfile);
        return;
    }


    fprintf(f, "Nom : %s\n", a.nom);
    fprintf(f, "Prenom : %s\n", a.prenom);
    fprintf(f, "Telephone : %s\n", a.tel);
    fprintf(f, "Mot de passe : %s\n", a.password);
    fprintf(f, "---------------------------------------------\n");
    fclose(f);
    fprintf(g, "\n%s ", a.nom);
    fprintf(g, "%s\n", a.password);
    fclose(g);

    printGreenOnWhite("\t\t\t\t\t Administrateur ajoute avec succes !\n");
}

// Fonction pour modifier une information d'un client dans un fichier
int modifier_client(const char *fichier, const char *rib, const char *champ, const char *nouvelle_valeur) {
    FILE *fichier_orig = fopen(fichier, "r");
    if (!fichier_orig) {
        perror("Erreur lors de l'ouverture du fichier original");
        return -1;
    }

    FILE *fichier_temp = fopen("temp_client.txt", "w");
    if (!fichier_temp) {
        perror("Erreur lors de l'ouverture du fichier temporaire");
        fclose(fichier_orig);
        return -1;
    }

    char ligne[200];
    int modification_effectuee = 0;
    int rib_trouve = 0;

    while (fgets(ligne, 200, fichier_orig)) {
        char copie_ligne[200];
        strcpy(copie_ligne, ligne);

        // Supprimer le saut de ligne pour faciliter les comparaisons
        copie_ligne[strcspn(copie_ligne, "\n")] = '\0';

        // Vérifier si c'est un RIB
        if (strstr(copie_ligne, rib) && strstr(copie_ligne, "RIB")) {
            rib_trouve = 1;
        }

        // Si on est dans le bon bloc et on trouve le champ à modifier
        if (rib_trouve && strstr(copie_ligne, champ)) {
            // Modifier la valeur
            fprintf(fichier_temp, "%s : %s\n", champ, nouvelle_valeur);
            modification_effectuee = 1;
        } else {
            // Écrire la ligne telle quelle
            fputs(ligne, fichier_temp);
        }

        // Vérifier si on arrive à la fin du bloc (--- ou autre délimiteur)
        if (strstr(copie_ligne, "---")) {
            rib_trouve = 0; // Sortir du bloc
        }
    }

    fclose(fichier_orig);
    fclose(fichier_temp);

    // Remplacer le fichier original par le fichier temporaire
    if (modification_effectuee) {
        if (remove(fichier) != 0 || rename("temp_client.txt", fichier) != 0) {
            perror("Erreur lors du remplacement du fichier original");
            return -1;
        }
    } else {
        remove("temp_client.txt"); // Supprimer le fichier temporaire s'il n'y a pas de modification
    }

    return modification_effectuee;
}
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
        FILE *file = fopen(demande_creationfile, "r");
        if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", demande_creationfile);
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
    char id[3] ;
    bool demande_complete = false;
     bool demande_trouvee = false;
    while (fgets(ligne, sizeof(ligne), file)) {
        // Lecture des champs ligne par ligne
        if (strncmp(ligne, "ID demande : ", 13) == 0) {
            sscanf(ligne, "ID demande : %s", &id);
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
                // Generer automatiquement le numero de compte
                int dernier_numero = 0;
                FILE *f = fopen(Clientfile, "r");
                if (f) {
                    char ligne_client[200];
                    while (fgets(ligne_client, sizeof(ligne_client), f)) {
                        sscanf(ligne_client, "Numero de compte : %d", &dernier_numero);
                    }
                    fclose(f);
                }
                int nouveau_numero = dernier_numero + 1;

                // Enregistrer les informations dans le fichier client
                f = fopen(Clientfile, "a");
                FILE *l = fopen(LoginClientfile, "a");
                if (f && l) {

                        fprintf(f, "RIB : %s\n", rib);
                        fprintf(f, "Mot de passe : %s\n", password);
                        fprintf(f, "Numero de compte : %d\n", nouveau_numero);
                        fprintf(f, "Nom : %s\n", nom);
                        fprintf(f, "Prenom : %s\n", prenom);
                        fprintf(f, "CIN : %s\n", cin);
                        fprintf(f, "Telephone : %s\n", tele);
                        fprintf(f, "Email : %s\n",email);
                        fprintf(f, "Date de naissance : %s\n", date);
                        fprintf(f, "Type de compte : %s\n",type_compte);
                        fprintf(f, "Solde initial : %.2f\n", solde_initial);
                        fprintf(f, "---------------------------------------------\n");
                        fclose(f);
                        fprintf(l, "%s\t", rib);
                        fprintf(l, "%s\n", password);
                        fclose(l);

                    printFormattedGreenOnWhite("\t\t\t\t Compte cree avec succes : Numero de compte %d.\n", nouveau_numero);
                    getchar();
                    getchar();
                    clearConsole();

                } else {
                    printf("Erreur : Impossible d'ouvrir les fichiers client ou login.\n");
                    if (f) fclose(f);
                    if (l) fclose(l);
                }
            } else if (action == 2) {
                printf("\t\t\t\t\t Demande rejetee.\n");
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
    remove(demande_creationfile);
    rename("temp_creation.txt", demande_creationfile);
    break;
}

            case 2: { // Gestion des demandes de suppression
                char  rib[25] ;
                char id[3];
                bool  demande_complete ;
                bool demande_trouvee = false;
                 FILE *file = fopen(demande_suppressionfile, "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier %s\n", demande_suppressionfile);
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
                            printGreenOnWhite("\t\t\t\t\t Demande approuvee, suppression du compte.\n");
                            supprimerCompte(rib);

                        } else if (action == 2) {
                            afficherCentre("Demande rejetee.\n");
                            FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
            fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de suppression a ete rejete\n\n"); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
                        } else {
                              FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
            fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de supression est en cours de traitement \n\n"); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
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
                remove(demande_suppressionfile);
                rename("temp_suppression.txt", demande_suppressionfile);
                break;
            }

            case 3: { // Gestion des demandes de modification
                bool  demande_complete ;
                bool demande_trouvee = false;
                FILE *file = fopen(demande_modification, "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier %s\n", demande_modification);
                return;
                }

                FILE *temp = fopen("temp_modification.txt", "w");
                if (!temp) {
                printf("Erreur : Impossible de créer un fichier temporaire\n");
                fclose(file);
                return;
                    }
                 char id[3] ;
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
                           printGreenOnWhite("\t\t\t\t\t Demande approuvee, modification du compte en cours.\n");
                            modifier_client(Clientfile, rib,  champ,  nouvelle_valeur);
                              FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
                            fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de modification de votre %s par %s est effectue avec succes\n\n",champ,nouvelle_valeur); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
                            }
                         else if (action == 2) {
                            printf("Demande rejetee.\n");
                              FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
                            fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de modification de votre %s par %s a ete rejete\n\n",champ ,nouvelle_valeur); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
                        } else {
                              FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
                            fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de modification de votre %s par %s est en cours de traitement \n\n",champ,nouvelle_valeur); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
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
                remove(demande_modification);
                rename("temp_modification.txt", demande_modification);

                break;
            }
            case 4:{// Gestion des demandes de carnet cheque
                 FILE *file = fopen(demande_carnet_cheque, "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier %s\n",  demande_carnet_cheque);
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
            char id[3], Nombre[6] , quantite[6];
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
                            afficherCentre("Demande approuvee.\n");
                              FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de carnet de cheque a ete valider , vouillez visiter notre agence pour le recevoir\n\n"); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
                        } else if(action==2) {
                            afficherCentre("Demande rejetee.\n");
                              FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
                            fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de carnet de cheque a ete rejete\n\n"); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
                        }
                        else {
                                  FILE *boite = fopen(boite_messagefile, "a");
        if (boite != NULL) {
            // Récupérer la date et l'heure actuelles
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            // Écrire les informations dans l'boite
            fprintf(boite, "RIB : %s\n", rib); // Ligne 1 : RIB
            fprintf(boite, "Date : %02d/%02d/%04d %02d:%02d:%02d\n",
                    tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                    tm.tm_hour, tm.tm_min, tm.tm_sec); // Ligne 2 : Date
            fprintf(boite, "votre demande de carnet de cheque est en cours de traitement\n\n"); // Ligne 3 : Message personnalisé

            fclose(boite);
        } else {
            printf("Erreur : Impossible d'ouvrir le fichier boite.\n");
        }
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
                remove(demande_carnet_cheque);
                rename("temp_carnet_cheque.txt", demande_carnet_cheque);
                break;
            }

                case 5: {// Gestion des demandes de récupération de mot de passe

                FILE *file = fopen(demande_recuperationfile, "r");
                if (!file) {
                printf("Erreur : Impossible d'ouvrir le fichier %s\n",  demande_recuperationfile);
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
    char id[3];
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
                printf("\t\t\t\t\t Demande approuvee, recupération du mot de passe en cours.\n");

                // Parcourir le fichier LoginClientfile pour trouver le mot de passe
                FILE *loginFile = fopen(LoginClientfile, "r");
                if (loginFile) {
                    char login_ligne[200], rib_login[25], password[20];
                    bool found = false;

                    while (fgets(login_ligne, sizeof(login_ligne), loginFile)) {
                        if (sscanf(login_ligne, "%s %s", rib_login, password) == 2 && strcmp(rib, rib_login) == 0) {
                            found = true;
                            // Simuler l'envoi d'un email
                printf("\t\t\t\t\t Mot de passe envoye a l'email %s : %s\n", email, password);
                            break;
                        }
                    }
                    fclose(loginFile);

                    if (!found) {
                        printFormattedRedOnWhite("\t\t\t\t\t Erreur : Aucun mot de passe trouve pour le RIB %s.\n", rib);
                    }
                } else {
                    printf("Erreur : Impossible d'ouvrir le fichier %s\n", LoginClientfile);
                }
            } else if (action == 2) {
                afficherCentre("Demande rejetee.\n");
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
    remove(demande_recuperationfile);
    rename("temp_recuperation.txt", demande_recuperationfile);
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

void login_admin() {
    int choix;
    int nb_comptes = 0;
    Compte comptes[MAX_COMPTES];

    char username_input[25];
    char password_input[20];
    bool admin_found = false;
    int attempts = 0;

    do {

        printf("\n\n\n");
        printf("\t\t\t\t\t+----------------------------------+\n");
        printf("\t\t\t\t\t|     Nom de l'administrateur      |\n");
         printf("\t\t\t\t\t+----------------------------------+\n");
        printf("\n");
        printf("\t\t\t\t\t+----------------------------------+\n");
         printf("\t\t\t\t\t|\t\t");
        scanf("%s", username_input);
        printf("\t\t\t\t\t+----------------------------------+\n\n");
        printf("\n\t\t\t\t\t+----------------------------------+\n");
        printf("\t\t\t\t\t|\t   Mot de passe            |\n");
        printf("\t\t\t\t\t+----------------------------------+\n");
         printf("\n");
         printf("\t\t\t\t\t+----------------------------------+\n");
         printf("\t\t\t\t\t|\t      ");
        read_password(password_input );//fonction pour masquer le mot de passe
        printf("\t\t\t\t\t+----------------------------------+\n");
        Sleep(300); // Attendre pour afficher la connexion réussie
        clearConsole();
        // Ouvrir le fichier admins.txt en mode lecture
       FILE* file = fopen("loginAdmin.txt", "r");
        if (file == NULL) {
            printf("Erreur : Impossible d'ouvrir le fichier admins.txt\n");
            return;
        }


        char line[100];
        char rib_enree[25];
        while (fgets(line, sizeof(line), file)) {
            char file_username[25];
            char file_password[20];

            // Lecture des noms d'utilisateur et mots de passe depuis le fichier
            if (sscanf(line, "%s %s", file_username, file_password) == 2) {
                if (strcmp(username_input, file_username) == 0 && strcmp(password_input, file_password) == 0) {
                    admin_found = true;
                    changerCouleur(MAGENTA);
                    printf("\n");
                   printf("\t\t\t\t\t Connexion reussie ! Bienvenue, %s.", file_username);
                   changerCouleur(BLACK);
                    fclose(file);
                     do {
        afficherMenuAdmin();
        scanf("%d", &choix);
        clearConsole();
        switch (choix) {
            case 1: // Creer un compte
                creerCompte(comptes, &nb_comptes);
                break;
            case 2: // Modifier un compte
                modifierCompte(comptes, nb_comptes);
                break;
            case 3: // Supprimer un compte
                supprimerCompte(comptes, &nb_comptes);
                break;
            case 4: // Afficher un compte
                changerCouleur(MAGENTA);
                afficherInfoCompte(comptes, nb_comptes);
                afficherCentre("appuyer sur entree pour revenir au menu:\n");
                 getchar();//consomme l'entree \n apres  scanf("%d",&choix);
                 getchar();//Attend une entrée utilisateur
                 clearConsole();
                 changerCouleur(BLACK);
                break;
            case 5: // Afficher tous les comptes
                changerCouleur(MAGENTA);
                afficherTousComptes(comptes, nb_comptes);
                afficherCentre("appuyer sur entree pour revenir au menu:\n");
                 getchar();//consomme l'entree \n apres  scanf("%d",&choix);
                 getchar();//Attend une entrée utilisateur
                 clearConsole();
                 changerCouleur(BLACK);
                break;
            case 6: // Ajouter un admin
                 ajouterAdmin();
                 Sleep(3000); // Attendre pour afficher le message
                 clearConsole();

                break;
            case 7: // Gerer les demandes
               gererDemandes();
               Sleep(3000); // Attendre pour afficher le message
               clearConsole();
                break;
                case 8:

                    printf("\t\t\t\t\t\t donner un rib :");
                    scanf("%s",rib_enree);
                    changerCouleur(MAGENTA);
                    afficherStatistiques(rib_enree);
                    changerCouleur(BLACK);
                    getchar();
                    getchar();
                    clearConsole();
                    break ;

                case 9:
                    changerCouleur(MAGENTA);
                    statistiquesComptes();
                    genererGraphiques();
                    changerCouleur(BLACK);
                     getchar();
                     getchar();
                    clearConsole();
                    break ;
            case 0:
                changerCouleur(PINK);
              printf("\t\t\t\t\t Au revoir ! Merci d'avoir utilise SAR BANK.\n");
                Sleep(3000); // Attendre pour afficher le message
                clearConsole();
                changerCouleur(PINK);
                break;
            default:
                printRedOnWhite("\t\t\t\t\t  Choix invalide.\n");
                break;
        }
    } while (choix != 0);

                    return; // Sortir après une connexion réussie
                }
            }
        }

        fclose(file);

        if (!admin_found) {
            printRedOnWhite("\t\t\t\t\t Nom d'utilisateur ou mot de passe incorrect !\n\n");
            attempts++;
            if (attempts < 3) {
                printf("\t\t\t\t\t Voulez-vous reessayer ? (1. Oui / 2. Quitter)\n");
                int retry_choice;
                scanf("%d", &retry_choice);

                if (retry_choice == 2) {
                    clearConsole();
                    break;
                }
                // Si retry_choice == 1, la boucle continue automatiquement
            } else {
                printRedOnWhite("\t\t\t\t\tTrois tentatives echouees. Veuillez reessayer plus tard.\n");
                Sleep(5000); // Attendre 5 secondes
            }
        }
    } while (attempts < 3 && !admin_found);
}

//AFFICHAGE DANS LE CONSOLE
void setConsoleColors(int textColor, int backgroundColor) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;

    // Récupérer les informations actuelles de la console
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    // Taille totale de la console
    DWORD consoleSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Coordonner le point de départ (haut à gauche)
    COORD coordScreen = {0, 0};

    // Définir les attributs de texte et fond
    SetConsoleTextAttribute(hConsole, (backgroundColor << 4) | textColor);

    // Remplir l'écran avec les nouveaux attributs
    FillConsoleOutputAttribute(
        hConsole,
        (backgroundColor << 4) | textColor,
        consoleSize,
        coordScreen,
        &written
    );

    // Remplir l'écran avec des espaces pour effacer l'ancien contenu
    FillConsoleOutputCharacter(hConsole, ' ', consoleSize, coordScreen, &written);

    // Repositionner le curseur en haut à gauche
    SetConsoleCursorPosition(hConsole, coordScreen);
}

// Fonction pour changer la couleur
void changerCouleur(const char *couleur) {
    printf("%s", couleur);
}

void A() {
    printf("\n\n\n\n\n\n\n");
    printf("\t\t\t  *****        \n");
    printf("\t\t\t  *            \n");
    printf("\t\t\t  *****        \n");
    printf("\t\t\t      *        \n");
    printf("\t\t\t  *****        \n");
    sleep(0); // Attendre avant de nettoyer la console
    system("cls");} // Nettoyer l'écran (sur Windows)
void B() {
    changerCouleur(MAGENTA);
    printf("\n\n\n\n\n\n\n");
    printf("\t\t\t  *****    *          \n");
    printf("\t\t\t  *      *   *        \n");
    printf("\t\t\t  *****  *****        \n");
    printf("\t\t\t      *  *   *        \n");
    printf("\t\t\t  ***** **   **       \n");
    changerCouleur(BLACK);
    sleep(0); // Attendre avant de nettoyer la console
    system("cls");}// Nettoyer l'écran (sur Windows)
void C() {
    changerCouleur(MAGENTA);
    printf("\n\n\n\n\n\n\n");
    printf("\t\t\t  *****    *     *****          \n");
    printf("\t\t\t  *      *   *   *    *         \n");
    printf("\t\t\t  *****  *****   *****          \n");
    printf("\t\t\t      *  *   *   *    *         \n");
    printf("\t\t\t  ***** **   **  ***  **        \n");
    changerCouleur(BLACK);
    sleep(1); // Attendre avant de nettoyer la console
    system("cls"); }// Nettoyer l'écran (sur Windows)
void D() {
    changerCouleur(MAGENTA);
    printf("\n\n\n\n\n\n\n");
    printf("\t\t\t  *****    *     *****     *****          \n");
    printf("\t\t\t  *      *   *   *    *    *    *         \n");
    printf("\t\t\t  *****  *****   *****     *****          \n");
    printf("\t\t\t      *  *   *   *    *    *    *         \n");
    printf("\t\t\t  ***** **   **  ***  **   *****          \n");
    changerCouleur(BLACK);
    sleep(0); // Attendre avant de nettoyer la console
    system("cls"); }// Nettoyer l'écran (sur Windows)
void E() {
    changerCouleur(MAGENTA);
    printf("\n\n\n\n\n\n\n");
    printf("\t\t\t  *****    *     *****     *****     *            \n");
    printf("\t\t\t  *      *   *   *    *    *    *  *   *          \n");
    printf("\t\t\t  *****  *****   *****     *****   *****          \n");
    printf("\t\t\t      *  *   *   *    *    *    *  *   *          \n");
    printf("\t\t\t  ***** **   **  ***  **   *****  **   **         \n");
    changerCouleur(BLACK);
    sleep(0); // Attendre avant de nettoyer la console
    system("cls"); // Nettoyer l'écran (sur Windows)
}
void F() {
    changerCouleur(MAGENTA);
    printf("\n\n\n\n\n\n\n");
    printf("\t\t\t  *****    *     *****     *****     *     ***  ***        \n");
    printf("\t\t\t  *      *   *   *    *    *    *  *   *    **   *         \n");
    printf("\t\t\t  *****  *****   *****     *****   *****    * *  *         \n");
    printf("\t\t\t      *  *   *   *    *    *    *  *   *    *  * *         \n");
    printf("\t\t\t  ***** **   **  ***  **   *****  **   **  ***  **         \n");
    changerCouleur(BLACK);
    sleep(0); // Attendre avant de nettoyer la console
    system("cls");} // Nettoyer l'écran (sur Windows)
    void G() {
    printf("\n\n\n\n\n\n\n");
    printf("\t\t\t  *****    *     *****     *****     *     ***  ***  ***   **     \n");
    printf("\t\t\t  *      *   *   *    *    *    *  *   *    **   *    *   *      \n");
    printf("\t\t\t  *****  *****   *****     *****   *****    * *  *    ***        \n");
    printf("\t\t\t      *  *   *   *    *    *    *  *   *    *  * *    *   *       \n");
    printf("\t\t\t  ***** **   **  ***  **   *****  **   **  ***  **   **    **     \n");
    sleep(1); // Attendre avant de nettoyer la console
    system("cls"); // Nettoyer l'écran (sur Windows)
}

void afficherCentre(const char *texte) {
    int largeur_console = obtenirLargeurConsole();
    int longueur_texte = strlen(texte);
    int espaces = (largeur_console - longueur_texte) / 2;

    // Ajouter les espaces avant le texte
    if (espaces > 0) {
    	int i;
        for ( i = 0; i < espaces; i++) {
            printf(" ");
        }
    }
    printf("%s\n", texte);
}

// Fonction pour obtenir la largeur de la console
int obtenirLargeurConsole() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int largeur = 80; // Valeur par défaut
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        largeur = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return largeur;
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        return w.ws_col;
    }
    return 80; // Valeur par défaut si la largeur n'est pas accessible
#endif
}
// Fonction pour afficher un texte formaté au centre
void afficherCentreSF(const char *format, ...) {
    // Buffer pour contenir le texte formaté
    char texte[1024];

    // Initialiser les arguments variables
    va_list args;
    va_start(args, format);

    // Créer le texte formaté
    vsnprintf(texte, sizeof(texte), format, args);

    // Terminer les arguments variables
    va_end(args);

    // Obtenir la largeur de la console
    int largeur_console = obtenirLargeurConsole();
    int longueur_texte = strlen(texte);

    // Calculer les espaces pour centrer le texte
    int espaces = (largeur_console - longueur_texte) / 2;

    // Ajouter les espaces avant le texte
    if (espaces > 0) {
    	int i;
        for ( i = 0; i < espaces; i++) {
            printf(" ");
        }
    }

    // Afficher le texte
    printf("%s\n", texte);
}

// Fonction pour obtenir la largeur de la console
int obtenirlargeurConsole() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int largeur = 80; // Valeur par défaut
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        largeur = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return largeur;
}

// Fonction pour afficher un texte en vert sur fond blanc
void printGreenOnWhite(const char *text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Sauvegarder les couleurs actuelles
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttributes = csbi.wAttributes;

    // Définir texte vert (2) sur fond blanc (15)
    SetConsoleTextAttribute(hConsole, (15 << 4) | 2); // 15 = blanc, 2 = vert

    // Afficher le texte
    printf("%s", text);

    // Restaurer les couleurs originales
    SetConsoleTextAttribute(hConsole, originalAttributes);
}
// Fonction pour afficher un texte formaté en vert sur fond blanc
void printFormattedGreenOnWhite(const char *format, ...) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Sauvegarder les couleurs actuelles
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttributes = csbi.wAttributes;

    // Définir texte vert (2) sur fond blanc (15)
    SetConsoleTextAttribute(hConsole, (15 << 4) | 2); // 15 = blanc, 2 = vert

    // Préparer le texte formaté
    va_list args;
    va_start(args, format);
    vprintf(format, args); // Affiche le texte formaté
    va_end(args);

    // Restaurer les couleurs originales
    SetConsoleTextAttribute(hConsole, originalAttributes);
}
// Fonction pour afficher un texte en rouge sur fond blanc
void printRedOnWhite(const char *text) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Sauvegarder les couleurs actuelles
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttributes = csbi.wAttributes;

    // Définir texte rouge (4) sur fond blanc (15)
    SetConsoleTextAttribute(hConsole, (15 << 4) | 4); // 15 = blanc (fond), 4 = rouge (texte)

    // Afficher le texte
    printf("%s", text);

    // Restaurer les couleurs originales
    SetConsoleTextAttribute(hConsole, originalAttributes);
}
// Fonction pour afficher un texte formaté en rouge sur fond blanc
void printFormattedRedOnWhite(const char *format, ...) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Sauvegarder les couleurs actuelles
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    WORD originalAttributes = csbi.wAttributes;

    // Définir texte rouge (4) sur fond blanc (15)
    SetConsoleTextAttribute(hConsole, (15 << 4) | 4); // 15 = blanc, 4 = rouge

    // Préparer le texte formaté
    va_list args;
    va_start(args, format);
    vprintf(format, args); // Affiche le texte formaté
    va_end(args);

    // Restaurer les couleurs originales
    SetConsoleTextAttribute(hConsole, originalAttributes);
}

// Fonction principale de gestion
int main() {
    // Fond blanc (15) et texte noir (0)
    setConsoleColors(0, 15);
    A();
    B();
    C();
    D();
    E();
    F();
    G();
    changerCouleur(MAGENTA);
    G();
    changerCouleur(BLACK);
    int choix;
     int c;


    char RIB [25], password[20];
    // Exemple d'initialisation d'administrateur
    Admin admin;
    admin.id_admin = 1;
    strcpy(admin.nom, "Admin");
    strcpy(admin.prenom, "Test");
    strcpy(admin.tel, "123456789");
    strcpy(admin.password, "adminpass");
    do{
    menuprincipal();
    scanf("%d",&c);
     clearConsole();
    if(c==1){
    login_admin();
    }
    if(c==2){
	menuClient();
	scanf("%d", &choix);
	clearConsole();
	if(choix==1){
       remplirDemande_creation();
       }
      if(choix==2){
            login(RIB,password);
       }

    }
}while(c!=0);
    return 0;
}

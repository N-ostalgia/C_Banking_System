#include "database.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

sqlite3 *g_db = NULL;

int db_open(const char *filename) {
    int rc = sqlite3_open(filename, &g_db);
    if (rc != SQLITE_OK) {
        db_print_error("Cannot open database");
        return 0;
    }

    // Créer les tables si elles n'existent pas
    const char *create_tables_sql =
        "CREATE TABLE IF NOT EXISTS clients ("
        "rib TEXT PRIMARY KEY,"
        "password_hash TEXT NOT NULL,"
        "account_number INTEGER UNIQUE,"
        "nom TEXT NOT NULL,"
        "prenom TEXT NOT NULL,"
        "cin TEXT,"
        "telephone TEXT,"
        "email TEXT,"
        "date_naissance TEXT,"
        "type_compte TEXT,"
        "solde REAL DEFAULT 0"
        ");"

        "CREATE TABLE IF NOT EXISTS admins ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "nom TEXT UNIQUE NOT NULL,"
        "prenom TEXT,"
        "tel TEXT,"
        "password_hash TEXT NOT NULL"
        ");"

        "CREATE TABLE IF NOT EXISTS transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "rib TEXT,"
        "transaction_type TEXT,"
        "montant REAL,"
        "description TEXT,"
        "date TEXT,"
        "FOREIGN KEY (rib) REFERENCES clients(rib)"
        ");"

        "CREATE TABLE IF NOT EXISTS demands ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "rib TEXT,"
        "type_demande TEXT,"
        "montant REAL,"
        "rib_dest TEXT,"
        "nouvelles_infos TEXT,"
        "valide INTEGER DEFAULT 0,"
        "date_demande TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (rib) REFERENCES clients(rib)"
        ");"

        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "rib TEXT,"
        "message TEXT,"
        "date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (rib) REFERENCES clients(rib)"
        ");";

    char *err_msg = 0;
    rc = sqlite3_exec(g_db, create_tables_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    // Initialiser les comptes par défaut
    db_initialize_default_accounts();

    return 1;
}

// Fonction pour initialiser les comptes par défaut
void db_initialize_default_accounts() {
    // Vérifier si l'admin par défaut existe déjà
    const char *check_admin_sql = "SELECT COUNT(*) FROM admins WHERE nom = 'admin'";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, check_admin_sql, -1, &stmt, 0);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        int admin_exists = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        if (!admin_exists) {
            // Créer l'admin par défaut
            db_add_admin("admin", "Administrateur", "0600000000", "admin123");
            printf("\n=== Compte Admin par defaut cree ===\n");
            printf("Nom d'utilisateur: admin\n");
            printf("Mot de passe: admin123\n");
            printf("=====================================\n\n");
        }
    }

    // Vérifier si le client par défaut existe déjà
    const char *check_client_sql = "SELECT COUNT(*) FROM clients WHERE rib = 'MA123456789012345678901234'";
    rc = sqlite3_prepare_v2(g_db, check_client_sql, -1, &stmt, 0);
    if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
        int client_exists = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        if (!client_exists) {
            // Créer le client par défaut
            db_add_client("MA123456789012345678901234", "client123", 1001,
                         "Dupont", "Jean", "AB123456", "0612345678",
                         "jean.dupont@email.com", "15/03/1990", "Courant", 5000.0);
            printf("\n=== Compte Client par defaut cree ===\n");
            printf("RIB: MA123456789012345678901234\n");
            printf("Mot de passe: client123\n");
            printf("Solde initial: 5000.00 DH\n");
            printf("=====================================\n\n");
        }
    }
}

void db_close() {
    if (g_db) {
        sqlite3_close(g_db);
        g_db = NULL;
    }
}

void db_print_error(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, sqlite3_errmsg(g_db));
}

char* db_get_current_timestamp() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (!t) {
        return NULL;
    }

    char *timestamp = malloc(30); // Buffer plus grand pour éviter les débordements
    if (!timestamp) {
        return NULL;
    }

    if (snprintf(timestamp, 30, "%02d/%02d/%04d %02d:%02d:%02d",
                 t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
                 t->tm_hour, t->tm_min, t->tm_sec) >= 30) {
        // Si le format est trop long, utiliser un format simplifié
        snprintf(timestamp, 30, "%04d%02d%02d%02d%02d%02d",
                 t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                 t->tm_hour, t->tm_min, t->tm_sec);
    }

    return timestamp;
}

// Client operations
int db_get_client_by_rib(const char *rib, char *nom, char *prenom, double *solde) {
    const char *sql = "SELECT nom, prenom, solde FROM clients WHERE rib = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (nom) {
            const char *nom_text = (const char*)sqlite3_column_text(stmt, 0);
            if (nom_text) {
                strcpy(nom, nom_text);
            } else {
                strcpy(nom, "");
            }
        }
        if (prenom) {
            const char *prenom_text = (const char*)sqlite3_column_text(stmt, 1);
            if (prenom_text) {
                strcpy(prenom, prenom_text);
            } else {
                strcpy(prenom, "");
            }
        }
        if (solde) {
            *solde = sqlite3_column_double(stmt, 2);
        }
        found = 1;
    }
    sqlite3_finalize(stmt);
    return found;
}

int db_get_client_full_info(const char *rib, char *nom, char *prenom, char *cin, char *type_compte, char *telephone, char *email, char *date_naissance, double *solde, int *account_number) {
    const char *sql = "SELECT nom, prenom, cin, type_compte, telephone, email, date_naissance, solde, account_number FROM clients WHERE rib = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(nom, (const char*)sqlite3_column_text(stmt, 0));
        strcpy(prenom, (const char*)sqlite3_column_text(stmt, 1));
        strcpy(cin, (const char*)sqlite3_column_text(stmt, 2));
        strcpy(type_compte, (const char*)sqlite3_column_text(stmt, 3));
        strcpy(telephone, (const char*)sqlite3_column_text(stmt, 4));
        strcpy(email, (const char*)sqlite3_column_text(stmt, 5));
        strcpy(date_naissance, (const char*)sqlite3_column_text(stmt, 6));
        *solde = sqlite3_column_double(stmt, 7);
        *account_number = sqlite3_column_int(stmt, 8);
        found = 1;
    }
    sqlite3_finalize(stmt);
    return found;
}

int db_verify_client_login(const char *rib, const char *password) {
    const char *sql = "SELECT COUNT(*) FROM clients WHERE rib = ? AND password_hash = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        found = sqlite3_column_int(stmt, 0) > 0;
    }
    sqlite3_finalize(stmt);
    return found;
}

int db_add_client(const char *rib, const char *password, int account_number, const char *nom, const char *prenom, const char *cin, const char *telephone, const char *email, const char *date_naissance, const char *type_compte, double solde) {
    const char *sql = "INSERT INTO clients (rib, password_hash, account_number, nom, prenom, cin, telephone, email, date_naissance, type_compte, solde) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, account_number);
    sqlite3_bind_text(stmt, 4, nom, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, prenom, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, cin, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, telephone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 9, date_naissance, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 10, type_compte, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 11, solde);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Insert failed");
        return 0;
    }
    return 1;
}

int db_update_client_solde(const char *rib, double new_solde) {
    const char *sql = "UPDATE clients SET solde = ? WHERE rib = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_double(stmt, 1, new_solde);
    sqlite3_bind_text(stmt, 2, rib, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Update failed");
        return 0;
    }
    return 1;
}

int db_get_all_clients() {
    const char *sql = "SELECT rib, nom, prenom, type_compte, solde FROM clients ORDER BY nom";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    printf("\n=== Liste des Clients ===\n");
    printf("%-15s %-15s %-15s %-15s %-10s\n", "RIB", "Nom", "Prenom", "Type", "Solde");
    printf("------------------------------------------------------------\n");

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-15s %-15s %-15s %-15s %-10.2f\n",
               (const char*)sqlite3_column_text(stmt, 0),
               (const char*)sqlite3_column_text(stmt, 1),
               (const char*)sqlite3_column_text(stmt, 2),
               (const char*)sqlite3_column_text(stmt, 3),
               sqlite3_column_double(stmt, 4));
        count++;
    }
    printf("\nTotal: %d clients\n", count);
    sqlite3_finalize(stmt);
    return count;
}

int db_delete_client(const char *rib) {
    const char *sql = "DELETE FROM clients WHERE rib = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Delete failed");
        return 0;
    }
    return 1;
}

int db_modify_client_field(const char *rib, const char *field, const char *new_value) {
    char sql[256];
    sprintf(sql, "UPDATE clients SET %s = ? WHERE rib = ?", field);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, new_value, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, rib, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Update failed");
        return 0;
    }
    return 1;
}

// Transaction operations
int db_add_transaction(const char *rib, const char *type, double montant, const char *description, const char *date_transaction) {
    const char *sql = "INSERT INTO transactions (rib, transaction_type, montant, description, date_transaction) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, type, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, montant);
    sqlite3_bind_text(stmt, 4, description, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, date_transaction, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Insert failed");
        return 0;
    }
    return 1;
}

int db_get_transactions_by_rib(const char *rib) {
    const char *sql = "SELECT transaction_type, montant, description, date_transaction FROM transactions WHERE rib = ? ORDER BY date_transaction DESC LIMIT 20";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);

    printf("\n=== Historique des Transactions ===\n");
    printf("%-15s %-10s %-40s %-20s\n", "Type", "Montant", "Description", "Date");
    printf("----------------------------------------------------------------\n");

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-15s %-10.2f %-40s %-20s\n",
               (const char*)sqlite3_column_text(stmt, 0),
               sqlite3_column_double(stmt, 1),
               (const char*)sqlite3_column_text(stmt, 2),
               (const char*)sqlite3_column_text(stmt, 3));
        count++;
    }
    printf("\nTotal: %d transactions\n", count);
    sqlite3_finalize(stmt);
    return count;
}

int db_get_transaction_statistics(const char *rib, double *total_deposits, double *total_withdrawals, double *total_transfers, int *total_transactions) {
    const char *sql = "SELECT transaction_type, SUM(montant), COUNT(*) FROM transactions WHERE rib = ? GROUP BY transaction_type";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);

    *total_deposits = 0;
    *total_withdrawals = 0;
    *total_transfers = 0;
    *total_transactions = 0;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *type = (const char*)sqlite3_column_text(stmt, 0);
        double amount = sqlite3_column_double(stmt, 1);
        int count = sqlite3_column_int(stmt, 2);

        if (strcmp(type, "DEPOT") == 0) {
            *total_deposits = amount;
        } else if (strcmp(type, "RETRAIT") == 0) {
            *total_withdrawals = amount;
        } else if (strcmp(type, "VIREMENT") == 0) {
            *total_transfers = amount;
        }
        *total_transactions += count;
    }
    sqlite3_finalize(stmt);
    return 1;
}

// Admin operations
int db_get_admin_by_name(const char *nom, char *prenom, char *telephone) {
    const char *sql = "SELECT prenom, telephone FROM admins WHERE nom = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, nom, -1, SQLITE_STATIC);
    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(prenom, (const char*)sqlite3_column_text(stmt, 0));
        strcpy(telephone, (const char*)sqlite3_column_text(stmt, 1));
        found = 1;
    }
    sqlite3_finalize(stmt);
    return found;
}

int db_verify_admin_login(const char *nom, const char *password) {
    const char *sql = "SELECT COUNT(*) FROM admins WHERE nom = ? AND password_hash = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, nom, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);
    int found = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        found = sqlite3_column_int(stmt, 0) > 0;
    }
    sqlite3_finalize(stmt);
    return found;
}

int db_add_admin(const char *nom, const char *prenom, const char *telephone, const char *password) {
    const char *sql = "INSERT INTO admins (nom, prenom, telephone, password_hash) VALUES (?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, nom, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, prenom, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, telephone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, password, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Insert failed");
        return 0;
    }
    return 1;
}

int db_get_all_admins() {
    const char *sql = "SELECT nom, prenom, telephone FROM admins ORDER BY nom";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    printf("\n=== Liste des Administrateurs ===\n");
    printf("%-15s %-15s %-15s\n", "Nom", "Prenom", "Telephone");
    printf("----------------------------------------\n");

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-15s %-15s %-15s\n",
               (const char*)sqlite3_column_text(stmt, 0),
               (const char*)sqlite3_column_text(stmt, 1),
               (const char*)sqlite3_column_text(stmt, 2));
        count++;
    }
    printf("\nTotal: %d administrateurs\n", count);
    sqlite3_finalize(stmt);
    return count;
}

// Demand operations
int db_add_demand(const char *rib, const char *type_demande, double montant, const char *rib_dest, const char *nouvelles_infos) {
    const char *sql = "INSERT INTO demands (rib, type_demande, montant, rib_dest, nouvelles_infos) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, type_demande, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, montant);
    sqlite3_bind_text(stmt, 4, rib_dest, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, nouvelles_infos, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Insert failed");
        return 0;
    }
    return 1;
}

int db_get_pending_demands() {
    const char *sql = "SELECT id, rib, type_demande, montant, nouvelles_infos FROM demands WHERE valide = 0 ORDER BY date_demande DESC";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    printf("\n=== Demandes en Attente ===\n");
    printf("%-5s %-15s %-15s %-10s %-30s\n", "ID", "RIB", "Type", "Montant", "Informations");
    printf("------------------------------------------------------------\n");

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-5d %-15s %-15s %-10.2f %-30s\n",
               sqlite3_column_int(stmt, 0),
               (const char*)sqlite3_column_text(stmt, 1),
               (const char*)sqlite3_column_text(stmt, 2),
               sqlite3_column_double(stmt, 3),
               (const char*)sqlite3_column_text(stmt, 4));
        count++;
    }
    printf("\nTotal: %d demandes en attente\n", count);
    sqlite3_finalize(stmt);
    return count;
}

int db_approve_demand(int demand_id) {
    const char *sql = "UPDATE demands SET valide = 1 WHERE id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_int(stmt, 1, demand_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Update failed");
        return 0;
    }
    return 1;
}

int db_reject_demand(int demand_id) {
    const char *sql = "DELETE FROM demands WHERE id = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    sqlite3_bind_int(stmt, 1, demand_id);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        db_print_error("Delete failed");
        return 0;
    }
    return 1;
}

// Utility functions
int db_get_next_account_number() {
    const char *sql = "SELECT MAX(account_number) FROM clients";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 1;
    }
    int next_number = 1;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        next_number = sqlite3_column_int(stmt, 0) + 1;
    }
    sqlite3_finalize(stmt);
    return next_number;
}

// Statistics and reporting
int db_get_total_balance() {
    const char *sql = "SELECT SUM(solde) FROM clients";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }
    double total = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        total = sqlite3_column_double(stmt, 0);
    }
    sqlite3_finalize(stmt);
    printf("\nSolde total du système: %.2f DH\n", total);
    return 1;
}

int db_get_account_type_distribution() {
    const char *sql = "SELECT type_compte, COUNT(*) FROM clients GROUP BY type_compte";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    printf("\n=== Distribution des Types de Comptes ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-15s: %d comptes\n",
               (const char*)sqlite3_column_text(stmt, 0),
               sqlite3_column_int(stmt, 1));
    }
    sqlite3_finalize(stmt);
    return 1;
}

int db_get_transaction_type_distribution() {
    const char *sql = "SELECT transaction_type, COUNT(*) FROM transactions GROUP BY transaction_type";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    printf("\n=== Distribution des Types de Transactions ===\n");
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%-15s: %d transactions\n",
               (const char*)sqlite3_column_text(stmt, 0),
               sqlite3_column_int(stmt, 1));
    }
    sqlite3_finalize(stmt);
    return 1;
}

// Fonctions pour l'export des données pour graphiques
void export_transactions_mensuelles() {
    FILE *f = fopen("transactions_mensuelles.txt", "w");
    if (!f) return;

    sqlite3_stmt *stmt;
    const char *sql = "SELECT strftime('%m', date), COUNT(*) FROM transactions GROUP BY strftime('%m', date) ORDER BY strftime('%m', date);";

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *mois = (const char*)sqlite3_column_text(stmt, 0);
            int count = sqlite3_column_int(stmt, 1);
            if (mois) {
                fprintf(f, "%s %d\n", mois, count);
            }
        }
    }
    sqlite3_finalize(stmt);
    fclose(f);
}

void export_types_comptes() {
    FILE *f = fopen("types_comptes.txt", "w");
    if (!f) return;

    sqlite3_stmt *stmt;
    const char *sql = "SELECT type_compte, COUNT(*) FROM clients GROUP BY type_compte;";

    if (sqlite3_prepare_v2(g_db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *type = (const char*)sqlite3_column_text(stmt, 0);
            int count = sqlite3_column_int(stmt, 1);
            if (type) {
                fprintf(f, "%s %d\n", type, count);
            }
        }
    }
    sqlite3_finalize(stmt);
    fclose(f);
}

// Payment operations
int db_pay_bill(const char *rib, double montant) {
    // Vérifier le solde
    double current_solde;
    if (!db_get_client_by_rib(rib, NULL, NULL, &current_solde)) {
        printf("\t\t\t\t\t Compte non trouvé.\n");
        return 0;
    }

    if (current_solde < montant) {
        printf("\t\t\t\t\t Solde insuffisant pour payer la facture.\n");
        return 0;
    }

    // Débiter le compte
    double new_solde = current_solde - montant;
    if (!db_update_client_solde(rib, new_solde)) {
        return 0;
    }

    // Ajouter la transaction
    char *timestamp = db_get_current_timestamp();
    int result = db_add_transaction(rib, "FACTURE", montant, "Paiement de facture", timestamp);
    free(timestamp);

    return result;
}

int db_mobile_recharge(const char *rib, double montant, const char *numero_telephone) {
    // Vérifier les paramètres
    if (!rib || !numero_telephone) {
        printf("\t\t\t\t\t Paramètres invalides.\n");
        return 0;
    }

    // Vérifier le solde
    double current_solde;
    if (!db_get_client_by_rib(rib, NULL, NULL, &current_solde)) {
        printf("\t\t\t\t\t Compte non trouvé.\n");
        return 0;
    }

    if (current_solde < montant) {
        printf("\t\t\t\t\t Solde insuffisant pour la recharge.\n");
        return 0;
    }

    // Débiter le compte
    double new_solde = current_solde - montant;
    if (!db_update_client_solde(rib, new_solde)) {
        return 0;
    }

    // Ajouter la transaction avec description sécurisée
    char description[100];
    if ((int)snprintf(description, sizeof(description), "Recharge mobile %s", numero_telephone) >= (int)sizeof(description)) {
        // Si le numéro est trop long, tronquer
        snprintf(description, sizeof(description), "Recharge mobile");
    }

    char *timestamp = db_get_current_timestamp();
    if (!timestamp) {
        printf("\t\t\t\t\t Erreur lors de la création du timestamp.\n");
        return 0;
    }

    int result = db_add_transaction(rib, "RECHARGE", montant, description, timestamp);
    free(timestamp);

    return result;
}

int db_pay_stamp(const char *rib, double montant) {
    double solde_actuel;
    if (!db_get_client_by_rib(rib, NULL, NULL, &solde_actuel)) {
        fprintf(stderr, "Client non trouve pour le paiement de la vignette.\n");
        return 0; // Client non trouvé
    }

    if (solde_actuel < montant) {
        fprintf(stderr, "Solde insuffisant pour payer la vignette.\n");
        return 2; // Solde insuffisant
    }

    double nouveau_solde = solde_actuel - montant;
    if (!db_update_client_solde(rib, nouveau_solde)) {
        fprintf(stderr, "Erreur lors de la mise a jour du solde apres paiement de la vignette.\n");
        return 0; // Erreur de mise à jour
    }

    char description[200];
    snprintf(description, sizeof(description), "Paiement de la vignette (%.2f DH)", montant);
    char *timestamp = db_get_current_timestamp();
    db_add_transaction(rib, "Vignette", montant, description, timestamp);
    free(timestamp);

    printf("Paiement de la vignette de %.2f DH effectue avec succes.\n", montant);
    return 1; // Succès
}

// Message operations
int db_get_messages(const char *rib) {
    /*
     * This function was for the console version and called UI functions.
     * It is stubbed out to allow the GUI version to compile without
     * "undefined reference" errors. The GUI will fetch messages differently.
    */
    return 1; // Return success, but do nothing.
}

// Fonction pour ajouter un message
int db_add_message(const char *rib, const char *message) {
    // Créer la table messages si elle n'existe pas
    const char *create_messages_sql =
        "CREATE TABLE IF NOT EXISTS messages ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "rib TEXT,"
        "message TEXT,"
        "date TEXT DEFAULT CURRENT_TIMESTAMP,"
        "FOREIGN KEY (rib) REFERENCES clients(rib)"
        ");";

    char *err_msg = 0;
    int rc = sqlite3_exec(g_db, create_messages_sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    // Insérer le message
    const char *sql = "INSERT INTO messages (rib, message) VALUES (?, ?)";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, message, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

// Fonction pour créer un compte client (utilisée dans la gestion des demandes)
int db_create_client_account(const char *rib, const char *nom, const char *prenom, const char *cin, const char *type_compte, const char *email, const char *telephone, const char *date_naissance, const char *password, double solde_initial) {
    int account_number = db_get_next_account_number();
    return db_add_client(rib, password, account_number, nom, prenom, cin, telephone, email, date_naissance, type_compte, solde_initial);
}

// Fonction pour mettre à jour un champ client (utilisée dans la gestion des demandes)
int db_update_client_field(const char *rib, const char *field, const char *new_value) {
    char sql[256];
    snprintf(sql, sizeof(sql), "UPDATE clients SET %s = ? WHERE rib = ?", field);

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, new_value, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, rib, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE);
}

// Fonction pour récupérer le mot de passe d'un client (utilisée dans la gestion des demandes)
int db_get_client_password(const char *rib, char *password) {
    const char *sql = "SELECT password_hash FROM clients WHERE rib = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);
    int found = 0;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *password_hash = (const char*)sqlite3_column_text(stmt, 0);
        if (password_hash) {
            strcpy(password, password_hash);
            found = 1;
        }
    }

    sqlite3_finalize(stmt);
    return found;
}

// Authentication functions
int db_verify_client(const char *rib, const char *password) {
    const char *sql = "SELECT password FROM clients WHERE rib = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *stored_password = (const char*)sqlite3_column_text(stmt, 0);
        if (stored_password && strcmp(stored_password, password) == 0) {
            sqlite3_finalize(stmt);
            return 1;
        }
    }

    sqlite3_finalize(stmt);
    return 0;
}

int db_verify_admin(const char *rib, const char *password) {
    const char *sql = "SELECT password FROM admins WHERE rib = ?";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *stored_password = (const char*)sqlite3_column_text(stmt, 0);
        if (stored_password && strcmp(stored_password, password) == 0) {
            sqlite3_finalize(stmt);
            return 1;
        }
    }

    sqlite3_finalize(stmt);
    return 0;
}

// Fonction pour récupérer les transactions d'un client
int db_get_transactions(const char *rib) {
    const char *sql = "SELECT type, montant, description, date FROM transactions WHERE rib = ? ORDER BY date DESC LIMIT 20";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(g_db, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        db_print_error("Prepare failed");
        return 0;
    }

    sqlite3_bind_text(stmt, 1, rib, -1, SQLITE_STATIC);

    printf("\n");
    printf("\t\t\t\t\t ======================= HISTORIQUE DES TRANSACTIONS =========================\n\n");

    int found = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *type = (const char*)sqlite3_column_text(stmt, 0);
        double montant = sqlite3_column_double(stmt, 1);
        const char *description = (const char*)sqlite3_column_text(stmt, 2);
        const char *date = (const char*)sqlite3_column_text(stmt, 3);

        printf("\t\t\t\t\t Date: %s\n", date);
        printf("\t\t\t\t\t Type: %s\n", type);
        printf("\t\t\t\t\t Montant: %.2f DH\n", montant);
        printf("\t\t\t\t\t Description: %s\n", description);
        printf("\t\t\t\t\t ------------------------------------------------------------\n");
        found = 1;
    }

    if (!found) {
        printf("\t\t\t\t\t Aucune transaction trouvée.\n");
    }

    printf("\t\t\t\t\t ================================================================\n\n");

    sqlite3_finalize(stmt);
    return found;
}

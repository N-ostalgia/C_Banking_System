#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <time.h>

// Database handle
extern sqlite3 *g_db;

// Open and close database
int db_open(const char *filename);
void db_close();
void db_initialize_default_accounts();

// Client operations
int db_get_client_by_rib(const char *rib, char *nom, char *prenom, double *solde);
int db_get_client_full_info(const char *rib, char *nom, char *prenom, char *cin, char *type_compte, char *telephone, char *email, char *date_naissance, double *solde, int *account_number);
int db_add_client(const char *rib, const char *password, int account_number, const char *nom, const char *prenom, const char *cin, const char *telephone, const char *email, const char *date_naissance, const char *type_compte, double solde);
int db_create_client_account(const char *rib, const char *nom, const char *prenom, const char *cin, const char *type_compte, const char *email, const char *telephone, const char *date_naissance, const char *password, double solde_initial);
int db_update_client_solde(const char *rib, double new_solde);
int db_verify_client_login(const char *rib, const char *password);
int db_get_all_clients();
int db_delete_client(const char *rib);
int db_modify_client_field(const char *rib, const char *field, const char *new_value);
int db_update_client_field(const char *rib, const char *field, const char *new_value);
int db_get_client_password(const char *rib, char *password);

// Transaction operations
int db_add_transaction(const char *rib, const char *type, double montant, const char *description, const char *timestamp);
int db_get_transactions(const char *rib);
int db_get_transaction_statistics(const char *rib, double *total_deposits, double *total_withdrawals, double *total_transfers, int *total_transactions);

// Payment operations
int db_pay_bill(const char *rib, double montant);
int db_mobile_recharge(const char *rib, double montant, const char *numero_telephone);
int db_pay_stamp(const char *rib, double montant);

// Message operations
int db_get_messages(const char *rib);
int db_add_message(const char *rib, const char *message);

// Admin operations
int db_get_admin_by_name(const char *nom, char *prenom, char *telephone);
int db_verify_admin_login(const char *nom, const char *password);
int db_add_admin(const char *nom, const char *prenom, const char *telephone, const char *password);
int db_get_all_admins();

// Demand operations
int db_add_demand(const char *rib, const char *type_demande, double montant, const char *rib_dest, const char *nouvelles_infos);
int db_get_pending_demands();
int db_approve_demand(int demand_id);
int db_reject_demand(int demand_id);

// Utility functions
void db_print_error(const char *msg);
char* db_get_current_timestamp();
int db_get_next_account_number();

// Statistics and reporting
int db_get_total_balance();
int db_get_account_type_distribution();
int db_get_transaction_type_distribution();

// Fonctions pour l'export des données pour graphiques
void export_transactions_mensuelles();
void export_types_comptes();

// Authentication functions
int db_verify_client(const char *rib, const char *password);
int db_verify_admin(const char *rib, const char *password);

#endif // DATABASE_H 
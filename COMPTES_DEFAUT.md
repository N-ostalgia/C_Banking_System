# Comptes par Défaut - Système Bancaire SQLite

## 📋 Comptes Créés Automatiquement

Le système crée automatiquement deux comptes par défaut lors de la première exécution :

### 🔐 Compte Administrateur

**Identifiants de connexion :**
- **Nom d'utilisateur :** `admin`
- **Mot de passe :** `admin123`
- **Téléphone :** 0600000000
- **Prénom :** Administrateur

**Fonctionnalités disponibles :**
- Créer des comptes clients
- Modifier des comptes existants
- Supprimer des comptes
- Gérer les demandes
- Afficher les statistiques
- Générer des graphiques
- Ajouter d'autres administrateurs

### 👤 Compte Client

**Identifiants de connexion :**
- **RIB :** `MA123456789012345678901234`
- **Mot de passe :** `client123`
- **Solde initial :** 5000.00 DH

**Informations du compte :**
- **Nom :** Dupont
- **Prénom :** Jean
- **CIN :** AB123456
- **Téléphone :** 0612345678
- **Email :** jean.dupont@email.com
- **Date de naissance :** 15/03/1990
- **Type de compte :** Courant
- **Numéro de compte :** 1001

**Fonctionnalités disponibles :**
- Consulter le solde
- Effectuer des dépôts
- Effectuer des retraits
- Effectuer des virements
- Payer des factures
- Effectuer des recharges mobiles
- Payer des vignettes
- Consulter l'historique des transactions
- Consulter les statistiques personnelles
- Consulter la boîte de messages
- Faire des demandes (suppression, modification, carnet)

## 🚀 Comment Utiliser

### Première connexion :
1. Lancez le programme `banking_sqlite.exe`
2. Les comptes par défaut seront créés automatiquement
3. Utilisez les identifiants ci-dessus pour vous connecter

### Connexion Admin :
1. Choisissez `[1]-ESPACE ADMIN`
2. Entrez : `admin` / `admin123`

### Connexion Client :
1. Choisissez `[2]-ESPACE CLIENT`
2. Choisissez `[2]-SE CONNECTER`
3. Entrez : `MA123456789012345678901234` / `client123`

## ⚠️ Sécurité

**Important :** Ces comptes sont créés à des fins de test et de démonstration. 
En production, il est fortement recommandé de :

1. **Changer immédiatement les mots de passe** après la première connexion
2. **Supprimer ou désactiver** ces comptes par défaut
3. **Créer de nouveaux comptes** avec des identifiants sécurisés
4. **Utiliser des mots de passe forts** (majuscules, minuscules, chiffres, caractères spéciaux)

## 🔧 Personnalisation

Pour modifier les comptes par défaut, éditez la fonction `db_initialize_default_accounts()` dans le fichier `database.c`.

## 📝 Notes

- Les comptes ne sont créés qu'une seule fois lors de la première exécution
- Si vous supprimez la base de données `banking.db`, les comptes seront recréés
- Les mots de passe sont stockés en clair dans cette version (non recommandé pour la production) 
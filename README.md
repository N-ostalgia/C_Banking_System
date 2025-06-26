
---

## ⚡ Installation & Compilation

### **Prérequis**
- [GCC](https://gcc.gnu.org/) ou [MinGW](http://www.mingw.org/) (Windows)
- [SQLite3](https://www.sqlite.org/index.html) (inclus dans le projet)
- [Gnuplot](http://www.gnuplot.info/) (pour les graphiques, optionnel)

### **Compilation (Windows ou Linux)**
```sh
make
```
ou, pour Windows :
```sh
mingw32-make
```
L’exécutable sera généré sous le nom : `banking_sqlite.exe`

---

## 🏁 Utilisation

1. **Lancez l’exécutable** :
   ```sh
   ./banking_sqlite.exe
   ```
2. **Comptes par défaut** (créés à la première exécution) :
   - **Admin** :  
     - Nom d’utilisateur : `admin`  
     - Mot de passe : `admin123`
   - **Client** :  
     - RIB : `MA123456789012345678901234`  
     - Mot de passe : `client123`

3. **Naviguez dans les menus** pour gérer les comptes, effectuer des transactions, etc.

---

## 📊 Génération de graphiques

- Les graphiques sont générés via Gnuplot à partir des fichiers :
  - `transactions_mensuelles.txt`
  - `types_comptes.txt`
- Les images générées :
  - `transactions_mensuelles.png`
  - `types_comptes.png`

---

## 🛡️ Sécurité

- **Les mots de passe sont stockés en clair** (pour la démonstration uniquement).
- **Changez les mots de passe par défaut** après la première connexion.
- **Ne mettez jamais de vraies données personnelles** dans la base de données en production.

---

## 📝 Personnalisation

- Pour modifier les comptes par défaut, éditez la fonction `db_initialize_default_accounts()` dans `database.c`.
- Pour ajouter des fonctionnalités, modifiez `main_sqlite.c` et `database.c`.

---

## 📄 Licence

Ce projet est fourni à des fins pédagogiques et de démonstration.

---

## 👩‍💻 Auteurs

- BOUIBAUAN Aya
- BIFKIOUN Safae
- AMOURAK Rachida

---

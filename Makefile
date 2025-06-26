# Makefile pour le système bancaire SQLite
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I.
TARGET = banking_sqlite.exe
SOURCES = main_sqlite.c database.c sqlite3.c
OBJECTS = $(SOURCES:.c=.o)

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compilation des fichiers objets
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	del /Q *.o $(TARGET) 2>nul || true

# Installation (copie de l'exécutable)
install: $(TARGET)
	copy $(TARGET) C:\Windows\System32\ 2>nul || echo "Installation nécessite des privilèges administrateur"

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make        - Compile le projet"
	@echo "  make clean  - Supprime les fichiers temporaires"
	@echo "  make help   - Affiche cette aide"

.PHONY: all clean install help 
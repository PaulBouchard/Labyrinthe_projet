# Modèle de fichier Makefile, à adapter pour le TP

# options de compilation
CC = gcc
CCFLAGS = -Wall

# fichiers du projet
SRC = deblab.c labyrinthAPI.c clientAPI.c
OBJ = $(SRC:.c=.o)
EXEC = labyrinthe.out


# règle initiale
all: $(EXEC)

# dépendance des .h
deblab.o: deblab.c
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $<

labyrinthAPI.o: labyrinthAPI.c labyrinthAPI.h
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $<	

clientAPI.o: clientAPI.c clientAPI.h
%.o: %.c
	$(CC) $(CCFLAGS) -o $@ -c $<	
# règles d'édition de liens
$(EXEC): $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $^

# règles supplémentaires
clean:
	rm -f *.o
mproper:
	rm -f $(EXEC) *.o
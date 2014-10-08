LIBS=casseur.c my_string.c
OBJ=casseur.o my_string.o
FLAGS=-Wall -g
EXE=casseur

all:compile link

link:compile

	gcc ${OBJ} -o ${EXE} ${FLAGS}
	
compile:${LIBS}

	gcc -c ${LIBS} ${FLAGS}
	
clean:

	rm -f ${EXE} ${OBJ}

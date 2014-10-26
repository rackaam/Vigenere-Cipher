LISCASSEUR=casseur.c my_string.c
OBJSCASSEUR=casseur.o my_string.o
EXECASSEUR=casseur

LIBSVIGENERE=vigenere.c my_string.c
OBJSVIGENERE=vigenere.o my_string.o
EXEVIGENERE=vigenere

FLAGS=-Wall -g

all:vigenere casseur

casseur:${OBJSCASSEUR}

	gcc ${OBJSCASSEUR} -o ${EXECASSEUR} ${FLAGS}

casseurComp:${LIBSCASSEUR}

	gcc -c ${LIBSCASSEUR} ${FLAGS}
	
vigenere:${OBJSVIGENERE}

	gcc ${OBJSVIGENERE} -o ${EXEVIGENERE} ${FLAGS}
	
vigenereComp:${LIBSVIGENERE}

	gcc -c ${LIBSVIGENERE} ${FLAGS}
	
clean:

	rm -f ${EXEVIGENERE} ${EXECASSEUR} ${OBJSVIGENERE} ${OBJSCASSEUR}

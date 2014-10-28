LISCASSEUR=casseur.c my_string.c
OBJSCASSEUR=casseur.o my_string.o
EXECASSEUR=casseur

HEADERS= constantes.h mystring.h

LIBSVIGENERE=vigenere.c my_string.c
OBJSVIGENERE=vigenere.o my_string.o
EXEVIGENERE=vigenere

FLAGS=-Wall -g
#KEY=MACLETROPLONGUEDEOUFquifaitVraimentCh13rachercherAlAMa1n
KEY=ABCDEFGHIJKLMNNM

all:vigenere casseur

casseur:${OBJSCASSEUR}

	gcc ${OBJSCASSEUR} -o ${EXECASSEUR} ${FLAGS}

casseurComp:${LIBSCASSEUR} ${HEADERS}

	gcc -c ${LIBSCASSEUR} ${FLAGS}
	
vigenere:${OBJSVIGENERE}

	gcc ${OBJSVIGENERE} -o ${EXEVIGENERE} ${FLAGS}
	
vigenereComp:${LIBSVIGENERE} ${HEADERS}

	gcc -c ${LIBSVIGENERE} ${FLAGS}

test:casseur vigenere

	valgrind --leak-check=full ./vigenere e Tests/Clair/miserables Tests/Chiffré/enc${KEY} ${KEY}
	valgrind --leak-check=full ./vigenere d Tests/Chiffré/enc${KEY} Tests/Clair/dec${KEY} ${KEY}
	valgrind --leak-check=full ./casseur Tests/Chiffré/enc${KEY}
	diff Tests/Clair/miserables Tests/Clair/dec${KEY}
	
clean:

	rm -f ${EXEVIGENERE} ${EXECASSEUR} ${OBJSVIGENERE} ${OBJSCASSEUR}
	rm -f Tests/Chiffré/enc* Tests/Clair/dec*

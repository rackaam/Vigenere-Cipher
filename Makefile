LISCASSEUR=casseur.c my_string.c
OBJSCASSEUR=casseur.o my_string.o
EXECASSEUR=casseur

HEADERS= constantes.h mystring.h

LIBSVIGENERE=vigenere.c my_string.c
OBJSVIGENERE=vigenere.o my_string.o
EXEVIGENERE=vigenere

FLAGS=-Wall -g
KEY=MACLETROPLNONGUEDEOUFquoi
#KEY=ABCDEFGHIJKLMNNM
#KEY=ABN

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

	valgrind --leak-check=full ./vigenere c Tests/Clair/miserables Tests/Chiffré/enc${KEY} ${KEY}
	valgrind --leak-check=full ./vigenere d Tests/Chiffré/enc${KEY} Tests/Clair/dec${KEY} ${KEY}
	valgrind --leak-check=full ./casseur Tests/Chiffré/enc${KEY}
	diff Tests/Clair/miserables Tests/Clair/dec${KEY}

testexec:

	./vigenere c Tests/Clair/programme Tests/Chiffré/encProg${KEY} ${KEY}
	./vigenere d Tests/Chiffré/encProg${KEY} Tests/Clair/decProg${KEY} ${KEY}
	chmod +x Tests/Clair/decProg${KEY}
	diff Tests/Clair/programme Tests/Clair/decProg${KEY}
	./Tests/Clair/decProg${KEY}
	
clean:

	rm -f ${EXEVIGENERE} ${EXECASSEUR} ${OBJSVIGENERE} ${OBJSCASSEUR} Tests/Chiffré/enc* Tests/Clair/dec*

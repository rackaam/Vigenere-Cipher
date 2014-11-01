LIBSCASSEUR=casseur.c my_string.c libcrypt.c
OBJSCASSEUR=casseur.o my_string.o libcrypt.o
EXECASSEUR=casseur

HEADERS= constantes.h my_string.h libcrypt.h

LIBSVIGENERE=vigenere.c my_string.c libcrypt.c
OBJSVIGENERE=vigenere.o my_string.o libcrypt.o
EXEVIGENERE=vigenere

LIBSFUZZER = fuzzer.c mrg32k3a.c
OBJSFUZZER = fuzzer.o mrg32k3a.o
HEADERSFUZZER = mrg32k3a.h

FLAGS=-Wall -g

#KEY=MACLETROPLNONGUEDEOUFquoi
#KEY=ABCDEFGHIJKLMNNM
KEY=ABN

all:vigenere casseur fuzzer

casseur: casseurComp ${OBJSCASSEUR}

	gcc ${OBJSCASSEUR} -o ${EXECASSEUR} ${FLAGS}

casseurComp:${LIBSCASSEUR} ${HEADERS}

	gcc -c ${LIBSCASSEUR} ${FLAGS}
	
vigenere: vigenereComp ${OBJSVIGENERE}

	gcc ${OBJSVIGENERE} -o ${EXEVIGENERE} ${FLAGS}
	
vigenereComp:${LIBSVIGENERE} ${HEADERS}

	gcc -c ${LIBSVIGENERE} ${FLAGS}

fuzzer: fuzzerComp ${OBJSFUZZER}

	gcc ${OBJSFUZZER} -o fuzzer ${FLAGS}

fuzzerComp:${LIBSFUZZER} ${HEADERSFUZZER}

	gcc -c ${LIBSFUZZER} ${FLAGS}
	
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

	rm -f ${EXEVIGENERE} ${EXECASSEUR} ${OBJSVIGENERE} ${OBJSCASSEUR} Tests/Chiffré/enc* Tests/Clair/dec* fuzzer ${OBJSFUZZER}

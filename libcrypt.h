#ifndef LIBCRYPT_H

#define LIBCRYPT_H

#include <stdlib.h>
#include <stdio.h>
#include "constantes.h"
#include "my_string.h"

/*
	Chiffre un fichier
	L'ensemble des paramètres ne doit pas être NULL
*/
void encrypt_file(char* file_in, char* file_out, char* key, int key_size);

/*
	Déchiffre un fichier
	L'ensemble des paramètres ne doit pas être NULL
*/
void decrypt_file(char* file_in, char* file_out, char* key, int key_size);

/*
	Chiffre un caractère
	k désigne un caractère de la clé
	c désigne un caractère à chiffrer
*/
char encrypt_char(char c, char k);

/*
	Déchiffre un caractère
	k désigne un caractère de la clé
	c désigne un caractère à déchiffrer
*/
char decrypt_char(char c, char k);

#endif

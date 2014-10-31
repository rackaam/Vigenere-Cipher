#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "my_string.h"
#include "constantes.h"

void encrypt_file(char* file_in, char* file_out, char* key);
void decrypt_file(char* file_in, char* file_out, char* key);
char encrypt_char(char c, char k);
char decrypt_char(char c, char k);

int main(int argc, char *argv[]) {
	
	char method;
	char* file_in;
	char* file_out;
	char* key;

	//Précond.
	if(argc != 5)
	{
		fprintf(stderr, "Usage %s [c/d] [FILE_IN] [FILE_OUT] [KEY]\n", argv[0]); 
		exit(BAD_ARGS);
	}
	
	method = argv[1][0];
	file_in = argv[2];
	file_out = argv[3];
	key = argv[4];
	
	if(method == 'c'){
	
		encrypt_file(file_in, file_out, key);
	}
	else if(method == 'd'){
	
		decrypt_file(file_in, file_out, key);
	}
	else{
	
		fprintf(stderr, "Bad option, specify 'c' or 'd' only\n");
	}
	
	return EXIT_SUCCESS;
}

/*
	Chiffre un fichier
	L'ensemble des paramètres ne doit pas être NULL
*/
void encrypt_file(char* file_in, char* file_out, char* key)
{
	int i;
	int key_size = strlen(key);
	string input, output;
	char *out_content;
	
	//Précond.
	if(file_in == NULL || file_out == NULL || key == NULL){
	
		fprintf(stderr, "[encrypt_file] Erreur dans les paramètres\n");
		exit(EXIT_SUCCESS);
	}
	
	//Traitement
	input = readstring(file_in);
	
	if((out_content = (char *)malloc(input.length * sizeof(char))) == NULL){
	
		perror("[encrypt_file] Erreur dans l'allocation de la chaîne de résultat");
		exit(MEM_ERROR);
	}
	
	//Chiffrage des caractères du fichier d'entrée
	for(i = 0; i < input.length; i++){
		
		out_content[i] = encrypt_char(input.content[i], key[i%key_size]);
	}
	
	output.content = out_content;
	output.length = input.length;
	writestring(file_out, output);
	
	//Nettoyage
	free(out_content);
	free(input.content);
}

/*
	Déchiffre un fichier
	L'ensemble des paramètres ne doit pas être NULL
*/
void decrypt_file(char* file_in, char* file_out, char* key)
{
	int i;
	int key_size = strlen(key);
	string input, output;
	char *out_content;
	
	//Précond.
	if(file_in == NULL || file_out == NULL || key == NULL){
	
		fprintf(stderr, "[decrypt_file] Erreur dans les paramètres\n");
		exit(0);
	}
	
	//Traitement
	input = readstring(file_in);
	
	if((out_content = (char*) malloc(input.length * sizeof(char))) == NULL){
	
		perror("[decrypt_file] Erreur dans l'allocation de la chaîne de résultat");
		exit(MEM_ERROR);
	}
	
	//Déchiffrage du fichier d'entrée
	for(i = 0; i < input.length; i++)
	{
		out_content[i] = decrypt_char(input.content[i], key[i%key_size]);
	}

	output.content = out_content;
	output.length = input.length;
	writestring(file_out, output);
	
	//Nettoyage
	free(out_content);
	free(input.content);
}

/*
	Chiffre un caractère
	k désigne un caractère de la clé
	c désigne un caractère à chiffrer
*/
char encrypt_char(char c, char k)
{
	return c+k;
}

/*
	Déchiffre un caractère
	k désigne un caractère de la clé
	c désigne un caractère à déchiffrer
*/
char decrypt_char(char c, char k)
{
	return c-k;
}

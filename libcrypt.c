#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libcrypt.h"
#include "constantes.h"

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

char encrypt_char(char c, char k)
{
	return c+k;
}

char decrypt_char(char c, char k)
{
	return c-k;
}

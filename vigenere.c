#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "my_string.h"

#define BAD_ARGS -2

void encrypt_file(char* file_in, char* file_out, char* key);
void decrypt_file(char* file_in, char* file_out, char* key);
char encrypt_char(char c, char k);
char decrypt_char(char c, char k);

int main(int argc, char *argv[]) {
	char method;
	char* file_in;
	char* file_out;
	char* key;

	if(argc != 5)
	{
		fprintf(stderr, "Erreur BAD_ARGS\n"); 
		exit(BAD_ARGS);
	}
	
	method = argv[1][0];
	file_in = argv[2];
	file_out = argv[3];
	key = argv[4];
	
	if(method == 'e' || method == 'c')
		encrypt_file(file_in, file_out, key);
	else if(method == 'd')
		decrypt_file(file_in, file_out, key);

	return EXIT_SUCCESS;
}

void encrypt_file(char* file_in, char* file_out, char* key)
{
	int i;
	int key_size = strlen(key);
	
	
	string input = readstring(file_in);
	char* out_content = (char*) malloc(input.length * sizeof(char));
	for(i = 0; i < input.length; i++)
	{
		out_content[i] = encrypt_char(input.content[i], key[i%key_size]);
	}
	string output;
	output.content = out_content;
	output.length = input.length;
	writestring(file_out, output);
}

void decrypt_file(char* file_in, char* file_out, char* key)
{
	int i;
	int key_size = strlen(key);
	
	string input = readstring(file_in);
	char* out_content = (char*) malloc(input.length * sizeof(char));
	for(i = 0; i < input.length; i++)
	{
		out_content[i] = decrypt_char(input.content[i], key[i%key_size]);
	}
	string output;
	output.content = out_content;
	output.length = input.length;
	writestring(file_out, output);
}

char encrypt_char(char c, char k)
{
	return (c+k) % SCHAR_MAX;
}

char decrypt_char(char c, char k)
{
	return c-k < 0 ? SCHAR_MAX+(c-k) : c-k;
}

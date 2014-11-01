#include <unistd.h>
#include <string.h>
#include "libcrypt.h"

int main(int argc, char *argv[]) {
	
	int flagi = 0, key_size, opt;
	char method, *file_in, *file_out, *key;
	string s;
	extern char *optarg;
	extern int optind, opterr, optopt;
	
	/* Récupération des options */
	while((opt = getopt(argc, argv, "i")) != -1) {
	
		switch(opt){
		
			case 'i':

				flagi = 1;
				break;

			default:
				break;
		}
	}
	
	//Précond.
	if(optind + 4 != argc){
	
		fprintf(stderr, "Usage %s [-i] c/d FILE_IN FILE_OUT KEY\nL'option -i spécifie que KEY contient le chemin vers un fichier contenant la clé\n", argv[0]); 
		exit(BAD_ARGS);
	}
	
	method = argv[optind][0];
	file_in = argv[optind+1];
	file_out = argv[optind+2];
	
	//Gestion du cas où on lit la clé depuis un fichier
	if(flagi){
	
		s = readstring(argv[optind+3]);
		key_size = s.length-1;
		key = s.content;
		key[key_size] = '\0';
	}
	else{
	
		key = argv[optind+3];
		key_size = strlen(key);
	}
	
	if(method == 'c'){
	
		encrypt_file(file_in, file_out, key, key_size);
	}
	else if(method == 'd'){
	
		decrypt_file(file_in, file_out, key, key_size);
	}
	else{
	
		fprintf(stderr, "Bad option, specify 'c' or 'd' only\n");
	}
	
	if(flagi){
	
		free(key);
	}
	
	return EXIT_SUCCESS;
}

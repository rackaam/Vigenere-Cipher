#include "libcrypt.h"

int main(int argc, char *argv[]) {
	
	char method;
	char* file_in;
	char* file_out;
	char* key;

	//Précond.
	if(argc != 5)
	{
		fprintf(stderr, "Usage %s [c/d] FILE_IN FILE_OUT KEY\n", argv[0]); 
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

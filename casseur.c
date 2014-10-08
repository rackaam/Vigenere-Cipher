#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "my_string.h"

#define BAD_ARGS -2

void calculer_occ(string str, int occ[]);
double calculer_ic(string str, int distance);
string* extraire(string source, int distance);

int main(int argc, char *argv[]){
	
	char* file_in;
	double ic;
	
	if(argc != 2)
	{
		fprintf(stderr, "Erreur BAD_ARGS\n"); 
		exit(BAD_ARGS);
	}
	
	file_in = argv[1];
	
	string str = readstring(file_in);
	
	int i;
	for(i = 1; i < 50; i++)
	{
		ic = calculer_ic(str, i);
		printf("IC %d => %lf\n", i, ic);
		
	} 
	
	return 0;
}

double calculer_ic(string str, int distance){

	int occ[128];
	string* sousChaine = extraire(str, distance);
	calculer_occ(*sousChaine, occ);
	double longueur = (double)sousChaine->length;
	double ic = 0;
	float tab_freq[128];
	int i;
	
	for(i=0; i<128; i++){
	
		tab_freq[i] = (occ[i]/longueur);
		ic += tab_freq[i] * (tab_freq[i] - 1);
	}
	
	printf("IC => %lf et %lf\n", ic, (longueur * (longueur-1))); 
	ic = ic / (longueur * (longueur-1));
	
	free(sousChaine);
	return ic;
}

void calculer_occ(string str, int occ[])
{
	int i;
	for(i = 0; i < 128; i++)
		occ[i] = 0;
	for(i = 0; i < str.length; i++)
	{
		occ[(int)str.content[i]] = occ[(int)str.content[i]] + 1; 
	}
	/*for(i = 0; i < 128; i++)
		printf("\n%d=>%d",i,occ[i]);*/
}

string* extraire(string source, int distance)
{
	string* sousMessage = (string*) malloc(sizeof(string));
	sousMessage->length = source.length / distance;
	sousMessage->content = (char *)malloc(sousMessage->length*sizeof(char));
	
	int i;
	for(i = 0; i < source.length; i += distance)
	{
		sousMessage->content[i] = source.content[i];
	}
	return sousMessage;
} 

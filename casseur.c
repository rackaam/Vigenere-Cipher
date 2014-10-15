#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "my_string.h"

#define BAD_ARGS -2
#define IC_SEUIL 0.06
#define MAX_TAILLE_CLE 10

void calculer_occ(string *str, double occ[]);
double calculer_ic(string *str);
string* extraire(string source, int distance);
int longueur_cle(string *s);
int icOK(double ic);
string* getSousChaine(string *base, int decalage, int espacement);

string* getSousChaine(string *base, int decalage, int espacement){

	if(base == NULL){
	
		return NULL;
	}
	
	string *res = NULL;
	int i, j, taille_alloc = (base->length)/espacement;
	
	if((res = (string *)malloc(sizeof(string))) == NULL){
	
		perror("Erreur dans l'allocation d'une structure string");
	}
	
	if((res->content = (char *)malloc(taille_alloc+1 * sizeof(char))) == NULL){
	
		perror("Erreur dans l'allocation d'une chaîne pour la structure string");
	}
	
	for(i=0; (i*espacement + decalage) < base->length; i++){
	
		res->content[i] = base->content[i*espacement + decalage];
	}
	
	res->content[taille_alloc] = '\0';
	res->length = taille_alloc;
	
	return res;
}

int icOK(double ic){

	return (ic > IC_SEUIL);
}

int longueur_cle(string *s){

	int resultat = -1, i = 1, j;
	double ic, moyenne_ic = 0.0;
	string *sous_chaine = NULL;
	
	do{
	
		i++;
		ic = 0.0;
		
		for(j=0; j<i; j++){
			
			if((sous_chaine = getSousChaine(s, j, i)) == NULL){
			
				fprintf(stderr, "Erreur dans la création d'une sous_chaine");
				exit(1);
			}
			
			//printf("%s\n", sous_chaine->content);
			ic += calculer_ic(sous_chaine);
			
			free(sous_chaine->content);
			free(sous_chaine);
		}
		
		moyenne_ic = ic/j;
		printf("Moyenne ic : %lf\n", moyenne_ic);

	}while(!icOK(moyenne_ic) && i < MAX_TAILLE_CLE);
	
	if(icOK(moyenne_ic)){
	
		return i;
	}
	
	return resultat;
}

int main(int argc, char *argv[]){
	
	char* file_in;
	int l = 0;
	
	if(argc != 2)
	{
		fprintf(stderr, "USAGE %s [ENCRYPTED FILE]\n", argv[0]); 
		exit(BAD_ARGS);
	}
	
	file_in = argv[1];
	string str = readstring(file_in);
	
	if((l = longueur_cle(&str)) != -1){
	
		printf("La longueur de la clé est %i\n", l);
	}
	
	return EXIT_SUCCESS;
}

double calculer_ic(string *str){

	double occ[128];
	double longueur = (double)str->length;
	double ic = 0.0;
	
	//float tab_freq[128];
	int i;
	
	calculer_occ(str, occ);
	
	for(i=0; i<128; i++){
	
		ic += occ[i] * (occ[i] - 1);
	}
	
	//printf("IC => %lf et %lf\n", ic, (longueur * (longueur-1))); 
	//printf("IC unitaire avant : %lf\n", ic);
	ic = ic / (longueur * (longueur-1));
	
	//printf("IC unitaire : %lf\n", ic);
	
	return ic;
}

void calculer_occ(string *str, double occ[])
{
	int i;
	
	for(i = 0; i < 128; i++)
		occ[i] = 0.0;
		
	for(i = 0; i < str->length; i++)
	{
		occ[(int)str->content[i]] = occ[(int)str->content[i]] + 1; 
	}
	
	/*for(i = 0; i < 128; i++)
		printf("\n%d=>%lf",i,occ[i]);*/
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

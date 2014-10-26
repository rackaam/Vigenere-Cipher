#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "my_string.h"

#define BAD_ARGS -2
#define IC_SEUIL 0.06
#define MAX_TAILLE_CLE 100

int longueur_cle(string *s);

//Prototypes en relations avec l'IC
int calculer_occ(string *str, double occ[]);
double calculer_ic(string *str);
int icOK(double ic);

string* getSousChaine(string *base, int decalage, int espacement);

/*
	Retourne une sous-chaîne de la string base, en prenant un caractère tous les espacements, à partir du cractère decalage de la chaîne
	decalage et espacement doivent être positifs
	base ne doit pas être null
	Retourne la sous-chaîne en cas de réussite (à désallouer après utilisation) ou NULL sinon
*/
string* getSousChaine(string *base, int decalage, int espacement){

	string *res = NULL;
	int i, taille_alloc = (base->length)/espacement;
	
	//Préconditions
	if(base == NULL || decalage < 0 || espacement < 0){
	
		return NULL;
	}
	
	//Traitement
	if((res = (string *)malloc(sizeof(string))) == NULL){
	
		perror("Erreur dans l'allocation d'une structure string");
		return NULL;
	}
	
	if((res->content = (char *)malloc(taille_alloc+1 * sizeof(char))) == NULL){
	
		perror("Erreur dans l'allocation d'une chaîne pour la structure string");
		return NULL;
	}
	
	for(i=0; (i*espacement + decalage) < base->length; i++){
	
		res->content[i] = base->content[i*espacement + decalage];
	}
	
	res->content[taille_alloc] = '\0';
	res->length = taille_alloc;
	
	return res;
}

/*
	Retourne 1 si ic est supérieur au seuil définit dans IC_SEUIL
	Retourn -1 si ic est négatif
*/
int icOK(double ic){

	if(ic < 0.0){
	
		return -1;
	}
	
	return (ic > IC_SEUIL);
}

/*
	Analyse une string et retourne la taille probable de la clé ayant servie à la chiffrer
	Retourne -1 en cas d'erreur dans la paramètre s
*/
int longueur_cle(string *s){

	int resultat = -1, i = 1, j;
	double ic, moyenne_ic = 0.0;
	string *sous_chaine = NULL;
	
	//Préconditions
	if(s == NULL){
	
		return -1;
	}
	
	//Traitement
	do{
	
		i++;
		ic = 0.0;
		
		for(j=0; j<i; j++){
			
			if((sous_chaine = getSousChaine(s, j, i)) == NULL){
			
				fprintf(stderr, "Erreur dans la création d'une sous_chaine");
				exit(1);
			}
			
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

/*
	Inscrit dans occ les occurences de chaque caractères à partir de la string str
	Retourne 0 en cas de succès et -1 en cas d'erreur
*/
int calculer_occ(string *str, double occ[])
{
	int i;
	
	//Précond.
	if(str == NULL || occ == NULL){
	
		return -1;
	}

	for(i = 0; i < 128; i++)
		occ[i] = 0.0;
		
	for(i = 0; i < str->length; i++)
	{
		occ[(int)str->content[i]] = occ[(int)str->content[i]] + 1; 
	}
	
	return 0;
}

/*
	Calcule et retourne l'indice de coïncidence d'une string (non NULL)s
	Retourne -1 en cas de mauvais paramètre
*/	
double calculer_ic(string *str){

	double occ[128];
	double longueur = (double)str->length;
	double ic = 0.0;
	int i;
	
	//Préconditions
	if(str == NULL){
	
		return -1;
	}
	
	//Traitement
	calculer_occ(str, occ);
	
	for(i=0; i<128; i++){
	
		ic += occ[i] * (occ[i] - 1);
	}
	
	ic = ic / (longueur * (longueur-1));

	return ic;
}

int main(int argc, char *argv[]){
	
	char* file_in = NULL;
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

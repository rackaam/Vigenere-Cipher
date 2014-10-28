#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "my_string.h"
#include "constantes.h"

#define NO_SOLUTION 3

#define IC_SEUIL 0.06
#define ICM_SEUIL 0.06
#define MAX_TAILLE_CLE 100
#define CAR_COMPARAISON 'e'

string* getSousChaine(string *base, int decalage, int espacement, int addition);

//Prototypes en relation avec l'ICM
int maxTab(int *occ);
char premierIndice(string *s, int longueurCle);
char *trouver_cle(string *s, int longueurCle);
double calculer_icm(string *c1, string *c2);
int icmOK(double icm);

//Prototypes en relations avec l'IC
int calculer_occ(string *str, int occ[]);
double calculer_ic(string *str);
int longueur_cle(string *s);
int icOK(double ic);

/*
	Retourne l'indice du tableau contenant la valeur max. de celui-ci
	Retourne -1 en cas d'erreur
*/
int maxTab(int *occ){

	int i, indMax = -1, max = -1;

	if(occ == NULL){
	
		fprintf(stderr, "Erreur dans le passage de paramètre à maxTab\n");
		return -1;
	}
	
	for(i=0; i<ENCODE_LIMIT; i++){
	
		if(occ[i]>max){
			
			max = occ[i];
			indMax = i;
		}
	}
	
	return indMax;
}

/*
	Retourne le premier caractère de la clé
*/
char premierIndice(string *s, int longueurCle){

	string *c0;
	int occ[ENCODE_LIMIT], diff, max;
	
	if(s == NULL){
	
		fprintf(stderr, "Erreur dans le passage d'arguments à premierIndice\n");
	}
		
	c0 = getSousChaine(s, 0, longueurCle, 0);
	calculer_occ(c0, occ);
	max = maxTab(occ);
	
	diff = max-CAR_COMPARAISON;
	
	free(c0->content);
	free(c0);
	
	return diff < 0 ? (char)diff+ENCODE_LIMIT : (char)diff;
}

/*
	Calcule l'indice de coïncidence mutuelle entre deux chaînes c1 et c2
*/
double calculer_icm(string *c1, string *c2){

	int i, occC1[ENCODE_LIMIT], occC2[ENCODE_LIMIT];
	double somme = 0.0;
	
	//Préconditions
	if(c1 == NULL || c2 == NULL){
	
		fprintf(stderr, "Erreur dans les arguments passés à calculer_icm\n");
		return -1;
	}
	
	//Traitement
	calculer_occ(c1, occC1);
	calculer_occ(c2, occC2);
	
	for(i=0; i<ENCODE_LIMIT; i++){
	
		somme += occC1[i]*occC2[i];
	}
	
	//Retour de l'ICM
	return (somme / ((c1->length)*(c2->length)));
}

/*
	Détermine les décalages entre la chaîne de décalage 0 et toutes celles qui suivent jusqu'à k longeur de la clé
*/
char *trouver_cle(string *s, int longueurCle){

	int i, j, k, temp;
	double icm;
	char *cle;
	string *c0, *cN;
	
	//Précond.
	if(s == NULL || longueurCle <= 0){
	
		fprintf(stderr, "Erreur dans le passage d'arguments à determiner_decalages\n");
		return NULL;
	}

	//Init.
	c0 = getSousChaine(s, 0, longueurCle, 0);
	
	if((cle = (char *)malloc(sizeof(char)*longueurCle+1)) == NULL){
	
		perror("Erreur dans l'allocation de la clé");
		exit(MEM_ERROR);
	}
	cle[0] = premierIndice(s, longueurCle);
	printf("Première lettre de la clé : %c\n", cle[0]);

	//Traitement
	
	//Pour chaque décalage
	for(i=1; i<longueurCle; i++){
		
		//On enlève 1 à la chaîne jusqu'à trouver le décalage adéquat
		for(j=0, icm = 0.0; !icmOK(icm) && j<ENCODE_LIMIT; j++){
			
			cN = getSousChaine(s, i, longueurCle, -j);
			icm = calculer_icm(c0, cN);
			if(i==15){printf("ICM %d= %lf\n",j, icm);}
			free(cN->content);
			free(cN);
		}
		j--;
		
		//On a pas eu d'ICM remarquable durant notre recherche
		if(!icmOK(icm)){
		
			fprintf(stderr, "Impossible de déterminer le décalage de la chaîne d'indice %d\n", i);
			
			free(c0->content);
			free(c0);
			return NULL;
		}
		
		temp = j+cle[0] < 0 ? j+cle[0]+ENCODE_LIMIT : (j+cle[0])%ENCODE_LIMIT;
		cle[i] = (char)temp;
	}	
	cle[longueurCle] = '\0';
	
	//Nettoyage
	free(c0->content);
	free(c0);
	
	return cle;
}

/*
	Retourne vrai si l'ICM dépasse un seuil qui permet de donner très probablement le décalage entre deux chaînes
	Retourne -1 en cas de mauvais passage de paramètre
*/
int icmOK(double icm){

	if(icm < 0.0){
	
		return -1;
	}
	
	return (icm > ICM_SEUIL);
}

/*
	Retourne une sous-chaîne de la string base, en prenant un caractère tous les espacements, à partir du cractère decalage de la chaîne et en ajoutant addition au caractère récupéré dans la chaîne de base.
	decalage et espacement doivent être positifs
	base ne doit pas être null
	Retourne la sous-chaîne en cas de réussite (à désallouer après utilisation) ou NULL sinon
*/
string *getSousChaine(string *base, int decalage, int espacement, int addition){

	string *res = NULL;
	int i, taille_alloc = (base->length)/espacement;
	char temp;
	
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
	
		temp = base->content[i*espacement + decalage] + addition;
		res->content[i] = temp < 0 ? (temp + ENCODE_LIMIT) : temp;
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

	int i, j;
	double ic, moyenne_ic;
	string *sous_chaine;
	
	//Préconditions
	if(s == NULL){
	
		return -1;
	}
	
	//Traitement
	for(i=2, moyenne_ic= 0.0, sous_chaine = NULL; !icOK(moyenne_ic) && i < MAX_TAILLE_CLE; i++){

		ic = 0.0;
		
		//On effectue une moyenne des ic de chaque sous-chaîne afin d'avoir une meilleure précision
		for(j=0; j<i; j++){
			
			sous_chaine = getSousChaine(s, j, i, 0);
			
			ic += calculer_ic(sous_chaine);

			free(sous_chaine->content);
			free(sous_chaine);
		}
		
		moyenne_ic = ic/j;
	}
	i--;
	
	if(icOK(moyenne_ic)){
	
		return i;
	}
	
	return -1;
}

/*
	Inscrit dans occ les occurences de chaque caractères à partir de la string str
	Retourne 0 en cas de succès et -1 en cas d'erreur
*/
int calculer_occ(string *str, int occ[])
{
	int i;
	
	//Précond.
	if(str == NULL || occ == NULL){
	
		return -1;
	}

	//Init.
	for(i = 0; i < ENCODE_LIMIT; i++)
		occ[i] = 0.0;
		
	//Traitement
	for(i = 0; i < str->length; i++){

		occ[(int)str->content[i]] = occ[(int)str->content[i]] + 1; 
	}
	
	return 0;
}

/*
	Calcule et retourne l'indice de coïncidence d'une string (non NULL)s
	Retourne -1 en cas de mauvais paramètre
*/	
double calculer_ic(string *str){

	int occ[ENCODE_LIMIT];
	double longueur = (double)str->length;
	double ic = 0.0;
	int i;
	
	//Préconditions
	if(str == NULL){
	
		return -1.0;
	}
	
	//Traitement
	calculer_occ(str, occ);
	
	for(i=0; i<ENCODE_LIMIT; i++){
	
		ic += occ[i] * (occ[i] - 1);
	}
	
	ic = ic / (longueur * (longueur-1));

	return ic;
}

int main(int argc, char *argv[]){
	
	int l, i;
	string input;
	char *cle;
	
	if(argc != 2)
	{
		fprintf(stderr, "USAGE : %s [FICHIER_CHIFFRÉ]\n", argv[0]); 
		exit(BAD_ARGS);
	}

	input = readstring(argv[1]);
	
	//On cherche la longueur de la clé
	if((l = longueur_cle(&input)) == -1){
	
		fprintf(stderr, "Impossible de déterminer la longueur de la clé de chiffrage\n");
		exit(NO_SOLUTION);
	}
	
	//Récupération de la clé
	if((cle = trouver_cle(&input, l)) == NULL){
	
		fprintf(stderr, "Impossible de trouver la clé\n");
		exit(NO_SOLUTION);
	}
	
	printf("Clé de chiffrage du fichier : %s\n", cle);
	
	//Nettoyage
	free(cle);
	free(input.content);
	
	return EXIT_SUCCESS;
}

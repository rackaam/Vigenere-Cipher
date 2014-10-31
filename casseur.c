#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "my_string.h"
#include "constantes.h"

#define NO_SOLUTION 3

#define IC_SEUIL 0.064
#define ICM_SEUIL 0.064
#define MAX_TAILLE_CLE 50

string* getSousChaine(string *base, int decalage, int espacement, int addition);

//Prototypes en relation avec l'ICM
char maxOcc(int *occ);
char premierIndice(string *s, int longueurCle, char car_comparaison);
char **trouver_cle(string *s, int longueurCle);
int calculer_icm(string *c1, string *c2, double *resultat);
int icmOK(double icm);

//Prototypes en relations avec l'IC
int calculer_occ(string *str, int occ[]);
double calculer_ic(string *str);
int longueur_cle(string *s);
int icOK(double ic);

/*
	Retourne le caractère le plus présent d'après le tableau d'occurences passé en paramètre
	Retourne -1 en cas d'erreur
*/
char maxOcc(int *occ){

	int i, max = -1;
	char carMax = 0;

	if(occ == NULL){
	
		fprintf(stderr, "[maxOcc] Erreur dans le passage des paramètres\n");
		return -1;
	}
	
	for(i=0; i<256; i++){
	
		if(occ[i]>max){
			
			max = occ[i];
			carMax = (i-127);
		}
	}
	
	//On enlève l'addition qu'on a faite lors de la création du tableau d'occurences
	return carMax;
}

/*
	Retourne le premier caractère de la clé
*/
char premierIndice(string *s, int longueurCle, char car_comparaison){

	string *c0;
	int occ[256];
	char diff, max;
	
	if(s == NULL){
	
		fprintf(stderr, "[premierIndice] Erreur dans le passage des paramètres\n");
		return ((char)-1);
	}
		
	c0 = getSousChaine(s, 0, longueurCle, 0);
	calculer_occ(c0, occ);
	max = maxOcc(occ);

	diff = max-car_comparaison;
	
	free(c0->content);
	free(c0);
	
	return diff;
}

/*
	Calcule l'indice de coïncidence mutuelle entre deux chaînes c1 et c2
	Retourne -1 en cas d'erreur dans les arguments
*/
int calculer_icm(string *c1, string *c2, double *resultat){

	int i, occC1[256], occC2[256];
	double somme = 0.0;
	
	/* Préconditions */
	if(c1 == NULL || c2 == NULL){
	
		fprintf(stderr, "[calculer_icm] Erreur dans le passage des paramètres\n");
		return -1;
	}
	
	/* Traitement */
	calculer_occ(c1, occC1);
	calculer_occ(c2, occC2);
	
	for(i=0; i<256; i++){
	
		somme += (occC1[i]*occC2[i]);
	}
	
	*resultat = somme / ((c1->length)*(c2->length));
	return 0;
}

/*
	Détermine les décalages entre la chaîne de décalage 0 et toutes celles qui suivent jusqu'à k longeur de la clé
	Retourne NULL en cas d'erreur
*/
char **trouver_cle(string *s, int longueurCle){

	const char *carComp = "eE ";
	char **cles;
	int i, j, k, temp, tailleComp = strlen(carComp), decalages[longueurCle];
	double icm;
	
	string *c0, *cN;
	
	/* Préconditions */
	if(s == NULL || longueurCle <= 0){
	
		fprintf(stderr, "[trouver_cle] Erreur dans le passage des arguments\n");
		return NULL;
	}

	/* Initialisations */
	
	//Chaîne de base
	c0 = getSousChaine(s, 0, longueurCle, 0);
	
	//Clés
	if((cles = (char **)malloc(sizeof(char *)*tailleComp+1)) == NULL){
	
		perror("[trouver_cle] Erreur dans l'allocation des clés");
		exit(MEM_ERROR);
	}
	cles[tailleComp] = NULL;
	
	for(i=0; i<tailleComp; i++){
	
		if((cles[i] = (char *)malloc(sizeof(char)*longueurCle+1)) == NULL){
	
			perror("[trouver_cle] Erreur dans l'allocation d'une clé");
			exit(MEM_ERROR);
		}
		
		//On trouve la première lettre pour chaque clé
		cles[i][0] = premierIndice(s, longueurCle, carComp[i]);
		cles[i][longueurCle] = '\0';
	}
	
	//Décalage
	decalages[0] = 0;
	
	/* Traitement */
	
	//Calcul des décalages
	for(i=1; i<longueurCle; i++){
		
		//On enlève 1 à la chaîne jusqu'à trouver le décalage adéquat
		for(j=0, icm = 0.0; !icmOK(icm) && j<256; j++){
			
			cN = getSousChaine(s, i, longueurCle, -j);
			calculer_icm(c0, cN, &icm);
			
			free(cN->content);
			free(cN);
		}
		j--;
		
		//On a pas eu d'ICM remarquable durant notre recherche
		if(!icmOK(icm)){
		
			fprintf(stderr, "[trouver_cle] Impossible de déterminer le décalage de la chaîne d'indice %d\n", i);
			
			free(c0->content);
			free(c0);
			
			for(i=0; i<tailleComp; i++)
				free(cles[i]);
				
			free(cles);
			return NULL;
		}
		
		decalages[i] = j;
	}	
	
	//On détermine les clés à l'aide du décalage
	for(i=0; i<tailleComp; i++){
		
		for(j=1; j<longueurCle; j++){
			
			cles[i][j] = decalages[j]+cles[i][0];
		}
	}
	
	/* Nettoyage */
	free(c0->content);
	free(c0);
	
	return cles;
}

/*
	Retourne vrai si l'ICM dépasse un seuil qui permet de donner très probablement le décalage entre deux chaînes
	Retourne -1 en cas de mauvais passage de paramètre (icm négatif)
*/
int icmOK(double icm){

	if(icm < 0.0){
		
		fprintf(stderr, "[icmOK] Erreur dans le passage des paramètres\n");
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
	
	/* Préconditions */
	if(base == NULL || decalage < 0 || espacement < 0){
	
		fprintf(stderr, "[getSousChaine] Erreur dans le passage des paramètres\n");
		return NULL;
	}
	
	/* Traitement */
	if((res = (string *)malloc(sizeof(string))) == NULL){
	
		perror("[getSousChaine] Erreur dans l'allocation d'une structure string");
		exit(MEM_ERROR);
	}
	
	if((res->content = (char *)malloc(taille_alloc+1 * sizeof(char))) == NULL){
	
		perror("[getSousChaine] Erreur dans l'allocation d'une chaîne pour la structure string");
		exit(MEM_ERROR);
	}
	
	for(i=0; (i*espacement + decalage) < base->length; i++){

		res->content[i] = base->content[i*espacement + decalage] + addition;
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
	
		fprintf(stderr, "[icOK] Erreur dans le passage des paramètres\n");
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
	
	/* Préconditions */
	if(s == NULL){
	
		fprintf(stderr, "[longueur_cle] Erreur dans le passage des paramètres\n");
		return -1;
	}
	
	/* Traitement */
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
	
	/* Préconditions */
	if(str == NULL || occ == NULL){
	
		fprintf(stderr, "[calculer_occ] Erreur dans le passage des paramètres\n");
		return -1;
	}

	/* Initialisations */
	for(i = 0; i < 256; i++)
		occ[i] = 0.0;
		
	/* Traitement */
	for(i = 0; i < str->length; i++){

		occ[(int)str->content[i]+127] = occ[(int)str->content[i]+127] + 1; 
	}
	
	return 0;
}

/*
	Calcule et retourne l'indice de coïncidence d'une string (non NULL)s
	Retourne -1 en cas de mauvais paramètre
*/	
double calculer_ic(string *str){

	int occ[256];
	double longueur = (double)str->length;
	double ic = 0.0;
	int i;
	
	/* Préconditions */
	if(str == NULL){
	
		fprintf(stderr, "[calculer_ic] Erreur dans le passage des paramètres\n");
		return -1.0;
	}
	
	/* Traitement */
	calculer_occ(str, occ);
	
	for(i=0; i<256; i++){
	
		ic += occ[i] * (occ[i] - 1);
	}
	
	ic = ic / (longueur * (longueur-1));

	return ic;
}

int main(int argc, char *argv[]){
	
	int l, i, j;
	string input;
	char **cles;
	
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
	if((cles = trouver_cle(&input, l)) == NULL){
	
		fprintf(stderr, "Impossible de trouver les clés de longueur %d\n", l);
		exit(NO_SOLUTION);
	}
	
	//Affichage des clés
	printf("\nClés probables (de longueur %d) :\n\n", l);	
	for(i=0; cles[i]!=NULL; i++){
	
		printf("\t");
		
		for(j=0; j<l; j++){
		
			printf("%c", cles[i][j]);
		}
		
		printf("\t(Hex : ");
		
		for(j=0; j<l; j++){
		
			printf("\\x%x", (unsigned char)cles[i][j]);
		}
		printf(")\n");
	}
	printf("\n");
	
	/* Nettoyage */
	
	for(i=0; cles[i]!=NULL; i++){
		
		free(cles[i]);
	}
	free(cles[i]); 
	free(cles);
	free(input.content);
	
	return EXIT_SUCCESS;
}

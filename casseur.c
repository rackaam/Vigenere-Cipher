#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <ctype.h>

#include "my_string.h"
#include "constantes.h"
#include "libcrypt.h"

#define NO_SOLUTION 3

#define IC_SEUIL 0.07
#define MAX_TAILLE_CLE 50

string* getSousChaine(string *base, int decalage, int espacement, char addition);
int longueur_cle(string *s);
int elire_cle(char **cles, int tailleCles, char *fileComp, char *fileIn);

//Affichage des clés
void afficher_cle_hex(char *cle, int longueurCle);
void afficher_cle_ascii(char *cle, int longueurCle);
void afficher_cle(char *cle, int longueurCle);
int cle_inscriptible(char *cle, int longueurCle);

//Prototypes en relation avec l'ICM
char maxOcc(int *occ);
char premierIndice(string *s, int longueurCle, char car_comparaison);
char **trouver_cle(string *s, int longueurCle);
int calculer_icm(string *c1, string *c2, double *resultat);

//Prototypes en relations avec l'IC
int calculer_occ(string *str, int occ[]);
double calculer_ic(string *str);
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
			carMax = (i-(-SCHAR_MIN));
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

	char **cles;
	int i, j, decalages[longueurCle];
	double icm, icmMax;
	
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
	if((cles = (char **)malloc(sizeof(char *)*257)) == NULL){
	
		perror("[trouver_cle] Erreur dans l'allocation des clés");
		exit(MEM_ERROR);
	}
	cles[256] = NULL;
	
	for(i=0; i<256; i++){
	
		if((cles[i] = (char *)malloc(sizeof(char)*longueurCle+1)) == NULL){
	
			perror("[trouver_cle] Erreur dans l'allocation d'une clé");
			exit(MEM_ERROR);
		}
		
		//On trouve la première lettre pour chaque clé
		cles[i][0] = premierIndice(s, longueurCle, (char)(i-128));
		cles[i][longueurCle] = '\0';
	}
	
	//Décalage
	decalages[0] = 0;
	
	/* Traitement */
	
	//Calcul des décalages
	for(i=1; i<longueurCle; i++){
		
		//On enlève 1 à la chaîne jusqu'à trouver le décalage adéquat
		for(j=0, icm = 0.0, icmMax = 0.0; j<256; j++){
			
			cN = getSousChaine(s, i, longueurCle, -j);
			calculer_icm(c0, cN, &icm);
		
			if(icm > icmMax){
			
				icmMax = icm;
				decalages[i] = j;
			}
			
			free(cN->content);
			free(cN);
		}
	}	
	
	//On détermine les clés à l'aide du décalage
	for(i=0; i<256; i++){
		
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
	Retourne une sous-chaîne de la string base, en prenant un caractère tous les espacements, à partir du cractère decalage de la chaîne et en ajoutant addition au caractère récupéré dans la chaîne de base.
	decalage et espacement doivent être positifs
	base ne doit pas être null
	Retourne la sous-chaîne en cas de réussite (à désallouer après utilisation) ou NULL sinon
*/
string *getSousChaine(string *base, int decalage, int espacement, char addition){

	string *res = NULL;
	int i, taille_alloc = (base->length)/espacement;
	
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

		occ[(int)str->content[i]+(-SCHAR_MIN)] = occ[(int)str->content[i]+(-SCHAR_MIN)] + 1; 
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

/*
	Élit la meilleure clé parmis l'ensemble fournit en paramètre, à partir du calcul de l'ICM avec le modèle passé en paramètre et le texte déchiffré par chacune des clés
	Tous les paramètres doivent être renseignés
	Retourne l'indice de la clé la plus probable en cas de réussite ou -1 en cas d'erreur
*/
int elire_cle(char **cles, int tailleCles, char *fileComp, char *fileIn){

	int indMax, i;
	double icm, icmMax;
	string comp, input;
	
	/* Préconditions */
	if(cles == NULL || fileComp == NULL || fileIn == NULL){
	
		fprintf(stderr, "[elire_cle] Erreur dans le passage des arguments\n");
		return -1;
	}
	
	/* Initialisation */
	icmMax = 0.0;
	comp = readstring(fileComp);
	
	/* Traitement */
	for(i=0 ; cles[i] != NULL; i++){
	
		//Déchiffrage du fichier
		decrypt_file(fileIn, "decrypt.temp", cles[i], tailleCles);
		input = readstring("decrypt.temp");
		
		//Calcul de l'ICM
		calculer_icm(&comp, &input, &icm);

		if(icm > icmMax){
		
			indMax = i;
			icmMax = icm;
		}
		
		free(input.content);
	}
	
	/* Nettoyage */
	free(comp.content);
	if(unlink("decrypt.temp") == -1){
	
		perror("Erreur dans la suppression du fichier decrypt.temp");
		return -1;
	}
	
	return indMax;
}

/*
	Affiche une clé sous la forme chaîne de caractères
	cle ne doit pas être NULL et longueur_cle doit être positif
*/
void afficher_cle_ascii(char *cle, int longueurCle){

	int i;
	
	/* Préconditions */
	if(cle == NULL || longueurCle <= 0){
	
		fprintf(stderr, "[afficher_cle_ascii] Erreur dans le passage des arguments\n");
		return;
	}
	
	for(i=0; i<longueurCle; i++){
		
		printf("%c", cle[i]);
	}
}

/*
	Affiche une clé sous la forme hexadécimale
	cle ne doit pas être NULL et longueur_cle doit être positif
*/
void afficher_cle_hex(char *cle, int longueurCle){

	int i;
	
	/* Préconditions */
	if(cle == NULL || longueurCle <= 0){
	
		fprintf(stderr, "[afficher_cle_hex] Erreur dans le passage des arguments\n");
		return;
	}
	
	for(i=0; i<longueurCle; i++){
		
		printf("\\x%x", (unsigned char)cle[i]);
	}
}

/*
	Retourne 0 si la clé contient des caractères non-inscriptibles, 1 dans le cas inverse et -1 en case d'erreur dans les paramètres
	cle ne doit pas être NULL et longueur_cle doit être positif
*/
int cle_inscriptible(char *cle, int longueurCle){

	int i, inscriptible = 1;
	
	/* Préconditions */
	if(cle == NULL || longueurCle <= 0){
	
		fprintf(stderr, "[cle_inscriptible] Erreur dans le passage des arguments\n");
		return -1;
	}
	
	for(i=0; i<longueurCle; i++){
	
		if(!isprint(cle[i])){
		
			inscriptible = 0;
		}
	}
	
	return inscriptible;
}

/*
	Affiche une clé sous la forme de chaîne de caractère ou hexadécimale selon qu'elle contient seulement des caractères inscriptibles ou non.
	cle ne doit pas être NULL et longueur_cle doit être positif
*/
void afficher_cle(char *cle, int longueurCle){

	/* Préconditions */
	if(cle == NULL || longueurCle <= 0){
	
		fprintf(stderr, "[afficher_cle] Erreur dans le passage des arguments\n");
		return;
	}
	
	/* Traitement */
	printf("\t");
	
	if(cle_inscriptible(cle, longueurCle)){
	
		afficher_cle_ascii(cle, longueurCle);
	}
	else{
	
		printf("Hex : ");
		afficher_cle_hex(cle, longueurCle);
		printf(")");
	}
	printf("\n");
}

int main(int argc, char *argv[]){
	
	int l, i, opt;
	string input;
	char **cles, *fichierComp = NULL, *outputFile = NULL, *inputFile = NULL, *decryptFile = NULL, *meilleureCle;
	FILE *f;
	extern char *optarg;
	extern int optind, opterr, optopt;
	
	/* Récupération des options */
	while((opt = getopt(argc, argv, "f:o:d:")) != -1) {
	
		switch(opt){
		
			case 'f':

				fichierComp = optarg;
				break;
		
			case 'o':
			
				outputFile = optarg;
				break;
			
			case 'd':
			
				decryptFile = optarg;
					
			default:
				break;
		}
	}
	
	
	/* Récupération des arguments */
	if(optind + 1 != argc)
	{
		fprintf(stderr, "USAGE : %s [-f FICHIER_COMPARAISON [-o FICHIER_CLE] [-d FICHIER_DECHIFFRE]] FICHIER_CHIFFRÉ\nL'option \
-o ne fonctionne que si l'option -f est spécifiée et valide\n", argv[0]);
		 
		exit(BAD_ARGS);
	}
	inputFile = argv[optind];
	input = readstring(inputFile);
	
	/* Traitement */
		
	//On cherche la longueur de la clé
	if((l = longueur_cle(&input)) == -1){
	
		fprintf(stderr, "Impossible de déterminer la longueur de la clé de chiffrage\n");
		exit(NO_SOLUTION);
	}
	
	//Récupération des clés probables
	if((cles = trouver_cle(&input, l)) == NULL){
	
		fprintf(stderr, "Impossible de trouver les clés de longueur %d\n", l);
		exit(NO_SOLUTION);
	}
	
	//Comparaison avec un fichier modèle
	if(fichierComp){
	
		meilleureCle = cles[elire_cle(cles, l, fichierComp, inputFile)];
		
		//Inscription de la clé dans le fichier de sortie
		if(outputFile){
		
			if((f = fopen(outputFile, "w")) == NULL){
			
				perror("Erreur dans la création du fichier clé");
				exit(1);
			}
			
			fprintf(f, "%s", meilleureCle);
			fclose(f);
			
			printf("Clé la plus probable inscrite dans le fichier %s\n", outputFile);
		}
		else{
		
			printf("La clé la plus probable au vu du fichier de comparaison est :\n\n");
			afficher_cle(meilleureCle, l);
		}
		
		//On inscrit le texte déchiffré dans le fichié spécifié par l'utilisateur
		if(decryptFile){
		
			decrypt_file(inputFile, decryptFile, meilleureCle, l);
			printf("Le déchiffré du fichier est disponible dans %s\n", decryptFile);
		}
	}
	//Affichage de toutes clés probables
	else{
	
		printf("\nClés probables (de longueur %d) :\n\n", l);	
		for(i=0; cles[i]!=NULL; i++){

			afficher_cle(cles[i], l);
		}
		
		printf("\nClés les plus probables (dans la langue française) :\n\n");
		afficher_cle(cles['e'+128], l);
		afficher_cle(cles['E'+128], l);
		afficher_cle(cles[' '+128], l);
	}
	
	/* Nettoyage */
	
	for(i=0; cles[i]!=NULL; i++){
		
		free(cles[i]);
	}
	free(cles[i]); 
	free(cles);
	free(input.content);
	
	return EXIT_SUCCESS;
}

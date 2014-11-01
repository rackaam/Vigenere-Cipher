#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "mrg32k3a.h"

#define IT_PAR_TAILLE 500
#define MAX_TAILLE 500

char generer_car();
char *generer_cle(long t);
int fichier_existe(char *fichier);
void ajouter_cle_erreur(char *cle);
int realiser_test_vigenere(char *prog, char *fichier_base, char *cle);

/*
	Retourne 0 si le fichier n'existe pas, 1 s'il existe, -1 en cas d'erreur dans le passage des paramètres
*/
int fichier_existe(char *fichier){

	if(fichier == NULL){
		
		fprintf(stderr, "[fichier_existe] Erreur dans le passage des paramètres\n");
		return -1;
	}
	
	return (access(fichier, F_OK) == 0);
}

/*
	Ajoute une clé ayant généré une erreur dans le programme à tester dans un fichier
*/
void ajouter_cle_erreur(char *cle){

	FILE *f;
	
	if(cle == NULL){
	
		fprintf(stderr, "[ajouter_cle_erreur] Erreur dans les paramètres\n");
		return;
	}

	if((f = fopen("cle_erreurs.txt", "a")) == NULL){
	
		perror("Erreur dans l'ouverture du fichier des clés problématiques\n");
		return;
	}
	
	fprintf(f, "%s\n", cle);
	fclose(f);	
}

/*
	Génère un caractère aléatoire entre les valeurs -127 et 127
*/
char generer_car(){

	char c;
	while((c = (char)((MRG32k3a()*254.0)-127.0)) == 0);
	return c;
}

/*
	Génère une clé aléatoire de taille t
	Retourne NULL en cas d'erreur dans les paramètres
*/
char *generer_cle(long t){

	long i;
	char *cle;
	
	if(t <= 0){
	
		fprintf(stderr, "[generer_cle] Erreur dans les paramètres\n");
		return NULL;
	}
	
	if((cle = (char *)malloc(sizeof(char)*t+1)) == NULL){
	
		perror("Erreur d'allocation de la clé");
		exit(1);
	}
	cle[t] = '\0';
	
	for(i=0; i<t; i++){
	
		cle[i] = generer_car();
	}
	
	return cle;
}

/*
	Génère un test en chiffrant le fichier de base avec la clé passée en paramètre, en le déchiffrant ensuite, et en comparant le fichier de base et le fichier déchiffré
	Tous les paramètres doivent être renseignés
*/
int realiser_test_vigenere(char *prog, char *fichier_base, char *cle){

	pid_t pid_fils;
	int code_retour_fils, fd;
	
	if(prog == NULL || cle == NULL || fichier_base == NULL){
	
		fprintf(stderr, "[realiser_test_vignere] Erreur dans le passage des paramètres\n");
		exit(1);
	}
	
	//Création du fichier chiffré
	switch((pid_fils = fork())){
	
		case -1:
		
			perror("[realiser_test_vignere] Erreur dans le fork 1");
			exit(1);
			break;
		
		//Fils
		case 0:
		
			if(execl(prog, prog, "c", fichier_base, "encrypted", cle, NULL) == -1){
			
				perror("[realiser_test_vignere] Erreur dans le lancement du programme de chiffrage");
			}
			exit(1);
			break;
	}
	
	//Père
	if(wait(&code_retour_fils) == -1){
	
		perror("[realiser_test_vignere] Erreur dans l'attente n°1 du fils");
		exit(1);
	}
	
	if(!WIFEXITED(code_retour_fils)){
	
		fprintf(stderr, "[realiser_test_vignere] Le fils n°1 ne s'est pas terminé normalement\n");
		return 0;
	}
	
	//Création du fichier déchiffré
	switch((pid_fils = fork())){
	
		case -1:
		
			perror("[realiser_test_vignere] Erreur dans le fork 2");
			exit(1);
			break;
		
		//Fils
		case 0:
		
			if(execl(prog, prog, "d", fichier_base, "encrypted", cle, NULL) == -1){
			
				perror("[realiser_test_vignere] Erreur dans le lancement du programme de déchiffrage");
			}
			exit(1);
			break;
	}
	
	//Père
	if(wait(&code_retour_fils) == -1){
	
		perror("[realiser_test_vignere] Erreur dans l'attente n°2 du fils");
		exit(1);
	}
	
	if(!WIFEXITED(code_retour_fils)){
	
		fprintf(stderr, "[realiser_test_vignere] Le fils n°2 ne s'est pas terminé normalement\n");
		return 0;
	}
	
	//Comparaison des deux fichiers
	switch((pid_fils = fork())){
	
		case -1:
		
			perror("[realiser_test_vignere] Erreur dans le fork 3");
			exit(1);
			break;
		
		//Fils
		case 0:
		
			//Redirection du flux de sortie
			if((fd = open("/dev/null", O_RDWR)) == -1){
			
				perror("[realiser_test_vignere] Erreur dans l'ouverture de /dev/null");
				exit(1);
			}
			
			close(1);
			if(dup(fd) == -1){
			
				perror("[realiser_test_vignere] Erreur dans le changement du flux de sortie");
				exit(1);
			}
			
			if(execlp("diff", "diff", "-a", fichier_base, "encrypted", NULL) == -1){
			
				perror("[realiser_test_vignere] Erreur dans le lancement du programme de comparaison");
			}
			exit(1);
			break;
	}
	
	//Père
	if(wait(&code_retour_fils) == -1){
	
		perror("[realiser_test_vignere] Erreur dans l'attente n°3 du fils");
		exit(1);
	}
	
	if(!WIFEXITED(code_retour_fils)){
	
		fprintf(stderr, "[realiser_test_vignere] Le fils n°3 ne s'est pas terminé normalement\n");
		return 0;
	}
	
	return WEXITSTATUS(code_retour_fils);
}

int main(int argc, char **argv){

	double taux_reussite, reussite_totale = 0.0;
	long i, j;
	int retour_test;
	char *cle, *programme, *fichier_base;
	
	if(argc != 3){
	
		fprintf(stderr, "Usage : %s [CHEMIN_VIGENERE] [FICHIER_BASE]\n", argv[0]);
		exit(0);
	}
	
	programme = argv[1];
	fichier_base = argv[2];
	
	printf("Paramètres :\n\n\tMAX_TAILLE_CLE %d\n\tNB_ITER : %d\n\n", MAX_TAILLE, IT_PAR_TAILLE);
	
	//Vérification de l'existance des fichiers passés en paramètre
	if(!fichier_existe(programme)){
	
		fprintf(stderr, "Le fichier %s n'existe pas \n", programme);
		exit(1);
	}
	
	if(!fichier_existe(fichier_base)){
	
		fprintf(stderr, "Le fichier %s n'existe pas \n", fichier_base);
		exit(1);
	}
	
	//Pour chaque taille de clé
	for(i=1; i<MAX_TAILLE; i++){
	
		//On effectue un certain nombre d'itérations pour une taille de clé
		for(j=0, taux_reussite = 0.0; j<IT_PAR_TAILLE; j++){
	
			if(!(j%1000)){
			
				printf("Itération %li pour une longueur de clé %li\n", j, i);
			}
			
			cle = generer_cle(i);
			retour_test = realiser_test_vigenere(programme, fichier_base, cle);
			
			//Test échoué
			if(retour_test == 0){

				ajouter_cle_erreur(cle);
			}

			taux_reussite += retour_test;
			free(cle);
		}

		printf("Taux de réussite avec des clés de taille %li : %lf%%\n", i, (taux_reussite*100.0)/(double)IT_PAR_TAILLE);
		reussite_totale += taux_reussite/(double)IT_PAR_TAILLE;
	}
	
	printf("Réussite totale : %lf%%\n", (reussite_totale*100.0)/(double)(MAX_TAILLE-1));
	return EXIT_SUCCESS;
}

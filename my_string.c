#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_string.h"

string readstring(char *file) {
  
  FILE *in;
  int i, n=0;
  string s;
  
  in = fopen(file,"r");
  if(in == NULL) {
    fprintf(stderr,"Pas de fichier '%s' \n",file);
    exit(0);
  }
  
  while (!feof(in)) {
    fgetc(in);
    n++;
  }
  n--;
  
  s.length=n;
  s.content = malloc(n*sizeof(char));
  rewind(in);
 
  for (i=0;i<n;i++) s.content[i] = fgetc(in);

  fclose(in);
  return s;
}

void writestring(char *file, string s) {
  FILE *out;
  int i;
  
  out=fopen(file,"w");
  if (out == NULL) {
    fprintf(stderr,"Problème lors de la création du fichier '%s' \n",file);
    exit(0);
  }
  
  for (i=0;i<s.length;i++) fputc(s.content[i],out);

  fclose(out);
}

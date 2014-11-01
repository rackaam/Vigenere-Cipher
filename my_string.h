#ifndef MYSTRING_H

#define MYSTRING_H

typedef struct {
  char * content;
  int length;
} string;

string readstring(char *file);
void writestring(char *file, string s);

#endif

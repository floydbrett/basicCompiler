#ifndef GENCODE_H
#define GENCODE_H
#include "strtab.h"
#include "tree.h"

extern FILE* yyin;
int getregister();
int nextreg();
void setupframe();
void teardownframe_andrestore();
void setreturnvalue();
void getmain(tree *node, FILE *outfile);
void getfunction(tree *node, FILE *outfile);
void emitfunction(tree *node, FILE *outfile);
int emit(tree *node, FILE *outfile);
int gencode(tree *ast, FILE *outfile);

#endif
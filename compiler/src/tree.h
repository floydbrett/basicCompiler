#ifndef TREE_H
#define TREE_H

#include <../src/strtab.h>

#define MAXCHILDREN 100

typedef struct treenode tree;

/* tree node - you may want to add more fields */
struct treenode {
    struct strEntry* stentry;
      int nodeKind;
      int numChildren;
      int val;
      //int scope;  Used for var/id. Index of the scope. This works b/c only global and local.
      //int type;
      char* str;
      //int sym_type; // Only used by var to distinguish SCALAR vs ARRAY
      tree *parent;
      tree *children[MAXCHILDREN];
};

tree *ast; /* pointer to AST root */

/* builds sub tree with zeor children  */
tree *maketree(int kind);

/* builds sub tree with leaf node */
tree *maketreeWithVal(int kind, int val);

tree* maketreeWithStr(int kind, char* str);

void addChild(tree *parent, tree *child);

void printAst(tree *root, int nestLevel);

/* Adds all children of sublist to list */
void flattenList(tree *list, tree *subList);

/* tree manipulation macros */
/* if you are writing your compiler in C, you would want to have a large collection of these */
enum nodeTypes {
    PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
    FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
    STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
    CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
    ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
    ARGLIST, VAR, ARRAYDECL, FUNCTYPENAME, INTEGER, IDENTIFIER, CHAR, VOID
}
;

/*operators refer to the different types of operators*/
enum operators { ADD, SUB, MUL, DIV, LTE, GTE, EQ, NEQ, ASGN, LT, GT };


#define nextAvailChild(node) node->children[node->numChildren]
#define getChild(node, index) node->children[index]

#endif

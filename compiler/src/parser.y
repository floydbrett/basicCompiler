%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<../src/tree.h>
#include<../src/strtab.h>

extern int yylineno;
typedef struct strEntry strEntry;

tree *ast; /* pointer to AST root */

/* NOTE: mC has two kinds of scopes for variables : local and global. Variables declared outside any
function are considered globals, whereas variables (and parameters) declared inside a function foo are local to foo. You should update the scope variable whenever you are inside a production that matches function definition (funDecl production). The rationale is that you are entering that function, so all variables, arrays, and other functions should be within this scope. You should pass this variable whenever you are calling the ST_insert or ST_lookup functions. This variable should be updated to scope = "" to indicate global scope whenever funDecl finishes. Treat these hints as helpful directions only. You may implement all of the functions as you like and not adhere to my instructions. As long as the directory structure is correct and the file names are correct, we are okay with it. */
%}

/* the union describes the fields available in the yylval variable */
%union
{
    int value;
    struct treenode *node;
    char *strval;
    int index; 
}

/*Add token declarations below. The type <value> indicates that the associated token will be of a value type such as integer, float etc., and <strval> indicates that the associated token will be of string type.*/
%token <strval> ID
%token <value> INTCONST CHARCONST VOIDCONST ERROR ILLEGAL_TOKEN
/* TODO: Add the rest of the tokens below.*/
%token <value> KWD_IF
%token <value> KWD_ELSE
%token <value> KWD_WHILE
%token <value> KWD_INT
%token <value> KWD_CHAR
%token <value> KWD_RETURN
%token <value> KWD_VOID
%token <value> OPER_ADD
%token <value> OPER_SUB
%token <value> OPER_MUL
%token <value> OPER_DIV
%token <value> OPER_LTE
%token <value> OPER_GTE
%token <value> OPER_LT
%token <value> OPER_GT
%token <value> OPER_EQ
%token <value> OPER_NEQ
%token <value> OPER_ASGN
%token <value> LSQ_BRKT
%token <value> RSQ_BRKT
%token <value> LCRLY_BRKT
%token <value> RCRLY_BRKT
%token <value> LPAREN
%token <value> RPAREN
%token <value> COMMA
%token <value> SEMICLN

/* TODO: Declate non-terminal symbols as of type node. Provided below is one example. node is defined as 'struct treenode *node' in the above union data structure. This declaration indicates to parser that these non-terminal variables will be implemented using a 'treenode *' type data structure. Hence, the circles you draw when drawing a parse tree, the following lines are telling yacc that these will eventually become circles in an AST. This is one of the connections between the AST you draw by hand and how yacc implements code to concretize that. We provide with two examples: program and declList from the grammar. Make sure to add the rest.  */

%type <node> program declList decl varDecl typeSpec funDecl formalDeclList formalDecl funbody localDeclList statementList statement compoundStmt assignStmt condStmt loopStmt returnStmt expression relOp addExpr addOp term mulOp factor funcCallExpr argList var arrayDecl funcTypeName

%start program

%%
/* TODO: Your grammar and semantic actions go here. We provide with two example productions and their associated code for adding non-terminals to the AST.*/


/* ST_insert is call to insert into the hash table*/
/* The scope is updating if variables are declare inside a function*/ 
/* There is an error handling if node is null, or is an illegal token*/

program         : 	{ newscope("Global Scope"); } declList
                {
                    	tree* progNode = maketree(PROGRAM);
                    	addChild(progNode, $2);
                    	ast = progNode;
                }
                ;
declList        : 	decl
                {
                    	tree* declListNode = maketree(DECLLIST);
                    	addChild(declListNode, $1);
                   	$$ = declListNode;
                }
                | 	declList decl
                {
                 $$ = $1;
                 addChild($$,$2);
                }
                ;
decl		:	varDecl { $$ = $1; }
        |	funDecl { $$ = $1; }
		;
varDecl		:	typeSpec ID SEMICLN
		{
        strEntry *entry = ST_insert($2,$1->val,SCALAR);
            if(entry == NULL){
                YYABORT;
            }
			tree *declNode = maketree(VARDECL);
			declNode->stentry = entry;
			$$=declNode;
		}
		|	typeSpec ID LSQ_BRKT INTCONST RSQ_BRKT SEMICLN
		{
			strEntry *entry = ST_insert($2,$1->val,ARRAY);
            if(entry == NULL){
                YYABORT;
            }
			tree *declNode = maketree(VARDECL);
            addChild(declNode, $1);
			declNode->stentry = entry;
            addChild(declNode, maketreeWithVal(INTEGER, yylval.value));
			$$ = declNode;
		}
		;
typeSpec	:	KWD_INT
		{	
			$$ = maketreeWithVal(TYPESPEC, INT_TYPE);
		}
		|	KWD_CHAR
		{
                        $$ = maketreeWithVal(TYPESPEC, CHAR_TYPE);
		}
		|	KWD_VOID
		{
			$$ = maketreeWithVal(TYPESPEC, VOID_TYPE);
                }
		;
funDecl		:	funcTypeName LPAREN formalDeclList RPAREN funbody 
		{
			tree* funDeclNode = $1;
            addChild(funDeclNode, $3);
            addChild(funDeclNode, $5);
			$$ = funDeclNode;
            upscope();
        }
		|	funcTypeName LPAREN  RPAREN funbody
		{
			tree *funDeclNode = $1;
			addChild(funDeclNode, $4);
			$$ = funDeclNode;	
            upscope();
		}
		;
funcTypeName	:	typeSpec ID
		{
			strEntry *entry = ST_insert($2,$1->val,FUNCTION);
            if(entry == NULL && strcmp($2, "output")!=0){
                YYABORT;
            }
			tree *funcTypeNameNode = maketree(FUNDECL);
            funcTypeNameNode->stentry = entry;
			$$ = funcTypeNameNode;
            newscope($2);
		}
		; 
formalDeclList 	: 	formalDecl
		{
            tree* formalDeclListNode = maketree(FORMALDECLLIST);
            addChild(formalDeclListNode, $1);
            $$ = formalDeclListNode;
        }
		|	formalDeclList COMMA formalDecl
		{
			$$ = $1;
            addChild($$,$3);
		}
		;
formalDecl	:	typeSpec ID
		{
			strEntry *entry = ST_insert($2,$1->val,SCALAR);
            if (entry == NULL) {
                YYABORT;
            }
			tree *declNode = maketree(FORMALDECL);
			addChild(declNode, maketree(IDENTIFIER));
            declNode->children[0]->stentry = entry;
            $$ = declNode;
		}
		|	typeSpec ID arrayDecl
		{
			ST_insert($2,$1->val,FUNCTION);
			tree* declNode = maketree(FORMALDECL);
                        addChild(declNode, $1);
			addChild(declNode, maketreeWithStr(IDENTIFIER,$2));
                        addChild(declNode, $3);
			$$ = declNode;	
		}
		;
arrayDecl	:	LSQ_BRKT RSQ_BRKT
		{
			tree* arrayDeclNode = maketree(ARRAYDECL);
                        $$ = arrayDeclNode;	
		}
		;
funbody		:	LCRLY_BRKT RCRLY_BRKT
		{
			tree* funbodyNode = maketree(FUNBODY);       
			$$ = funbodyNode;
		}
        |	LCRLY_BRKT localDeclList RCRLY_BRKT
        {
            tree* funbodyNode = maketree(FUNBODY);
            addChild(funbodyNode,$2);
			$$ = funbodyNode;
        }
        |	LCRLY_BRKT statementList RCRLY_BRKT
        {
            tree* funbodyNode = maketree(FUNBODY);
            addChild(funbodyNode,$2);
			$$ = funbodyNode;
        }
		|	LCRLY_BRKT localDeclList statementList RCRLY_BRKT	 
		{
            tree* funbodyNode = maketree(FUNBODY);
            addChild(funbodyNode,$2);       
			addChild(funbodyNode, $3);
			$$ = funbodyNode;
        }
		;
localDeclList	:	varDecl
		{
            $$ = maketree(LOCALDECLLIST);
            addChild($$, $1);
        }
		|	localDeclList varDecl 	
		{
			$$ = $1;
            addChild($$,$2);
		}
		; 
statementList	:	statement
		{
           $$ = maketree(STATEMENTLIST);
            addChild($$,$1);
        }
		|	statementList statement
		{
			$$ = $1;
            addChild($$, $2);
		}
		;
statement	:	compoundStmt { $$ = $1; }
		|	assignStmt { $$ = $1; }
		|	condStmt { $$ = $1; }
		|	loopStmt { $$ = $1; }
		|	returnStmt { $$ = $1; }
		;
compoundStmt	:	LCRLY_BRKT statementList RCRLY_BRKT
                {  	
                   $$ = $2;
                }
                ;
assignStmt	: 	var OPER_ASGN expression SEMICLN
                {
                        tree *assignmentNode = maketree(ASSIGNSTMT);
                        addChild(assignmentNode, $1);
                        addChild(assignmentNode, $3);
                        $$ = assignmentNode;
                }
                |  	expression SEMICLN { $$ = $1; }
                ;
condStmt	: 	KWD_IF LPAREN expression RPAREN statement
                {
                        tree *condStmtNode = maketree(CONDSTMT);
                        addChild(condStmtNode, $3);
                        addChild(condStmtNode, $5);
                        $$ = condStmtNode;
                }
                | 	KWD_IF LPAREN expression RPAREN statement KWD_ELSE statement 
                {
                        tree *condStmtNode = maketree(CONDSTMT);
                        addChild(condStmtNode, $3);
                        addChild(condStmtNode, $5);
                        addChild(condStmtNode, $7);
                        $$ = condStmtNode;
                }
                ; 

loopStmt	:	KWD_WHILE LPAREN expression RPAREN statement
                {
                        tree *loopStmtNode = maketree(LOOPSTMT);
                        addChild(loopStmtNode, $3);
                        addChild(loopStmtNode, $5);
                        $$ = loopStmtNode;
                }
                ;
returnStmt	:	KWD_RETURN SEMICLN
                {
                        tree *returnStmtNode = maketree(RETURNSTMT);
                        $$ = returnStmtNode;
                }
                | 	KWD_RETURN expression SEMICLN
                {
                        tree *returnStmtNode = maketree(RETURNSTMT);
                        addChild(returnStmtNode, $2);
                        $$ = returnStmtNode;
                }
                ;
var		:	ID
                {
                        $$ = maketreeWithStr(IDENTIFIER,$1);
                }
                |       ID LSQ_BRKT addExpr RSQ_BRKT
                {
                        tree *varNode = maketree(VAR);
                        addChild(varNode, maketreeWithStr(IDENTIFIER,$1));
                        addChild(varNode, $3);
                        $$ = varNode;
                }
                ;       
expression	:	addExpr { $$ = $1; }
                |  	expression relOp addExpr
                {
                        $$ = $2;
                        addChild($$, $1);
                        addChild($$, $3);
                }
                ; 
relOp		:	OPER_GTE
                {
                        $$ = maketreeWithVal(RELOP,GTE);
                }
                |  	OPER_GT
                {
                        $$ = maketreeWithVal(RELOP,GT);
                }
                |  	OPER_LT
                {
                        $$ = maketreeWithVal(RELOP,LT);
                }
                |  	OPER_LTE                     
                {
                        $$ = maketreeWithVal(RELOP,LTE);
                }
                |  	OPER_EQ
                {
                        $$ = maketreeWithVal(RELOP,EQ);
                }
                |  	OPER_NEQ
                {
                        $$ = maketreeWithVal(RELOP,NEQ);
                }
		|       OPER_ASGN
                {
                        $$ = maketreeWithVal(RELOP,ASGN);
                }
                ;
addExpr		:	term { $$ = $1; }
                | 	addExpr addOp term
                {
                        $$ = $2;
                        addChild($$,$1);
                        addChild($$,$3);
                }
                ;
addOp		:	OPER_ADD
                {
                        $$ = maketreeWithVal(ADDOP,ADD);
                }
                |  	OPER_SUB
                {
                        $$ = maketreeWithVal(ADDOP,SUB);
                }
                ;
term		:	factor { $$ = $1; }
                |  	term mulOp factor
                {
                    addChild($2, $1);
                    addChild($2, $3);
                    $$ = $2;
                }
                ;
mulOp		:	OPER_MUL
                {
                        $$ = maketreeWithVal(MULOP,MUL);
                }
                |  	OPER_DIV
                {
                        $$ = maketreeWithVal(MULOP,DIV);
                } 
                ;
factor		:	LPAREN expression RPAREN { $$ = $1; }
                |  	var { $$ = $1; }
                |  	funcCallExpr { $$ = $1; }
                |  	INTCONST
                {
                        tree *factorNode = maketree(FACTOR);
                        addChild(factorNode, maketreeWithVal(INTEGER,yylval.value));
                        $$ = factorNode;
                }
                |  	CHARCONST
                {
                        tree *factorNode = maketree(FACTOR);
                        addChild(factorNode, maketreeWithVal(CHAR,yylval.value));
                        $$ = factorNode;
               	}
		;
funcCallExpr	:	ID LPAREN argList RPAREN
                {
                        tree *funcCallExpr = maketree(FUNCCALLEXPR);
                        addChild(funcCallExpr, maketreeWithStr(IDENTIFIER, $1));
                        addChild(funcCallExpr, $3);
                        $$ = funcCallExpr;
                }
                |  	ID LPAREN RPAREN
                {
                        tree *funcCallExpr = maketree(FUNCCALLEXPR);
                        addChild(funcCallExpr, maketreeWithStr(IDENTIFIER,$1));
                        $$ = funcCallExpr;
                }
                ;
argList		:	expression
                {
                   $$ = maketree(ARGLIST);
                   addChild($$,$1);
                }
                |  	argList COMMA expression
                {
                    $$ = $1;
                    addChild($$,$3);
                }
                ;

%%

int yywarning(char * msg){
    printf("warning: line %d: %s\n", yylineno, msg);
    return 0;
}

int yyerror(char * msg){
    printf("error: line %d: %s\n", yylineno, msg);
    return 0;
}

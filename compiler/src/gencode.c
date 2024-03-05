#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>   // definition for nodes of the AST
#include <strtab.h> // the symbol table for needed information about nodes
#include "tree.h"
#include "strtab.h"

extern FILE* yyin;
// a counter for jump labels to be made unique
int labelcounter=0;
// reset register counter (done after calling a func or at beginning of func)
int registercounter=7;

// defines to get the next label counter or to reset registers
#define nextlabel() (++labelcounter)
#define resetregisters() (registercounter=7)
/*
char *nodeNames[33] = {"program", "declList", "decl", "varDecl", "typeSpecifier",
                       "funDecl", "formalDeclList", "formalDecl", "funBody",
                       "localDeclList", "statementList", "statement", "compoundStmt",
                       "assignStmt", "condStmt", "loopStmt", "returnStmt","expression",
                       "relop", "addExpr", "addop", "term", "mulop", "factor",
                       "funcCallExpr", "argList", "integer", "identifier", "var",
                       "arrayDecl", "char", "funcTypeName"};

char *typ[3] = {"int", "char", "void"};
//char *ops[10] = {"+", "-", "*", "/", "<", "<=", "==", ">=", ">", "!="};
*/
/*
Number  | Name      | Description
0       | $zero     | The value 0
2-3     | $v0 - $v1 | (values) from expression evaluation and function results
4-7     | $a0 - $a3 | (arguments) First four parameters for subroutine
8-15    | $t0 - $t7 | Temporary variables
16-23   | $s0 - $s7 | Saved values representing final computed results
24-25   | $t8 - $t9 | Temporary variables2
31      | $ra       |Return address
*/
char *registernames[] = { "$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
                          "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                          "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
                          "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra" };
                          

#define tabprint(...); fprintf(outfile, "\t" __VA_ARGS__)

int getregister(){
	return registernames[registercounter];
}

int nextreg() {
  // jump the gap from $t7 to $t8
  if (++registercounter == 16) registercounter = 24;
  // wrap from $t9 to $t0
  else if (registercounter == 26) registercounter = 8;
  return registercounter;
}



void setupframe(FILE *outfile){ 
    tabprint("sw $fp, 0($sp)\n");
    tabprint("sw $ra, -4($sp)\n");
    tabprint("addu $fp, $zero, $sp\n");
    //find local variable count multiply by 4 
    //and result replaces -32 below
    tabprint("addiu $sp, $sp, -32\n");
}


void teardownframe_andrestore(){

}

void setreturnvalue(){
	
	
	
	
}

void getfunction(tree *node,FILE *outfile){
  if (node != NULL){
    if((node->nodeKind)==5){ //checks for the type[] either being int void or char and prints it
        for(int j = 0; j < node->numChildren; j++){
            if( getChild(node,j)->nodeKind == 0){
                if (strcmp(getChild(node,j)->stentry->id,"main")!=0){
                    tabprint(".globl main\n");
                    fprintf(outfile,"%s:\n", getChild(node,j)->stentry->id);
                    tabprint("sw $fp, 0($sp)\n");
                    tabprint("sw $ra, -4($sp)\n");
                    tabprint("addu $fp, $zero, $sp\n");
                    //find local variable count multiply by 4 
                    //and result replaces -32 below?
                    tabprint("addiu $sp, $sp, -32\n");
                    emit(getChild(node,j),outfile);
                    tabprint("li $v0, 10 # system exit\n");
                    tabprint("syscall\n");
                 }
                return;
            }
        }
    }
     int i;
     for (i = 0; i < node->numChildren; i++) {
             getmain(getChild(node, i),outfile);
         }
     }
  return;
}

void getmain(tree *node, FILE *outfile){
	if(node != NULL){
		if((node->nodeKind)==5){
			fprintf(outfile, "node kind == 5\n");
			for(int i = 0; i < node->numChildren; i++){
				fprintf(outfile, "testing children\n");
				if((getChild(node, i)->nodeKind )== 31){
					fprintf(outfile, "NODES");
					if(strcmp(getChild(node, i)->stentry->id, "main") ==0){
						tabprint(".globl main\n");
						fprintf(outfile, ",%s:\n", getChild(node, i)->stentry->id);
						tabprint("sw $fp, 0($sp)\n");
						tabprint("sw $ra, -4($sp)\n");
                    	tabprint("addu $fp, $zero, $sp\n");
                    	//todo get offset
                    	tabprint("addiu $sp, $sp, -32\n");
                    	tabprint("li $v0, 10 # system exit\n");
                    	tabprint("syscall\n");
                    	
                    	return;
					}
				}
			}
			fprintf(outfile, "after get main");
		}
		
		for(int j = 0; j < node->numChildren; j++){
			getmain(getChild(node, j), outfile);
		}
	}
	
	return;
}

void emitfunction(tree *node, FILE *outfile){
	if (strcmp(node->stentry->id,"main")==0) return ; // we already emitted this function
	
	fprintf(outfile, "%s\n", node->stentry->id);
	setupframe(outfile);
	
	for(int i = 0; i < node->numChildren; i++){
		emit(node->children[i]);
	}
	
	fprintf(outfile, "jal $ra\n");
}

int emit(tree *node, FILE *outfile){
	int lhs, rhs, dst;
	switch(node->nodeKind){
		
		case 0:
			
		case 1:
			
		case 2:
		
		case 3:
		
		case 4:
		
		case 5:
			if(strcmp(ast->stentry->id, "main")){
				fprintf(outfile, "in case 5\n");
			}
			emitfunction(node, outfile);
			return 0;
			
		
		case 6:
		
		case 7:
		
		case 8:
		
		case 9:
		
		case 10:
		
		case 11:
		
		case 12:
			
		case 13: //assignment statement
			rhs = emit(node->children[1], outfile);
			dst = nextreg();
			tabprint("move %s, %s #assign variable %s\n", registernames[dst], registernames[rhs], node->stentry->id);
			tabprint("sw %s, -4($fp) #store %s into the stack", registernames[dst], node->stentry->id);
			
		case 14:
		
		case 15:
		
		case 16: //return statement
		
		case 17:
		
		case 18: //relation operation
		
		case 19:
		
		case 20: //addition/subtraction operation
			lhs = emit(node->children[0], outfile);
			rhs = emit(node->children[1], outfile);
			dst = nextreg();
			if(node->val == 0){
				tabprint("add %s, %s %s # addop\n", registernames[dst], registernames[lhs], registernames[rhs]);
			}
			if(node->val == 1){
				tabprint("sub %s, %s %s # subop\n", registernames[dst], registernames[lhs], registernames[rhs]);
			}
			return dst;
			
		case 21:
			
		case 22: //mult/div operation
			lhs = emit(node->children[0], outfile);
			rhs = emit(node->children[1], outfile);
			dst = nextreg();
			if(node->val == 2){
				tabprint("mul %s, %s %s # mulop\n", registernames[dst], registernames[lhs], registernames[rhs]);
			}
			if(node->val == 3){
				tabprint("div %s, %s %s # divop\n", registernames[dst], registernames[lhs], registernames[rhs]);
			}
			return dst;
		
			
		case 23:
			
		case 24:
		
		case 25:
			if(strcmp(ast->stentry->id, "output")){
				rhs = gencode(ast->children[0]->children[0]);
				printf("li $v0, 1\n");
				printf("mov $v0, %s\n", registernames[rhs]);
				printf("syscall\n");
			}
			else{
				resetregisters();
			}
			return 0;
			
			break;
		
		case 26: //integer
			dst = getregister();
			fprintf(outfile, "li %s %d\n", registernames[dst], ast->val);
			return dst;
			
		case 27:
			
		case 28:
			
		case 29:
			
		case 30: //char
			dst = getregister();
			fprintf(outfile, "li %s, %c", registernames[dst], ast->val);
			return dst;
			
			
		
		
	}
	
	
	
}


/*
An example of how / why for label counters

Suppose we have:
FORNODE
FORNODE->children[0] = stmtnode (init)
FORNODE->children[1] = condnode (condition)
FORNODE->children[2] = compoundstmt (body)
FORNODE->children[3] = stmtnode (post)

void emitforstmt(tree *ast,FILE *outfile) {
  // Get a unique label number for the labels of this FORSTMT
  const int forlabel = nextlabel();

  // emit the for statement:
  fprintf(outfile,"FORSTMT_INIT%d: # Preparing to do for loop\n",
                  forlabel);
  gencode(ast->children[0],outfile);
  fprintf(outfile,"FORSTMT_COND%d: # For statement condition\n",
                  forlabel);
  // how you evaluate conditions will probably vary
  const int result = gencode(ast->children[1],outfile);
  // here I am assuming this register will equal zero if the condition is false
  fprintf(outfile,"beq %s, $zero, FORSTMT_END%d # Exit the for statement\n",
                  registernames[result], forlabel);
  // otherwise we do the body of the for loop
  fprintf(outfile,"FORSTMT_BODY%d: # Begin the for statement\'s body\n",
                  forlabel);
  // emit the body of the loop
  gencode(ast->children[2],outfile);
  // do the post expression
  fprintf(outfile,"FORSTMT_POST%d: # For statement post statement\n",
                  forlabel);
  // We need to return to the for loop's condition
  fprintf(outfile,"j FORSTMT_COND%d # Return to for condition\n",
                  forlabel);
  // Now we print the label for the for statement exit
  fprintf(outfile,"FORSTMT_END%d: # End of for loop\n",
                  forlabel);
}
int gencode(tree *ast,FILE *outfile) {
  switch(ast->nodeKind) {
  case FORSTMT:
    emitforstmt(ast,outfile); return 0;
  }
  // if we don't return a value in the switch statement then it wasn't handled
  // returning -1 should cause a seg-fault if used to index registernames[]
  return -1;
}
*/

/*
 * The following page is a good reference for MIPS
https://ecs-network.serv.pacific.edu/ecpe-170/tutorials/mips-instruction-set
*/
int gencode(tree *ast,FILE *outfile) {
  // variables can't be defined inside a switch
  // move more complicated things to their own function to keep this cleaner
  // (e.g., function definition needs to setup/teardown stack, ...)
  tabprint(".data\n");
  
  
  tabprint(".text\n");
  
  setupframe(outfile);
  
  for(int i = 0; i < ast->numChildren; i++){
  	getmain(getChild(ast, i), outfile);
  }
  /*
  for(int i = 0; i < ast->numChildren; i++){
  	getfunction(getChild(ast, i), outfile);
  }
  */
  

/*
  int lhs,rhs,dst;
  switch(ast->nodeKind) {
// output(6);
    case INTCONST:
      dst = getregister();
      fprintf(outfile,"li %s, %d\n",
                      registernames[dst], ast->value);
      return dst;
/*
 *     FUNCCALLNODE
 *     ARGLIST
 *     EXPRESSION , ....
 */
 /*
    case FUNCDEFNODE:
      setupframe();
      resetregisters();
      for all statements in funcdefnode emitcode();
      teardownframe_andrestore();
      setreturnvalue();
      if (strcmp(funcname,"main")==0)
        syscall_for_clean_exit();
      else {
        fprintf(outfile,"jr $ra\n");
      }
    case FUNCCALLNODE:
      if (strcmp(ast->id,"output")) {
        rhs = gencode(ast->children[0]->children[0]);
        printf("li $v0, 1\n");
        printf("mov $a0, %s\n",registernames[rhs]);
        printf("syscall\n");
      }
      else {
        // save registers, prepare to move context
        // resolve arguments, put them where appropriate
        // jump and link to function label
        // immediately after jump and link all $t_ registers are garbage.
        // return value will be (somewhere)
        resetregisters();
        // anything needed will have to be restored from the stack
      }
      // return 0, break, (?)
      return 0;
      break;
    default: break;
  }
*/
  return 0; // (?)
}


// get the next available register
// this naiively just cycles through $t0 -> $t9 -> $t0


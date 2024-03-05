#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "strtab.h"


typedef struct table_node table_node;
typedef struct strEntry strEntry;

table_node* current_scope = NULL;

/* Provided is a hash function that you may call to get an integer back. */
unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int numcount = 0;

strEntry* ST_insert(char* id, int _data_type, int _symbol_type) {
	unsigned long index = hash(id) % MAXIDS;
	for (int i = 0; i < MAXIDS; ++i, ++index) {
		if (index == MAXIDS) index = 0;
		if (current_scope->strTable[index] == NULL) {
			current_scope->strTable[index] = malloc(sizeof(strEntry));
			current_scope->strTable[index]->id = id;
			current_scope->strTable[index]->data_type = _data_type;
			current_scope->strTable[index]->symbol_type = _symbol_type;
			return current_scope->strTable[index];
		}
		if (strcmp(current_scope->strTable[index]->id, id) == 0) {
			printf("Variable %s is duplicately declared\n", id);
			return NULL;
		}
	}
	printf("Symbol table is Full!\n");
	return NULL;
}

strEntry* ST_lookup(char* id) {
	// TODO: Concatenate the scope and id and use that to create the hash key
	const unsigned long startindex = hash(id) % MAXIDS;

	table_node* searchscope = current_scope;
	while (searchscope != NULL) {
		for (int i = 0, searchindex = startindex; i < MAXIDS; ++i, ++searchindex) {
			if (searchindex == MAXIDS) searchindex = 0;
			if (searchscope->strTable[searchindex] == NULL) {
				break;
			}
			if (strcmp(searchscope->strTable[searchindex]->id, id) == 0) {
				return searchscope->strTable[searchindex];
			}
		}
		searchscope = searchscope->parent;
	}
	printf("Variable %s is not found / undeclared! \n", id);
	return NULL;
}

void print_sym_tab_r(table_node* scope) {
	printf("Scope: %s\n", scope->scopename);
	for (int i = 0; i < MAXIDS; ++i) {
		if (scope->strTable[i] == NULL) continue;
		printf("id=%s, data_type=%d, symtype=%d\n",
			scope->strTable[i]->id,
			scope->strTable[i]->data_type,
			scope->strTable[i]->symbol_type);
	}
	table_node* traverse = scope->first_child;
	while (traverse != NULL) {
		print_sym_tab_r(traverse);
		traverse = traverse->next;
	}
}

void print_sym_tab() {
	print_sym_tab_r(current_scope);
}

void newscope(char* scopename) {
	table_node* newscope = calloc(1, sizeof(table_node));
	if (current_scope != NULL) {
		if (current_scope->last_child == NULL) {
			current_scope->first_child = newscope;
			current_scope->last_child = newscope;
		}
		else {
			current_scope->last_child->next = newscope;
			current_scope->last_child = newscope;
		}
	}
	newscope->parent = current_scope;
	current_scope = newscope;
}
void upscope() {
	if (current_scope->parent == NULL) {
		printf("You shouldn\'t be here, current_scope is NULL in upscope\n");
		return;
	}
	current_scope = current_scope->parent;
}

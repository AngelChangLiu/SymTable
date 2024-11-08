/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Angel Chang Liu                                            */
/*--------------------------------------------------------------------*/

#ifndef SYMTABLE_included
#define SYMTABLE_included
#include <stddef.h>

/* A SymTable_T is a collection of key/value pairs */

typedef struct SymTable *SymTable_T;

/*--------------------------------------------------------------------*/

/* Returns a new SymTable object that contains no bindings, or NULL if 
insufficient memory is available */

SymTable_T SymTable_new(void);

/*--------------------------------------------------------------------*/

/* Frees all memory occupied by the input oSymTable */

void SymTable_free(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* Returns the number of bindings in the input oSymTable */

size_t SymTable_getLength(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* If the input oSymTable does not contain a binding with key pcKey, 
then SymTable_put adds a new binding to oSymTable consisting of key 
pcKey and value pvValue and returns 1 (TRUE). Otherwise the function 
must leave oSymTable unchanged and returns 0 (FALSE). If insufficient 
memory is available, then the function leaves oSymTable unchanged and 
returns 0 (FALSE). Inputs are SymTable_T oSymtable, const char *pcKey, 
const void *pvValue */

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue);

/*--------------------------------------------------------------------*/

/* If oSymTable contains a binding with key pcKey, then SymTable_replace 
replaces the binding's value with pvValue and returns the old value. 
Otherwise it leaves oSymTable unchanged and returns NULL. Inputs are 
SymTable_T oSymtable, const char *pcKey, const void *pvValue */

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue);

/*--------------------------------------------------------------------*/

/* SymTable_contains returns 1 (TRUE) if oSymTable contains a binding 
whose key is pcKey, and 0 (FALSE) otherwise. Inputs are SymTable_T 
oSymtable and const char *pcKey */

int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* Returns the value of the binding within oSymTable whose key is pcKey, 
or NULL if no such binding exists. Inputs are SymTable_T oSymtable and 
const char *pcKey. Inputs are SymTable_T oSymTable and  const char 
*pcKey */

void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* If oSymTable contains a binding with key pcKey, then SymTable_remove 
removs that binding from oSymTable and return the binding's value.
Otherwise the function does not change oSymTable and returns NULL. 
Inputs are SymTable_T oSymTable and const char *pcKey */

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/*--------------------------------------------------------------------*/

/* SymTable_map applies function *pfApply to each binding in oSymTable, 
passing pvExtra as an extra parameter. Inputs are SymTable_T oSymTable,
the function void (*pfApply)(const char *pcKey, void *pvValue, 
void *pvExtra) and const void *pvExtra */

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra);

/*--------------------------------------------------------------------*/

#endif
/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Angel Chang Liu                                            */
/*--------------------------------------------------------------------*/
 
#include "symtable.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Each key/value is stored in a Node. Nodes are linked to form a 
list */
struct Node {
   /* A string that uniquely identifies its binding */
   char *key; 
   /* Data that is somehow pertinent to its key */
   void* value;
   /* A node that links the current Node with the next Node */
   struct Node *psNextNode;
   }; 

/* Collection of key value pairs */
struct SymTable {
   /* Node that points to the first Node or the beginning of a list */
   struct Node *psFirstNode;
   /* Stores the number of bindings in a list */
   size_t length;
   };
 
/*--------------------------------------------------------------------*/
 
SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;

   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->psFirstNode = NULL;
   oSymTable->length = 0; 
   return oSymTable;

}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable) {
   struct Node *psCurrentNode;
   struct Node *psNextNode;

   assert(oSymTable != NULL);

   /* iterate through each node */
   for (psCurrentNode = oSymTable->psFirstNode;
      psCurrentNode != NULL;
      psCurrentNode = psNextNode) {
   
      psNextNode = psCurrentNode->psNextNode;
      free(psCurrentNode->key);
      free(psCurrentNode);
   }

   free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);

   return oSymTable->length;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue) {
   
   struct Node *psNewNode;
   struct Node *psCurrentNode;
   char *newKey;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* iterate */
   for (psCurrentNode = oSymTable->psFirstNode;
      psCurrentNode != NULL;
      psCurrentNode = psCurrentNode->psNextNode) {

      if (strcmp(psCurrentNode->key, pcKey) == 0) {
         return 0;
      }
   }

   /* allocate new space for the node and key and rebind */
   psNewNode = (struct Node*)malloc(sizeof(struct Node));
   if (psNewNode == NULL)
      return 0;
   psNewNode->psNextNode = oSymTable->psFirstNode;
   oSymTable->psFirstNode = psNewNode;

   newKey = (char*)malloc(strlen(pcKey) + 1);
   if (newKey == NULL) {
      free(psNewNode);
      return 0;
   }
   strcpy(newKey, pcKey);
   oSymTable->length++;

   psNewNode->key = newKey;

   psNewNode->value = (void*)pvValue;

   return 1;
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue) {
   
   void* temp;
   struct Node *psCurrentNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* iterate, find, replace*/
   for (psCurrentNode = oSymTable->psFirstNode;
      psCurrentNode != NULL;
      psCurrentNode = psCurrentNode->psNextNode) {

      if (strcmp(psCurrentNode->key, pcKey) == 0) {
         temp = psCurrentNode->value;
         psCurrentNode->value = (void*)pvValue;
         return temp;
      }

   }
   return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
   
   struct Node *psCurrentNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* iterate and find match */
   for (psCurrentNode = oSymTable->psFirstNode;
      psCurrentNode != NULL;
      psCurrentNode = psCurrentNode->psNextNode) {

      if (strcmp(psCurrentNode->key, pcKey) == 0) {
         return 1;
      }
   }
   return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

   struct Node *psCurrentNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   /* iterate and return value when found */
   for (psCurrentNode = oSymTable->psFirstNode;
      psCurrentNode != NULL;
      psCurrentNode = psCurrentNode->psNextNode) {

      if (strcmp(psCurrentNode->key, pcKey) == 0) {
         return psCurrentNode->value;
      }
   }

   return NULL;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

   void* temp;
   struct Node *psCurrentNode;
   struct Node *psPreviousNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   /* checks first node */
   psPreviousNode = oSymTable->psFirstNode;
   if (psPreviousNode == NULL) {
      return NULL;
   }

   if (strcmp(psPreviousNode->key, pcKey) == 0) {
         temp = psPreviousNode->value;
         oSymTable->psFirstNode = psPreviousNode->psNextNode;

         free(psPreviousNode->key);
         free(psPreviousNode);
         oSymTable->length--;
         return temp;
      }

   /* iterates through the rest of the nodes */
   for (psCurrentNode = oSymTable->psFirstNode->psNextNode;
      psCurrentNode != NULL;
      psCurrentNode = psCurrentNode->psNextNode) {
      
      if (strcmp(psCurrentNode->key, pcKey) == 0) {
         temp = psCurrentNode->value;
         psPreviousNode->psNextNode = psCurrentNode->psNextNode;

         free(psCurrentNode->key);
         free(psCurrentNode);
         oSymTable->length--;
         return temp;
      }

      psPreviousNode = psPreviousNode->psNextNode;
   }

   return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra) {
   
   struct Node *psCurrentNode;
   const char *key;
   void* value;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   for (psCurrentNode = oSymTable->psFirstNode;
      psCurrentNode != NULL;
      psCurrentNode = psCurrentNode->psNextNode) {
         
      key = psCurrentNode->key;
      value = psCurrentNode->value;
      /* applies the function to each binding */
      (*pfApply)(key, value, (void*)pvExtra);
   }
}

/*--------------------------------------------------------------------*/
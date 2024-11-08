/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Angel Chang Liu                                            */
/*--------------------------------------------------------------------*/
 
#include "symtable.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* valid sizes of auBucketCounts */
static const size_t auBucketCounts[] = {509, 1021, 2039, 4093, 8191, 
   16381, 32749, 65521}; 

/* Each key/value is stored in a Binding. Bindings are linked to form a 
SymTable */
struct Binding { 
   /* A string that uniquely identifies its binding */
   char *key; 
   /* Data that is somehow pertinent to its key */
   const void* value; 
   /* A node that links the current Node with the next Node */
   struct Binding *psNextBinding; 
}; 

/* Collection of key value pairs */
struct SymTable { 
   /* Binding that is a pointer to the first index of the array which
   is a pointer to the first Binding or the beginning of a list */
   struct Binding **buckets; 
   /* Stores the number of bindings in a list */
   size_t numBindings;
   /* Stores the number of buckets (array size)*/
   size_t numBucketCounts;
   /* keeps track of bucket value
   int bucketIndex; */
};

/*--------------------------------------------------------------------*/

/*
static void expander(SymTable_T oSymTable) {

   struct Binding *psCurrentBinding;
   size_t i;
   size_t newHash;
   struct Binding **newBuckets;

   assert(oSymTable != NULL);

   comment: go to next bucket size number
   oSymTable->bucketIndex++;
   oSymTable->numBucketCounts = auBucketCounts[oSymTable->bucketIndex];

   comment: Allocate new memory
   newBuckets = (struct Binding**)calloc(oSymTable->numBucketCounts, 
      sizeof(struct Binding*));
   if (oSymTable->buckets == NULL) {
      free(oSymTable);
      return NULL;
   }



   comment: Rehash all objects
   for (i = 0; i < oSymTable->numBucketCounts; i++) {
      psCurrentBinding = oSymTable->buckets[i];
      oSymTable->buckets = newBuckets;
     
      while (psCurrentBinding != NULL) {
         newHash = SymTable_hash(psCurrentBinding->key, 
            oSymTable->numBucketCounts);
         newBuckets[newHash] = psCurrentBinding;
         psCurrentBinding = psCurrentBinding->psNextBinding;

      }
   }

   free(oSymTable);
}

*/

/*--------------------------------------------------------------------*/


/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/
 
SymTable_T SymTable_new(void) {
   SymTable_T oSymTable;

   /* allocate new memory */
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;
      
   oSymTable->numBucketCounts = auBucketCounts[0];

   /* allocate new memory */
   oSymTable->buckets = (struct Binding**)calloc(oSymTable->
      numBucketCounts, sizeof(struct Binding*));
   if (oSymTable->buckets == NULL) {
      free(oSymTable);
      return NULL;
   }

   /* comment: for expander
   oSymTable->bucketIndex = 0; */
   oSymTable->numBindings = 0;

   return oSymTable;

}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable) {
   struct Binding *psCurrentBinding;
   struct Binding *psNextBinding;
   /* index to iterate through buckets */
   size_t i;

   assert(oSymTable != NULL);

   /* iterate through all bindings */
   for (i = 0; i < oSymTable->numBucketCounts; i++) {
      psCurrentBinding = oSymTable->buckets[i];
      while (psCurrentBinding != NULL) {
         psNextBinding = psCurrentBinding->psNextBinding;
         free(psCurrentBinding->key);
         free(psCurrentBinding);
         psCurrentBinding = psNextBinding;
      }
   }
   free(oSymTable->buckets);
   free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable) {
   assert(oSymTable != NULL);

   return oSymTable->numBindings;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue) {
   
   struct Binding *psNewBinding;
   struct Binding *psCurrentBinding;
   size_t hash;
   char *newKey;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->numBucketCounts);

   /* check if present already */
   for (psCurrentBinding = oSymTable->buckets[hash];  
      psCurrentBinding != NULL;
      psCurrentBinding = psCurrentBinding->psNextBinding) {

      if (strcmp(psCurrentBinding->key, pcKey) == 0) {
         return 0;
      }
   }

   /* calls expander if max numBucketCount reached
   if (oSymTable->bucketIndex > oSymTable->numBucketCounts) {
      expander(oSymTable);
   }
   */

  /* allocating new memory and rebinding */
   newKey = (char*)malloc(strlen(pcKey) + 1);
   if (newKey == NULL)
         return 0;
   strcpy(newKey, pcKey);

   psNewBinding = (struct Binding*)malloc(sizeof(struct Binding));
   if (psNewBinding == NULL) {
         free(newKey);
         return 0;
   }
   psNewBinding->psNextBinding = oSymTable->buckets[hash];
   oSymTable->buckets[hash] = psNewBinding;

   psNewBinding->key = newKey;

   psNewBinding->value = (void*)pvValue;

   oSymTable->numBindings++;

   /* comment: for expander to keep track of where in the bucket it is
   oSymTable->bucketIndex++; */

   return 1;
}


/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable,
   const char *pcKey, const void *pvValue) {
   
   void* temp;
   struct Binding *psCurrentBinding;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);
   
   /* iterate to find */
   for (psCurrentBinding = oSymTable->buckets[(SymTable_hash(pcKey, 
      oSymTable->numBucketCounts))];
      psCurrentBinding != NULL;
      psCurrentBinding = psCurrentBinding->psNextBinding) {
      /* if found, replace */
      if (strcmp(psCurrentBinding->key, pcKey) == 0) {
         temp = (void*)psCurrentBinding->value;
         psCurrentBinding->value = pvValue;
         return temp;
      }
   }
   return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey) {
   
   struct Binding *psCurrentBinding;

   assert (oSymTable != NULL);
   assert (pcKey != NULL);

   oSymTable->numBucketCounts = auBucketCounts[0];

   /* check if present through iteration */
   for (psCurrentBinding = oSymTable->buckets[(SymTable_hash(pcKey, 
      oSymTable->numBucketCounts))];
      psCurrentBinding != NULL;
      psCurrentBinding = psCurrentBinding->psNextBinding) {

      if (strcmp(psCurrentBinding->key, pcKey) == 0) {
         return 1;
      }
   }
      return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey) {

   struct Binding *psCurrentBinding;

   assert (oSymTable != NULL);
   assert (pcKey != NULL);

   oSymTable->numBucketCounts = auBucketCounts[0];

   /* iterate to find the correct binding */
   for (psCurrentBinding = oSymTable->buckets[(SymTable_hash(pcKey, 
      oSymTable->numBucketCounts))];
      psCurrentBinding != NULL;
      psCurrentBinding = psCurrentBinding->psNextBinding) {

      if (strcmp(psCurrentBinding->key, pcKey) == 0) {
         return (void*)psCurrentBinding->value;
      }
   }
   return NULL;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey) {

   void* temp;
   struct Binding *psCurrentBinding;
   struct Binding *psPreviousBinding;
   size_t hash;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash = SymTable_hash(pcKey, oSymTable->numBucketCounts);
   psPreviousBinding = NULL;

   /* checks if present */
   for (psCurrentBinding = oSymTable->buckets[hash];  
      psCurrentBinding != NULL;
      psCurrentBinding = psCurrentBinding->psNextBinding) {

      if (strcmp(psCurrentBinding->key, pcKey) == 0) {
         /* save old value, relink list, decrease count, return old 
            value */
         temp = (void*)psCurrentBinding->value;
         if (psPreviousBinding == NULL) {
            oSymTable->buckets[hash] = psCurrentBinding->psNextBinding;
         }
         else {
            psPreviousBinding->psNextBinding = psCurrentBinding->
               psNextBinding;
         }

         free(psCurrentBinding->key);
         free(psCurrentBinding);

         oSymTable->numBindings--;

         return temp;
      }

      psPreviousBinding = psCurrentBinding;
   }

   return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
   void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
   const void *pvExtra) {
   
   struct Binding *psCurrentBinding;
   size_t i;

   const char *key;
   void* value;

   assert(oSymTable != NULL);
   assert(pfApply != NULL);

   oSymTable->numBucketCounts = auBucketCounts[0];

   for (i = 0; i < oSymTable->numBucketCounts; i++) {
      psCurrentBinding = oSymTable->buckets[i];
      while (psCurrentBinding != NULL) {
         key = psCurrentBinding->key;
         value = (void*)psCurrentBinding->value;
         /* apply the function on each binding */
         (*pfApply)(key, value, (void*)pvExtra);
         psCurrentBinding = psCurrentBinding->psNextBinding;
      }
   }
}

/*--------------------------------------------------------------------*/
/* Hash tables
 * 15-122 Principles of Imperative Computation, Fall 2010
 * Frank Pfenning
 */
#ifndef _HASHTABLE_H
#define _HASHTABLE_H

typedef void* ht_key;
typedef void* ht_elem;
 
/* Hash table interface */
typedef struct table* tableptr;
tableptr table_new (int init_size,
		 ht_key (*elem_key)(ht_elem e),
		 bool (*equal)(ht_key k1, ht_key k2),
		 int (*hash)(ht_key k, int m)
		 );
ht_elem table_insert(tableptr H, ht_elem e);
ht_elem table_search(tableptr H, ht_key k);
/* ht_elem table_delete(tableptr H, ht_key k); */
void table_free(tableptr H, void (*elem_free)(ht_elem e));

#endif

/* ROBBD implementation
 * 15-122 Principles of Imperative Computation, Fall 2010
 * Frank Pfenning
 */

#ifndef _ROBDD_H_
#define _ROBDD_H_
#include <vector>
#include <iostream>
extern "C"
{
#include "xalloc.h"
#include "contracts.h"
#include "hashtable.h"
}

using namespace std;

typedef int bdd_node;
struct node
{
    int var;       /* variables v, v1, v2, ... */
    bdd_node low;  /* low successor */
    bdd_node high; /* high successor */
};
class bdd
{
public:
    int num_vars;     /* variables 1 <= v <= num_vars */
    int limit;        /* limit = \length(T) */
    int size;         /* 0 <= size < limit */
    node** T; /* node array */
    tableptr H;       /* mapping a = (var,low,high) to u, where T[u] = a */
    void bdd_free();
    void allsat(bdd_node u);
    int bdd_size(); /* total number of nodes */
    void onesat(bdd_node u);
    int satcount(bdd_node u);
};
bdd * bdd_new(int k); /* k variables */
bdd_node make(bdd * B, int var, bdd_node low, bdd_node high);
bdd_node apply(bdd * B, int (*func)(int b1, int b2), bdd_node u1, bdd_node u2);
bool is_bdd(bdd * B);
int my_and(int b1, int b2);
int my_or(int b1, int b2);
int my_xor(int b1, int b2);

#endif

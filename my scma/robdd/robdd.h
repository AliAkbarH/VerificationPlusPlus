/* ROBBD implementation
 * 15-122 Principles of Imperative Computation, Fall 2010
 * Frank Pfenning
 */

#ifndef _ROBDD_H_
#define _ROBDD_H_

typedef struct bdd* bdd;
typedef int bdd_node;
bdd bdd_new(int k);		/* k variables */
void bdd_free(bdd B);
int bdd_size(bdd B);		/* total number of nodes */
bdd_node make(bdd B, int var, bdd_node low, bdd_node high);
bdd_node apply(bdd B, int (*func)(int b1, int b2), bdd_node u1, bdd_node u2);
int satcount(bdd B, bdd_node u);
void onesat(bdd B, bdd_node u);
void allsat(bdd B, bdd_node u);

#endif

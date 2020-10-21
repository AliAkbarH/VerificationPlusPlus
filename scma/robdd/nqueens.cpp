/* n-queens problem, counting solutions
 * 15-122 Principles of Imperative Computation, Fall 2010
 * Frank Pfenning
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "robdd.h"

/* boolean functions, on integers 0 my_and 1 */


int imp(int b1, int b2) {
  return b1 == 1 ? b2 : 1;
}

int nqueens(bdd* B, int n) {
  int i, j, k;
  bdd_node u, r, not_x, x;
  r = 1;

  /* each column has a queen */
  printf("each column has a queen \n");
  for (i = 0; i < n; i++) {
    u = 0;			/* false */
    for (j = 0; j < n; j++) {
      x = make(B, i+j*n+1, 0, 1); /* x_ij */
      printf("x=%d\n", x);
      u = apply(B, &my_or, u, x);
      printf("u=%d\n", u);

    }
    r = apply(B, &my_and, r, u);
    printf("r=%d\n", r);
  }

  /* no two queens in the same column */
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      u = 1;			/* true */
      for (k = 0; k < n; k++) {
	if (k != j) {
	  not_x = make(B, i+k*n+1, 1, 0); /* x_ik */
	  u = apply(B, &my_and, not_x, u);
	}
      }
      x = make(B, i+j*n+1, 0, 1); /* x_ij */
      u = apply(B, &imp, x, u);
      r = apply(B, &my_and, r, u);
    }
  }

  /* no two queens in the same row */
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      u = 1;
      for (k = 0; k < n; k++) {
        if (k != i) {
          not_x = make(B, k+j*n+1, 1, 0); /* x_kj */
          u = apply(B, &my_and, not_x, u);
        }
      }
      x = make(B, i+j*n+1, 0, 1); /* x_ij */
      u = apply(B, &imp, x, u);
      r = apply(B, &my_and, r, u);
    }
  }

  /* no two queens on same up diagonal */
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      u = 1;
      for (k = -i; k < n-i; k++) {
	if (k != 0 && 0 <= j+k && j+k < n) {
	  not_x = make(B, (i+k)+(j+k)*n+1, 1, 0); /* x_(i+k)(j+k) */
	  u = apply(B, &my_and, not_x, u);
	}
      }
      x = make(B, i+j*n+1, 0, 1); /* x_ij */
      u = apply(B, &imp, x, u);
      r = apply(B, &my_and, r, u);
    }
  }

  /* no two queens on same down diagonal */
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      u = 1;
      for (k = -i; k < n-i; k++) {
	if (k != 0 && 0 <= j-k && j-k < n) {
	  not_x = make(B, (i+k)+(j-k)*n+1, 1, 0); /* x_(i+k)(j-k) */
	  u = apply(B, &my_and, not_x, u);
	}
      }
      x = make(B, i+j*n+1, 0, 1); /* x_ij */
      u = apply(B, &imp, x, u);
      r = apply(B, &my_and, r, u);
    }
  }

  return r;
}

// int main() {
//   int n = 8;			/* board of size n*n */
//   bdd* B = bdd_new(n*n);		/* bdd B with n*n variables */
//   int r = nqueens(B,n);		/* r = bdd representing n-queens constraints */
//   printf("Final bdd size: %d nodes\n", B->bdd_size());
//   printf("r: %d \n", r);
//   B->onesat( r);
//   // allsat(B, r);		     /* print all solutions in compact form */
//   printf("%d solutions\n", B->satcount(r));
//   B->bdd_free();			/* for good memory hygiene */
//   return 0;
// }

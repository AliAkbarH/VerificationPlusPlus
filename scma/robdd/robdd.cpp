/* ROBDDs
 * 15-122 Principles of Imperative Computation, Fall 2010
 * Frank Pfenning
 * Following a BDD tutorial by Henrik Reif Andersen
 * This uses fixed-size hash tables, which is problematic.
 * Students are busy implementing adaptive hash tables in
 * their Assignment 7.
 * For illustration purposes, we use different schemes for
 * keys and elements of the hash table.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include "robdd.h"
#define BDD_LIMIT (1 << 20)
#define BDD_HASHTABLE_SIZE (1 << 18)
#define APPLY_HASHTABLE_SIZE (1 << 16)
#define SATCOUNT_HASHTABLE_SIZE (1 << 10)

/* bdd nodes are represented as integers
 * They act as pointers, except for 0 = false
 * and 1 = true.  Pointers would be cleaner,
 * but the coding of true and false would be
 * more difficult, as would be hashing.
 */
typedef int bdd_node; /* bdd_node u, u1, u2, ... */
// typedef struct node* nodeptr;	/* node a, a1, a2, ... */
// struct node {
//   int var;			/* variables v, v1, v2, ... */
//   bdd_node low;			/* low successor */
//   bdd_node high;		/* high successor */
// };

/* hash table elements */
// typedef struct elem* elemptr;
struct elem
{
  node *e_node; /* key = node a */
  bdd_node u;   /* data = u where T[u] = a */
};

void elem_free(ht_elem e)
{
  /* don't free e->node - it is owned by the bdd array T */
  free((elem *)e);
}

/* hash table functions */
/* table: ht_key = void*, ht_elem = void* */
/* bdd:   ht_key = node, ht_elem = elem */

/* key comparison = node comparison */
bool equal(ht_key k1, ht_key k2)
{
  node *a1 = (node *)k1; /* cast from void* */
  node *a2 = (node *)k2; /* cast from void* */
  return a1->var == a2->var && a1->low == a2->low && a1->high == a2->high;
}

/* extracting keys from elements */
ht_key elem_key(ht_elem e)
//@requires e != NULL;
{
  return ((elem *)e)->e_node;
}

/* hash function */
/* uses pseudo-random number generation */
/* use unsigned int in C to guarantee modular arithmetic */
int my_hash(ht_key k, int m)
{
  REQUIRES(m > 1);
  node *a = (node *)k; /* cast from void* */
  unsigned int x = 1664525;
  unsigned int y = 1013904223; /* inlined random number generator */
  unsigned int r = 0xdeadbeef; /* initial seed */
  unsigned int h = (unsigned)a->var;
  r = r * x + y; /* mod 2^32, linear congruential random num */
  h = r * h + (unsigned)a->low;
  r = r * x + y; /* mod 2^32, linear congruential random num */
  h = r * h + (unsigned)a->high;
  h = h % (unsigned)m; /* reduce to range */
  ENSURES(0 <= (int)h && (int)h < m);
  return (int)h;
}

/* checking if a bdd is valid */
/* this does not check if the bdd is fully reduced */
bool is_bdd(bdd *B)
//@requires B->limit == \length(B->T);
{
  if (!(0 <= B->size && B->size < B->limit))
    return false;
  node **T = B->T;
  int k = B->num_vars; /* k = number of variables */
  if (k > INT_MAX - 1)
    return false;
  int n = B->size; /* n = number of nodes */
  elem *e;
  int i;
  for (i = 0; i < B->size; i++)
  {
    node *a = T[i];
    int v = a->var;
    if (!((i == 0 && v == k + 1)            /* represents 0; var field is k+1 */
          || (i == 1 && v == k + 1)         /* represents 1; var field is k+1 */
          || (i >= 2 && 1 <= v && v <= k))) /* other vars in [1..k] */
      return false;
    if (i >= 2)
    {                                 /* nodes besides 0 and 1 */
      if (!(0 <= a->low && a->low < n /* low and high in range */
            && 0 <= a->high && a->high < n && T[a->low] != NULL && T[a->high] != NULL
            /* var index must increase */
            && T[a->low]->var > v && T[a->high]->var > v))
        return false;
      /* check hash table for correctness */
      e = (elem *)table_search(B->H, a);
      if (!(e != NULL && elem_key(e) == a && T[e->u] == a))
        return false;
    }
  }
  return true;
}

/* bdd_new(k) creates a new bdd of k variables */
bdd *bdd_new(int k)
{
  assert(0 <= k && k < INT_MAX - 1);
  bdd *B = (bdd *)xmalloc(sizeof(struct bdd));
  B->num_vars = k;
  B->limit = BDD_LIMIT;
  B->size = 2;
  {
    /* allocating array, initially only with 0 and 1 */
    node **T = (node **)xcalloc(B->limit, sizeof(node *));
    // T.reserve(B->limit);
    node *zero = (node *)xmalloc(sizeof(struct node));
    node *one = (node *)xmalloc(sizeof(struct node));
    zero->var = k + 1;
    one->var = k + 1;
    /* low and high for zero and one are irrelevant */
    T[0] = zero;
    T[1] = one;
    B->T = T;
  }
  B->H = table_new(BDD_HASHTABLE_SIZE,
                   &elem_key, &equal, &my_hash);
  ENSURES(is_bdd(B));
  return B;
}

void bdd::bdd_free()
{
  REQUIRES(is_bdd(this));
  for (int i = 0; i < this->size; i++)
    free(this->T[i]); /* free all nodes */
  free(this->T);      /* free node array */
  /* free node array */
  // for (int i = 0; i < this->T.size(); i++)
  // {
  //   delete this->T[i];
  // }
  // this->T.clear();

  table_free(this->H, &elem_free); /* free hash table */
  // free(this);                      /* free bdd */
}

/* bdd_size(B) = n, the current number of nodes */
int bdd::bdd_size()
{
  REQUIRES(is_bdd(B));
  return this->size;
}

/* make(B, v, low, high) creates a node (v, low, high)
 * or returns it, in case it already exists in B.
 * Also avoids creating a node if the test is redundant
 * (low = high).
 */
bdd_node make(bdd *B, int var, int low, int high)
{
  REQUIRES(is_bdd(B));
  assert(1 <= var && var <= B->num_vars);
  assert(0 <= low && low < B->size);
  assert(0 <= high && high < B->size);
  node *a;
  elem *e;
  int u;
  if (low == high)
    return low; /* don't create, if test redundant */
  /* create new node */
  a = (node *)xmalloc(sizeof(struct node *));
  a->var = var;
  a->low = low;
  a->high = high;
  /* search for it in the table */
  e = (elem *)table_search(B->H, a);
  if (e != NULL)
  {
    free(a);     /* free temp element */
    return e->u; /* return if found */
  }
  if (!(B->size < B->limit))
  {
    fprintf(stderr, "ran out of space in BDD array of size %d\n", B->limit);
    abort();
  }
  /* enter new node into the bdd */
  u = B->size;
  B->T[u] = a;
  B->size++;
  /* enter into hash table */
  e = (elem *)xmalloc(sizeof(struct elem));
  e->e_node = a;
  e->u = u;
  table_insert(B->H, e);
  ENSURES(is_bdd(B));
  ENSURES(0 <= u && u < B->size);
  return u;
}

/* hash table for binary "apply" operation
 * to implemented dynamic programming, giving
 * a table representation of the function
 */
struct apfun
{
  bdd_node u1;
  bdd_node u2;
  bdd_node u12; /* u12 = apply(f, u1, u2) */
};
// typedef struct apfun* apfun;

void apfun_free(ht_elem ap)
{
  free((apfun *)ap);
}

/* key = elem = void* in the table
 * key = elem = apfun in the bdd
 */
ht_key ap_key(ht_elem ap)
{
  return ap;
}

/* key equality compares inputs, ignore output */
bool ap_equal(ht_key ap1, ht_key ap2)
{
  return ((apfun *)ap1)->u1 == ((apfun *)ap2)->u1 && ((apfun *)ap1)->u2 == ((apfun *)ap2)->u2;
}

/* hash functions uses pseudorandom numbers */
int ap_hash(ht_key ap, int m)
{
  REQUIRES(m > 0);
  unsigned int x = 1664525;
  unsigned int y = 1013904223;
  unsigned int r = 0xdeadbeef;
  unsigned int h = (unsigned)((apfun *)ap)->u1;
  r = r * x + y;
  h = r * h + (unsigned)((apfun *)ap)->u2;
  h = h % (unsigned)m;
  return (int)h;
}

/* apply_rec(B, f, u1, u2, S) = f on (u1, u2) in B
 * using table S for memoization.  f should be
 * a boolean function, where bools are encoded as ints 0 and 1
 */
bdd_node apply_rec(bdd *B, int (*func)(int b1, int b2),
                   bdd_node u1, bdd_node u2, tableptr S)
{ // REQUIRES(is_bdd(B));
  REQUIRES(0 <= u1 && u1 < B->size);
  REQUIRES(0 <= u2 && u2 < B->size);
  REQUIRES(func != NULL);
  int u;
  if (u1 <= 1 && u2 <= 1)
    return (*func)(u1, u2);
  else
  {
    /* construct new table entry for (u1, u2) */
    apfun *ap0 = (apfun *)xmalloc(sizeof(struct apfun *));
    ap0->u1 = u1;
    ap0->u2 = u2; // ap0->u12 is irrelevant here
    apfun *ap = (apfun *)table_search(S, ap0);
    /* if already computed, return stored result */
    if (ap != NULL)
    {
      free(ap0);
      return ap->u12;
    }
    /* not already computed, apply function */
    int v1 = (B->T[u1])->var;
    int v2 = (B->T[u2])->var;
    if (v1 == v2)
      u = make(B, v1,
               apply_rec(B, func, (B->T[u1])->low, (B->T[u2])->low, S),
               apply_rec(B, func, (B->T[u1])->high, (B->T[u2])->high, S));
    else if (v1 < v2)
      u = make(B, v1,
               apply_rec(B, func, (B->T[u1])->low, u2, S),
               apply_rec(B, func, (B->T[u1])->high, u2, S));
    else /*@assert v1 > v2;@*/
      u = make(B, v2,
               apply_rec(B, func, u1, (B->T[u2])->low, S),
               apply_rec(B, func, u1, (B->T[u2])->high, S));
    ap0->u12 = u;
    table_insert(S, ap0);
    ENSURES(is_bdd(B));
    ENSURES(0 <= u && u < B->size);
    return u;
  }
}

/* apply(B, f, u1, u2) = f on (u1, u2) in B
 * using a table for memoization.  f should be
 * a boolean function, where bools are encoded as ints 0 and 1
 */
bdd_node apply(bdd *B, int (*func)(int b1, int b2),
               bdd_node u1, bdd_node u2)
{
  REQUIRES(is_bdd(B));
  assert(0 <= u1 && u1 < B->size);
  assert(0 <= u2 && u2 < B->size);
  tableptr S = table_new(APPLY_HASHTABLE_SIZE,
                         &ap_key, &ap_equal, &ap_hash);
  int u = apply_rec(B, func, u1, u2, S);
  table_free(S, &apfun_free);
  return u;
}

/* hash table for satcount computation */
struct satc
{
  bdd_node u;
  int count;
};
// typedef struct satc* satc;

/* key = void*, elem = void* for hashtable,
 * key = bdd_node*, elem = satc for for bdd
 * illustrates the use of address-of (&)
 */
ht_key satc_key(ht_elem sc)
{
  return &(((satc *)sc)->u);
}

bool satc_equal(ht_key k1, ht_key k2)
{
  return *(bdd_node *)k1 == *(bdd_node *)k2; /* cast from void */
}

int satc_hash(ht_key k, int m)
{
  REQUIRES(*(bdd_node *)k >= 0 && m > 0);
  return *(int *)k % m;
}

void satc_free(ht_elem sc)
{
  free((satc *)sc); /* cast is redundant */
}

int safe_shiftl(int n, int k)
{
  REQUIRES(n >= 0 && k >= 0);
  if ((n > 0 && k >= (int)sizeof(int) * 8) || n > (INT_MAX >> k))
  {
    fprintf(stderr, "integer overflow %d << %d\n", n, k);
    abort();
  }
  return n << k;
}

int safe_plus(int m, int n)
{
  REQUIRES(m >= 0 && n >= 0);
  if (n > INT_MAX - m)
  {
    fprintf(stderr, "integer overflow %d + %d\n", m, n);
    abort();
  }
  return m + n;
}

/* satcount_rec(B, u, H) = r, the number
 * of satisfying assignments for u in B,
 * memoizing in table H
 */
int satcount_rec(bdd *B, int u, tableptr H)
//@requires is_bdd(B);
{
  assert(0 <= u && u < B->size);
  if (u == 0)
    return 0;
  else if (u == 1)
    return 1;
  else
  {
    satc *sc = (satc *)table_search(H, &u); /* check table for count */
    if (sc != NULL)
      return sc->count; /* found, return count */
    node *a = B->T[u];
    int v = a->var;
    int low = a->low;
    int high = a->high;
    /* next two lines account for omitted redundant tests via */
    /* multiplication with 2^m, where m is number of omitted tests */
    int count_low = safe_shiftl(satcount_rec(B, low, H),
                                (B->T[low])->var - v - 1); /* could overflow */
    int count_high = safe_shiftl(satcount_rec(B, high, H),
                                 (B->T[high])->var - v - 1); /* could overflow */
    int count = safe_plus(count_low, count_high);            /* could overflow */
    /* insert count into table before returning */
    sc = (satc *)xmalloc(sizeof(struct satc *));
    sc->u = u;
    sc->count = count;
    table_insert(H, sc);
    return count;
  }
}

/* satcount_rec(B, u) = r, the number
 * of satisfying assignments for u in B
 */
int bdd::satcount(int u)
{
  REQUIRES(is_bdd(B));
  assert(0 <= u && u < this->size);

  tableptr H = table_new(SATCOUNT_HASHTABLE_SIZE,
                         &satc_key, &satc_equal, &satc_hash);
  // std::cout << "something"<<(this->T[u])->var - 1 << endl;
  int num = safe_shiftl(satcount_rec(this, u, H), (this->T[u])->var - 1); /* could overflow */
  // std::cout << "mohammad robdd" << endl;
  table_free(H, &satc_free);
  return num;
}

/* onesat(B, u) prints one satisfying assignment for node u in B */
void bdd::onesat(int u)
{
  REQUIRES(is_bdd(B));
  assert(0 <= u && u < this->size);
  assert(u != 0); /* bdd not satisfiable! */
  if (u == 0)
    printf("No solutions\n");
  else
  {
    node *a = this->T[u];
    int v = a->var;
    while (v <= this->num_vars)
    {
      printf("x[%d]=", v);
      if (a->low != 0)
      {
        printf("0\n");
        u = a->low;
      }
      else
      {
        printf("1\n");
        u = a->high;
      }
      a = this->T[u];
      v = a->var;
    }
  }
}

/* compact printing of all satisfying assignments
 * obviously, this can exponential even more easily
 * than other bdd functions, so use with care
 */
void parray(int *A, int level)
{
  for (int i = 1; i < level; i++)
    if (A[i] < 0)
      printf("."); /* '.' for redundant test */
    else
      printf("%d", A[i]);
  printf("=1\n");
}

void allsat_rec(bdd *B, int *A, int level, int u)
{                               // REQUIRES(is_bdd(B));
  assert(0 < u && u < B->size); /* u != 0 (false) */
  node *a = B->T[u];
  int v = a->var;
  while (level < v)
  {
    A[level] = -1; /* mark redundant tests as -1 */
    level++;
  }
  ASSERT(level == v);
  if (u == 1)
  {
    parray(A, level);
    return;
  }
  if (a->low != 0)
  {
    A[v] = 0;
    allsat_rec(B, A, v + 1, a->low);
  }
  if (a->high != 0)
  {
    A[v] = 1;
    allsat_rec(B, A, v + 1, a->high);
  }
  return;
}

/* print all satisfying assignments in compact form
 * x1 x2 x3 ... xk = 1 for xi = 0 or xi = 1
 * or xi = '.' (which means variable is arbitrary)
 */
void bdd::allsat(int u)
{
  REQUIRES(is_bdd(this));
  assert(0 <= u && u < this->size);
  if (u == 0)
  {
    printf("no solutions");
  }
  else
  {
    int *A = (int *)xcalloc(this->num_vars + 1, sizeof(int));
    allsat_rec(this, A, 1, u);
    free(A);
  }
}

//mohammad
int my_and(int b1, int b2)
{
  return b1 & b2;
}

int my_or(int b1, int b2)
{
  return b1 | b2;
}

int my_xor(int b1, int b2)
{
  return b1 ^ b2;
}
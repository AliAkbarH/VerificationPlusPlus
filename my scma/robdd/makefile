# call with make D='-DDEBUG -g' for debug mode
FLAGS=-Wall -Wextra -std=c99 -pedantic -Werror
# D=-DDEBUG -g
GCC=gcc ${D} ${FLAGS}

default: stacks

stacks : contracts.h xalloc.h xalloc.c stacks.h stacks.c stacks-test.c
	$(GCC) xalloc.c stacks.c stacks-test.c -o stacks

hashtable : contracts.h xalloc.h xalloc.c hashtable.h hashtable.c hashtable-test.c
	$(GCC) xalloc.c hashtable.c hashtable-test.c -o hashtable

robdd : contracts.h xalloc.h xalloc.c hashtable.h hashtable.c robdd.h robdd.c robdd-test.c
	$(GCC) xalloc.c hashtable.c robdd.c robdd-test.c -o robdd

nqueens : contracts.h xalloc.h xalloc.c hashtable.h hashtable.c robdd.h robdd.c nqueens.c
	$(GCC) xalloc.c hashtable.c robdd.c nqueens.c -o nqueens

clean :
	${RM} -rf a.out a.out.dSYM
	${RM} -rf stacks stacks.dSYM
	${RM} -rf hashtable hashtable.dSYM
	${RM} -rf robdd robdd.dSYM
	${RM} -rf nqueens nqueens.dSYM

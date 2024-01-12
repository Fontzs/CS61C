#include "bit_ops.h"

#include <stdio.h>

/* Returns the Nth bit of X. Assumes 0 <= N <= 31. */
unsigned get_bit(unsigned x, unsigned n) { return (x >> n) & 1; }

/* Set the nth bit of the value of x to v. Assumes 0 <= N <= 31, and V is 0 or 1
 */
void set_bit(unsigned *x, unsigned n, unsigned v) {
  unsigned mask = 1 << n;
  *x = (*x & ~mask) | (v << n);
}

/* Flips the Nth bit in X. Assumes 0 <= N <= 31.*/
void flip_bit(unsigned *x, unsigned n) {
  unsigned bit = get_bit(*x, n);
  set_bit(x, n, !bit);
}

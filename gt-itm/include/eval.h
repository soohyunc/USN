/* $Id: eval.h 1 2009-10-11 22:06:03Z soohyunc $
 *
 * eval.h -- header file for evaluation routines
 *
 */
#define mdist   u.I

enum Field {Len, A, B, Hops};

void twofield_sptree(Graph*, Vertex*, enum Field, enum Field);


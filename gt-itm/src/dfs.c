/* $Id: dfs.c 1 2009-10-11 22:06:03Z soohyunc $
 *
 * dfs.c -- simple dfs program and routine that uses dfs to check connectivity
 *
 * Uses the Stanford GraphBase tools.
 */

#include <stdio.h>
#ifndef FBSD
#include <alloca.h>
#endif
#include <sys/types.h>
#include "gb_graph.h"
#include "math.h"
#include "geog.h"

#define STACKMAX	0x600

static char dfsID[]="$Id: dfs.c 1 2009-10-11 22:06:03Z soohyunc $";

#define NBBY 8

/* check connectivity of graph g                        */
/* uses depth-first search.                             */
isconnected(Graph *G)
{
u_char *vis;
Vertex *vp;
int i,nbytes;

    nbytes = (G->n/NBBY)+ (G->n%NBBY?1:0);

    if (nbytes < STACKMAX) {	/* for small amounts we use stack frame */
        vis = (u_char *) alloca(nbytes);
    } else {
        vis = (u_char *) malloc(nbytes);
    }

    for (i=0; i<nbytes; i++) vis[i]=0;

    i =  (dfs(G,0,vis)==G->n);

    if (nbytes >= STACKMAX) free(vis);

    return i;

}

#define bitset(v,i)     (v[(i)/NBBY]&(1<<((i)%NBBY)))
#define setbit(v,i)     v[(i)/NBBY] |= (1<<((i)%NBBY))


/* trivial depth-first search. */
/* Returns number of newly-visited nodes in subtree rooted at node n. */
int
dfs(Graph *G,int n,u_char *vis)
{
int nvis, i, nextn;
Vertex *vp;
Arc *ap;


    setbit(vis,n);       /* mark this node as visited */
    vp = G->vertices + n;
    nvis = 1;           /* nvis == number of nodes visited */
    for (ap=vp->arcs; ap; ap=ap->next) {
         nextn = ap->tip - G->vertices;
        if (!bitset(vis,nextn)) {  /* not seen yet */
            nvis += dfs(G,nextn,vis);
        }
    }
    return nvis;
} /* dfs */

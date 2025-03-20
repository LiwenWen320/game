#ifndef ANALYZE_H
#define ANALYZE_H

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

#include <igraph_constants.h>
#include <igraph_datatype.h>
#include <igraph_iterators.h>
#include <igraph_types.h>
#include <igraph_vector.h>
#include "seg_defines.h"


__BEGIN_DECLS

igraph_real_t frac_cooperators(const igraph_vector_t *strat);
int assortativity_coefficient(const igraph_t *igraph, 
                            igraph_real_t *ret,
                            igraph_neimode_t mode, 
                            igraph_bool_t loops);
int degree_distrib(const igraph_t *graph, 
                     analyze_vector_t *analyze,
                     igraph_neimode_t mode,
                     igraph_bool_t loops);
igraph_real_t clustering_coefficient(const igraph_t *graph,
                                  igraph_bool_t directed);                                                                    

__END_DECLS

#endif

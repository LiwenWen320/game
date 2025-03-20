#ifndef COORDINATION_GAME_H
#define COORDINATION_GAME_H

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
#include <igraph_types.h>
#include <igraph_vector.h>

__BEGIN_DECLS


int coordination_game(const igraph_t *graph,
                             igraph_integer_t vid,
                             igraph_vector_t *utilities,
                             const igraph_vector_t *strategies,
                             igraph_real_t b,
                             igraph_real_t p,
                             igraph_real_t c);


__END_DECLS

#endif

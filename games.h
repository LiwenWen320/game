#ifndef GAMES_H
#define GAMES_H

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

//#include "public_goods_game.h"
#include "coordination_game.h"

__BEGIN_DECLS

/*
#define TIME_STEPS 10e6
#define AVER_TIME_STEPS 10e3
*/

/* parameters for graph */
/*
struct _graph_init_param_t
{
    igraph_integer_t nvert;
    igraph_integer_t k_deg;
    igraph_bool_t directed;
    igraph_bool_t multiple;
    seg_coevol_dynam_param_t seg_coevol_dynam_param;
    igraph_real_t sigma; // reputation decay factor
    // igraph_integer_t init_indeg;
    // igraph_integer_t init_outdeg;

} ;

typedef struct _graph_init_param_t graph_init_param_t;
*/

int seg_game(const graph_init_param_t *graph_init_param,
                const char *game_func_name,
                int (* const game_func)(const igraph_t *,
                             igraph_integer_t,
                             igraph_vector_t *,
                             const igraph_vector_t *,
                             igraph_real_t,
                             igraph_real_t,
                             igraph_real_t));

__END_DECLS

#endif

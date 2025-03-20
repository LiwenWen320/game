#ifndef MICROSCOPIC_UPDATE_H
#define MICROSCOPIC_UPDATE_H

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

/*
struct _seg_coevol_dynam_param_t
{
    igraph_real_t b;
    igraph_real_t c;
    igraph_real_t w;  // ratio of time scale of strategy updating and partner adjustment
    igraph_real_t beta;
    igraph_real_t pr;  // probability of establish a new link
    igraph_real_t ps;  // probability of sever a existing link
} ;

typedef struct _seg_coevol_dynam_param_t seg_coevol_dynam_param_t;

#define G_PROB(prob) (RNG_UNIF01() > prob)
#define G_W_PROB(w) (RNG_UNIF01() > 1/(1+w))
#define WITH_PROB(prob) !G_PROB(prob)
#define WITH_W_PROB(w) !G_W_PROB(w) 
#define REAL_MIN  DBL_MIN
#define REAL_MAX  DBL_MAX
*/

int seg_coevolution_dynamics1(igraph_t *graph,
                             igraph_integer_t vid,
                             igraph_vector_t *utilities,
                             igraph_vector_t *strategies,
                             igraph_vector_t *ovd,
                             igraph_vector_t *count,
                             igraph_vector_t *environment_neigrs,
                             /*5.16const igraph_vector_t *reputations,*/
                             const seg_coevol_dynam_param_t *seg_coevol_dynam_param,
                             pool_t *count_neigrs,

                             igraph_neimode_t mode,
                             int (* const game_func)(const igraph_t *,
                             igraph_integer_t,
                             igraph_vector_t *,
                             const igraph_vector_t *,
                             igraph_real_t,
                             igraph_real_t,
                             igraph_real_t));



int seg_coevolution_dynamics2(igraph_t *graph,
                             igraph_integer_t vid,
                             igraph_vector_t *utilities,
                             igraph_vector_t *strategies,
                             igraph_vector_t *ovd,
                             igraph_vector_t *count,
                             igraph_vector_t *environment_neigrs,
                             /*5.16const igraph_vector_t *reputations,*/
                             const seg_coevol_dynam_param_t *seg_coevol_dynam_param,
                             pool_t *count_neigrs,

                             igraph_neimode_t mode,
                             int (* const game_func)(const igraph_t *,
                             igraph_integer_t,
                             igraph_vector_t *,
                             const igraph_vector_t *,
                             igraph_real_t,
                             igraph_real_t,
                             igraph_real_t));


/*5.16
int seg_reputation_update(const igraph_t *graph,
                          const igraph_vector_t *strategies,
                          igraph_vector_t *reputations,
                          igraph_real_t sigma);*/

float seg_interactive_neigrs(const igraph_t *graph,
                             const  igraph_vector_t *strategies,
                             igraph_vector_t *count,
                             igraph_vector_t *interactive_neigrs);

/*9.24
float seg_interactive_neigrs_neigrs(const igraph_t *graph,
                                    const igraph_vector_t *strategies,
                                    igraph_vector_t *ncount,                    //统计二阶邻居合作数的变量                  
                                    igraph_vector_t *interactive_neigrs_neigrs); //二阶邻居效应变量
*/
float seg_purchase_intention(const igraph_t *graph,
                            igraph_vector_t *purchase_intention);
             
float seg_purchase_effect1(const igraph_t *graph,
                          igraph_vector_t *interactive_neigrs,
                          
                          igraph_vector_t *interactive,
                          igraph_vector_t *purchase_intention,
                          igraph_real_t Pr1
                          //igraph_vector_t *purchase_effect
                          );

float seg_purchase_effect2(const igraph_t *graph,
                          igraph_vector_t *interactive_neigrs,
                          
                          igraph_vector_t *interactive,
                          igraph_vector_t *purchase_intention,
                          igraph_real_t Pr2
                          //igraph_vector_t *purchase_effect
                          );

__END_DECLS

#endif

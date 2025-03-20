#ifndef SEG_DEFINES_H 
#define SEG_DEFINES_H 

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
#include <float.h>
#include "hash_set.h"

__BEGIN_DECLS

struct _seg_coevol_dynam_param_t
{
    igraph_real_t b;
    igraph_real_t p;
    igraph_real_t c;
    igraph_real_t Pr1;
    igraph_real_t Pr2;
    igraph_real_t beta1;
    igraph_real_t beta2;
} ;

typedef struct _seg_coevol_dynam_param_t seg_coevol_dynam_param_t;

/* parameters for graph */
struct _graph_init_param_t
{
  
    igraph_integer_t time_steps;
    igraph_integer_t average_time_steps;
    igraph_integer_t sample_step;
    igraph_integer_t nvert;
    igraph_integer_t edges;
    igraph_real_t exponent_out; 
    igraph_real_t exponent_in;
    igraph_bool_t loops;
    igraph_bool_t finite_size_correction;

    igraph_integer_t large_nvert;
    igraph_integer_t middle_nvert;
    igraph_integer_t small_nvert;

    igraph_integer_t initial_cooperator_l;
    igraph_integer_t initial_cooperator_m;
    igraph_integer_t initial_cooperator_s;

    igraph_bool_t multiple;
    seg_coevol_dynam_param_t seg_coevol_dynam_param;
    igraph_real_t sigma; // reputation decay factor
    //igraph_real_t cooper_ratio;
} ;

typedef struct _graph_init_param_t graph_init_param_t;

struct _pool_t
{
    igraph_vector_t v;
    hash_set_st *set;
};

typedef struct _pool_t pool_t; 

struct _analyze_vector_t
{
    igraph_real_t fc;
    igraph_real_t assortativity_coefficient[3]; // 0: IN, 1: OUT, 2: ALL
    igraph_real_t clustering_coefficient;
    igraph_vector_t degree_distrib[3][2];   // 0: IN, 1:OUT, 2: ALL    
                                            // 0: degree, 1: number of vertices
};

typedef struct _analyze_vector_t analyze_vector_t;

struct _analyze_ret_t
{
    analyze_vector_t av;
    double times;
};

typedef struct _analyze_ret_t analyze_ret_t;

#define G_PROB(prob) (RNG_UNIF01() > prob)
#define G_W_PROB(w) (RNG_UNIF01() > 1/(1+w))
#define WITH_PROB(prob) (RNG_UNIF01() <= prob)
#define WITH_W_PROB(w) (RNG_UNIF01() <= 1/(1+w)) 
#define REAL_MIN  DBL_MIN
#define REAL_MAX  DBL_MAX

// #define PRINT_DEBUG

int analyze_vector_destroy(analyze_vector_t *v);
int analyze_ret_init(analyze_ret_t *ar);  // without in degree_distrib

__END_DECLS

#endif

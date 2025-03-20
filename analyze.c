#include <igraph_interface.h>
#include <igraph_nongraph.h>
#include <igraph_random.h>
#include <math.h>
#include <string.h>
//#include <mongoc.h>

#include "analyze.h"


/*5.16static void print_vector(const igraph_vector_t *v, FILE *f) {
  long int i;
  for (i=0; i<igraph_vector_size(v); i++) {
    fprintf(f, " %li", (long int) VECTOR(*v)[i]);
  }
  fprintf(f, "\n");
}*/

igraph_real_t frac_cooperators(const igraph_vector_t *strat) {
    igraph_real_t fc=0;
    
    int i;
    for ( i = 0; i < igraph_vector_size(strat); ++i)
    {
        if (VECTOR(*strat)[i] > 0 )
            fc += 1;
    }

    return fc / igraph_vector_size(strat);
}

int assortativity_coefficient(const igraph_t *graph, 
                            igraph_real_t *ret,
                            igraph_neimode_t mode, 
                            igraph_bool_t loops) {

    igraph_vector_t v, vd;
    igraph_integer_t from=0, to=0;
    igraph_integer_t ecount = igraph_ecount(graph);
    igraph_real_t ec = 1.0 / ecount;

    igraph_real_t a=0, b=0, c=0;
    //igraph_es_t es;
    //igraph_eit_t eit;
    //igraph_integer_t eid;
    int i;

    *ret = 0;
    igraph_vector_init(&v, 2);
    igraph_vector_init(&vd, 2);
    
    if (mode == IGRAPH_ALL)
    {
        for (i = 0; i < ecount; ++i)
        {
            igraph_edge(graph, i, &from, &to);
            VECTOR(v)[0] = from; VECTOR(v)[1] = to;

            igraph_degree(graph, &vd, igraph_vss_vector(&v), mode, loops);
            a += (VECTOR(vd)[0] * VECTOR(vd)[1]);
            b += pow(VECTOR(vd)[0] + VECTOR(vd)[1], 2);
            c += (pow(VECTOR(vd)[0], 2) + pow(VECTOR(vd)[1], 2));
        }

        a = a * ec;
        b = pow(b*0.5*ec, 2);
        c = c * 0.5 * ec;

        *ret = (a-b) / (c-b);
    }

    igraph_vector_destroy(&v);
    igraph_vector_destroy(&vd);

    return 0;
}

static int degree_distrib_fill(const igraph_vector_t *dv,
                                igraph_vector_t *deg,
                                igraph_vector_t *deg_num) {
    int i, j, cnt;

    cnt = 0;
    for (i=0; i< igraph_vector_size(dv); i++){
        if (VECTOR(*dv)[i] > 0)
            cnt++;
    }

    igraph_vector_init(deg, cnt);
    igraph_vector_init(deg_num, cnt);

    j = 0;
    for (i = 0; i < igraph_vector_size(dv); ++i)
    {
        if (VECTOR(*dv)[i] > 0){
            VECTOR(*deg)[j] = i;
            VECTOR(*deg_num)[j] = VECTOR(*dv)[i];
            ++j;
        }
    }    

    return 0;
}

int degree_distrib(const igraph_t *graph, 
                     analyze_vector_t *analyze,
                     igraph_neimode_t mode,
                     igraph_bool_t loops){

    igraph_vector_t v, dv;
    igraph_integer_t dcount = 2 * igraph_vcount(graph);
    igraph_integer_t d;
    int i;

    igraph_vector_init(&v, igraph_vcount(graph));
    igraph_vector_init(&dv, dcount);
    igraph_degree(graph, &v, igraph_vss_all(), mode, loops);

    for (i=0; i < igraph_vcount(graph); i++){
        d = VECTOR(v)[i];
        VECTOR(dv)[d]++;
    }

    if (mode == IGRAPH_IN){
        degree_distrib_fill(&dv, &analyze->degree_distrib[0][0], &analyze->degree_distrib[0][1]);
    } else if (mode == IGRAPH_OUT){
        degree_distrib_fill(&dv, &analyze->degree_distrib[1][0], &analyze->degree_distrib[1][1]);
    } else {
        degree_distrib_fill(&dv, &analyze->degree_distrib[2][0], &analyze->degree_distrib[2][1]);
    }

    igraph_vector_destroy(&v);
    igraph_vector_destroy(&dv);

    return 0;
}

int deg_strategy(const igraph_t *graph,
                const igraph_vector_t *strat,
                igraph_vector_t *indeg,
                igraph_vector_t *indeg_num,
                igraph_bool_t cooper,
                igraph_neimode_t mode,
                igraph_bool_t loops){

    igraph_vector_t v, dv;
    igraph_integer_t d;

    igraph_vector_init(&v, igraph_vcount(graph));
    igraph_vector_init(&dv, igraph_vcount(graph));
    igraph_degree(graph, &v, igraph_vss_all(), mode, loops);
    if (cooper){
        int i;  
        for ( i=0; i<igraph_vcount(graph); i++){
            if (VECTOR(*strat)[i] > 0){
                d = VECTOR(v)[i];
                VECTOR(dv)[d]++;
            }
        }
    } else {
        int i;
        for ( i=0; i<igraph_vcount(graph); i++){
            if (VECTOR(*strat)[i] < 0){
                d = VECTOR(v)[i];
                VECTOR(dv)[d]++;
            }
        }
    }
    degree_distrib_fill(&dv, indeg, indeg_num);

    igraph_vector_destroy(&v);
    igraph_vector_destroy(&dv);

    return 0;
}

int del_dulp(igraph_vector_t *ret, igraph_vector_t *n){
    igraph_integer_t k;
    igraph_vector_fill(ret, -2);
    int i;
    for (i = 0; i < igraph_vector_size(n); ++i)
    {
        k = (int) VECTOR(*n)[i] % (int) igraph_vector_size(ret);
        if (VECTOR(*ret)[k] < 0)
        {
            VECTOR(*ret)[k] = VECTOR(*n)[i];
        }
    }

    return 0;
}

igraph_real_t clustering_coefficient(const igraph_t *graph,
                          igraph_bool_t directed){

    igraph_integer_t from, to, eid, cnt, k, ki, vcount/*5.16, ecount*/;
    igraph_vector_t neis, neis_d;
    igraph_real_t c = 0;
    int n, i, j, ret;

    vcount = igraph_vcount(graph);
    //5.16ecount = igraph_ecount(graph);

    igraph_vector_init(&neis, 10);
    igraph_vector_init(&neis_d, vcount);

    c = 0;
    for (n = 0; n < vcount; ++n)
    {
        igraph_neighbors(graph, &neis, n, IGRAPH_ALL);
        del_dulp(&neis_d, &neis);
        k = igraph_vector_size(&neis);
        if (k > 1){
            cnt = 0;
            for (i=0; i < k; i++){
                ki = (int) VECTOR(neis)[i] % (int) vcount;
                from = VECTOR(neis_d)[ki];
                for (j = i+1; j < k; j++)
                {
                    ki = (int) VECTOR(neis)[j] % (int) vcount;
                    to = VECTOR(neis_d)[ki];
                    // if (from == to)
                    //     continue;
                    ret = igraph_get_eid(graph, &eid, from, to, IGRAPH_DIRECTED, /*error=*/ 1);
                    if (ret == 0){ 
                        // printf("ret: %d, eid: %d, from: %d, to: %d\n", ret, eid, from, to);
                        cnt++;
                    } else {
                        // printf("ret: %d, eid: %d\n", ret, eid); 
                    }
                    
                    ret = igraph_get_eid(graph, &eid, to, from, IGRAPH_DIRECTED, /*error=*/ 1);
                    if (ret == 0){ // IGRAPH_EINVAL
                        // printf("ret: %d, eid: %d, from: %d, to: %d\n", ret, eid, to, from);
                        cnt++;
                    } else {
                        // printf("ret: %d, eid: %d\n", ret, eid); 
                    }
                }
            }

            c += 1.0 * cnt / (k*(k-1)); 
        }
    }
    //printf("clustering_coefficient.\n");
    igraph_vector_destroy(&neis);
    igraph_vector_destroy(&neis_d);

    return c / vcount;
}



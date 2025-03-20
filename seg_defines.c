#include "seg_defines.h"

/* without in degree_distrib */
int analyze_ret_init(analyze_ret_t *ar){
    ar->av.fc = 0;
    ar->av.assortativity_coefficient[0] = 0;
    ar->av.assortativity_coefficient[1] = 0;
    ar->av.assortativity_coefficient[2] = 0;
    ar->av.clustering_coefficient = 0;
    ar->times = 0;

    return 0;
}

int analyze_vector_destroy(analyze_vector_t *v){
    int i, j;

    for (i=0; i<3; i++){
        for (j=0; j<2; j++){
            igraph_vector_destroy(&v->degree_distrib[i][j]);
        }
    }

    return 0;
}

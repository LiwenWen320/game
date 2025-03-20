//line 38,39,40
#include <stdio.h>
//#include <bson.h>
//#include <mongoc.h>
#include "games.h"


extern int coordination_game(const igraph_t *graph,
                                 igraph_integer_t vid,
                                 igraph_vector_t *utilities,
                                 const igraph_vector_t *strategies,
                                 igraph_real_t b,
                                 igraph_real_t p,
                                 igraph_real_t c);

int graph_param_init(graph_init_param_t *gip){


    gip->time_steps = 1*1e5;
    gip->average_time_steps = 1*1e3;
    gip->sample_step = gip->time_steps;
    if (gip->sample_step > gip->time_steps){
        printf("No samples, because sample_step is greater than time_steps\n");
    }

   gip->nvert = 5000;
    gip->edges = 300000;
    gip->exponent_out=10;
    gip->exponent_in=2;
    gip->loops=0;//IGRAPH_NO_loops;
    gip->multiple = IGRAPH_NO_MULTIPLE;
    gip->finite_size_correction=1;

    gip->large_nvert=50;
    gip->middle_nvert=200;
    gip->small_nvert=4750;

    gip->initial_cooperator_l=0;
    gip->initial_cooperator_m=100;
    gip->initial_cooperator_s=0;

    gip->seg_coevol_dynam_param.b = 0.1;  // T
    gip->seg_coevol_dynam_param.p = 1;
    gip->seg_coevol_dynam_param.c = 1;  // R = 1
    gip->seg_coevol_dynam_param.beta1 = 0.01;
    gip->seg_coevol_dynam_param.beta2 = 0.1;
    gip->seg_coevol_dynam_param.Pr1 = 0.1;
    gip->seg_coevol_dynam_param.Pr2 = 0.9;
    //gip->sigma = 1;
    //gip->cooper_ratio = 0.5;

    return 0;
}

void func_T_W(){
    graph_init_param_t gip;
    igraph_real_t k;
    int i;//建立i次网络
    graph_param_init(&gip);  
   
    

    for (i=0; i < 10; i++) { 
      
   
            //printf("  b:%lf\n ",b);
          for (k = 0;  k< 1.03; k += 0.1) {
            //printf("  b:%lf\n ",b);
            gip.initial_cooperator_s =k*4750;
            //printf("  b:%lf\n ",b);
            
            seg_game(&gip, "tw08", &coordination_game);
       } 
      
     }         
    }



//主函数
int main(int argc, char const *argv[])
{ 
    
    func_T_W();
    //func_T_pr();

    return 0;
}

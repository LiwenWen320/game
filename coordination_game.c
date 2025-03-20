//line 28
#include <igraph_iterators.h>
#include <igraph_interface.h>
#include <igraph_random.h>
#include "coordination_game.h"
#include "hash_set.h"

#include <assert.h>

/*5.16static uint32_t hash_int(const void *number) {
  return ((*(int *)number) % 50000);
}*/

int coordination_game(const igraph_t *graph,
                             igraph_integer_t vid,
                             igraph_vector_t *utilities,
                             const igraph_vector_t *strategies,
                             igraph_real_t b,
                             igraph_real_t p,
                             igraph_real_t c) {
    
    igraph_vs_t adj;
    igraph_vit_t vit;
    igraph_integer_t v_id;
    igraph_real_t payoff;
    igraph_real_t payoff_OUT;
    igraph_real_t payoff_IN;
    // igraph_real_t a=0;
    
   
  //与出邻居博弈
    igraph_vs_adj(&adj, vid, IGRAPH_OUT);
    //printf("vid1=%d\n",vid);
    igraph_vit_create(graph, adj, &vit);
   
    payoff = 0;
    while (!IGRAPH_VIT_END(vit)){
        v_id = IGRAPH_VIT_GET(vit);
        //printf("vid2=%d\n",vid);

        //if (VECTOR(*strategies)[v_id] > 0) {
          // if (VECTOR(*strategies)[vid]>0) {
	/*vid is coorperator*/
            //    payoff += c;
            //} else {
	/*vid is defector*/
              //  payoff += b;
            //}
        //}
        //printf("here\n");
        //v_id  is coorperator
        if(VECTOR(*strategies)[v_id] > 0){
          /*vid is coorperator*/     
          if (VECTOR(*strategies)[vid]>0){
            payoff_OUT += 1-0.506*(1-b);
          }
          /*vid is defector*/
            else {
                payoff_OUT += 1;
            }}
      
        if(VECTOR(*strategies)[v_id] <0){
          /*vid is coorperator*/ 
          if (VECTOR(*strategies)[vid]>0){
             payoff_OUT += 0;
          }
         /*vid is defector*/
          else {
                payoff_OUT += 0;
          }
        }

        IGRAPH_VIT_NEXT(vit);
        //printf("vid3=%d\n",vid);

    }

    igraph_vit_destroy(&vit);
    igraph_vs_destroy(&adj); 

  

  //与入邻居博弈
    igraph_vs_adj(&adj, vid, IGRAPH_IN);
    //printf("vid1=%d\n",vid);
    igraph_vit_create(graph, adj, &vit);
   
    payoff = 0;
    while (!IGRAPH_VIT_END(vit)){
        v_id = IGRAPH_VIT_GET(vit);
        //printf("vid2=%d\n",vid);

        
        //v_id  is coorperator
        if(VECTOR(*strategies)[v_id] > 0){
          /*vid is coorperator*/     
          if (VECTOR(*strategies)[vid]>0){
            payoff_IN += 1-0.506*(1-b);
          }
          /*vid is defector*/
            else {
                payoff_IN += 0;
            }}
      
        if(VECTOR(*strategies)[v_id] <0){
          /*vid is coorperator*/ 
          if (VECTOR(*strategies)[vid]>0){
             payoff_IN += -0.506*(1-b);
          }
         /*vid is defector*/
          else {
                payoff_IN += 0;
          }
        }

        IGRAPH_VIT_NEXT(vit);
        //printf("vid3=%d\n",vid);

    }

    igraph_vit_destroy(&vit);
    igraph_vs_destroy(&adj); 

  
    payoff = payoff_OUT + payoff_IN;
    
    VECTOR(*utilities)[vid] = payoff;
    //printf("vid4=%d\n",vid);
    
    return 0;
    
}

//line 254,353
#include <igraph_interface.h>
#include <igraph_nongraph.h>
#include <igraph_random.h>
#include <math.h>
#include "microscopic_update.h"
//#include "public_goods_game.h"
#include "coordination_game.h"

#include "hash_set.h"

#include <assert.h>

extern int coordiantion_game(const igraph_t *graph,
                             igraph_integer_t vid,
                             igraph_vector_t *utilities,
                             const igraph_vector_t *strategies,
                             igraph_real_t b,
                             igraph_real_t p,
                             igraph_real_t c);

static int real_equal(igraph_real_t a, igraph_real_t b){
  return fabs(a - b) < 1e-10;
  
}

static int seg_strategy_imitation(const igraph_t *graph,
                            igraph_integer_t vid,
                            igraph_vector_t *utilities,
                            const igraph_vector_t *strategies,
                            const seg_coevol_dynam_param_t *param,
                            int (* const game_func)(const igraph_t *,
                                 igraph_integer_t,
                                 igraph_vector_t *,
                                 const igraph_vector_t *,
                                 igraph_real_t,
                                 igraph_real_t,
                                 igraph_real_t),
                            pool_t *count_neigrs){
  igraph_vs_t adj;
  igraph_vit_t vit;

  igraph_integer_t nid, v_id;
  igraph_real_t u = REAL_MIN, phi=0;
  igraph_integer_t count_len, maxu_cnt;
  int i;
  igraph_bool_t dulp;

  igraph_vector_null(&count_neigrs->v);
  count_len = igraph_vector_size(&count_neigrs->v);

  //printf("count_len=%d\n",count_len);
  //count_len=顶点数

  /* update vid's utility */
  
  game_func(graph, vid, utilities, strategies, param->b, param->p, param->c);

  /* strategy update */

  IGRAPH_CHECK(igraph_vs_adj(&adj, vid, IGRAPH_OUT));
  IGRAPH_CHECK(igraph_vit_create(graph, adj, &vit));
  //printf("vid=%d\n",vid);
  
  // printf("%s:%d\n", __FILE__,__LINE__);

  maxu_cnt = 0;
  while (!IGRAPH_VIT_END(vit)){
    // find the vertex with max utility from vid's OUT-neighbors
    v_id = IGRAPH_VIT_GET(vit);

    /* update v_id's utility */
    game_func(graph, v_id, utilities, strategies, param->b, param->p, param->c);

    if (VECTOR(*utilities)[v_id] > u){
      u = VECTOR(*utilities)[v_id];
      // nid = v_id;
      maxu_cnt = 1;
      VECTOR(count_neigrs->v)[maxu_cnt-1] = v_id;//用v来存储v_id？
    } else if (real_equal(VECTOR(*utilities)[v_id], u) && maxu_cnt < count_len) 
    {
      dulp = FALSE;
      for (i=0; i<maxu_cnt; i++){
        if (VECTOR(count_neigrs->v)[i] == v_id){
          dulp = TRUE;
          break;
        }
      }
      if (!dulp){
        maxu_cnt++;
        VECTOR(count_neigrs->v)[maxu_cnt-1] = v_id;
      } 
    }

    IGRAPH_VIT_NEXT(vit);
  }

  // clean up
  igraph_vit_destroy(&vit);
  igraph_vs_destroy(&adj);

  if (maxu_cnt > 0)
  {
    if (maxu_cnt == 1){
      nid = VECTOR(count_neigrs->v)[0];
    } else {
      nid = RNG_INTEGER(0, maxu_cnt-1);
      u = VECTOR(*utilities)[nid];
    }
    // imitate strategy
    phi = 1 / (1 + exp(param->beta1 * (u-VECTOR(*utilities)[vid])));

    if (WITH_PROB(phi)){
      VECTOR(*strategies)[vid] = VECTOR(*strategies)[nid];
    }
  }

  return 0;
}


float seg_environment_neigrs(const igraph_t *graph,
                             igraph_integer_t vid,
                             const igraph_vector_t *strategies,
                             const igraph_vector_t *ovd,
                             igraph_vector_t *count,  //统计一阶阶邻居合作数的变量
                             igraph_vector_t *environment_neigrs
                             ){
  igraph_integer_t v_id;
  igraph_integer_t  i=0;

  igraph_vs_t adj;
  igraph_vit_t vit;

  igraph_vs_adj(&adj, i, IGRAPH_OUT);
  igraph_vit_create(graph, adj, &vit);

  i=vid;

  VECTOR(*count)[i]=0;
        
  while (!IGRAPH_VIT_END(vit)){
    v_id = IGRAPH_VIT_GET(vit);
      
  if(VECTOR(*strategies)[v_id]>0){
     VECTOR(*count)[i] ++;
    }

    IGRAPH_VIT_NEXT(vit);
     
  }
       
  if(VECTOR(*strategies)[i]>0) {

    VECTOR(*environment_neigrs)[i] =(float)VECTOR(*count)[i]-((float)VECTOR(*ovd)[i]/2);

  }else{

    VECTOR(*environment_neigrs)[i] =((float)VECTOR(*ovd)[i]-(float)VECTOR(*count)[i])-((float)VECTOR(*ovd)[i]/2);
  }

    //printf("%s:%d\n", __FILE__,__LINE__);
    igraph_vit_destroy(&vit);
    igraph_vs_destroy(&adj);
    //igraph_vector_destroy(&count);
   
 
  return VECTOR(*environment_neigrs)[i];
}


static int seg_environment_strategy_imitation(const igraph_t *graph,
                                              igraph_integer_t vid,
                                              const igraph_vector_t *strategies,
                                              const seg_coevol_dynam_param_t *param,
                                              const igraph_vector_t *ovd,
                                              igraph_vector_t *count,
                                              igraph_vector_t *environment_neigrs)
                                              {
  
  int i=0,h=0 ;
  float ef,omega;

  i=vid;

  ef=(float)VECTOR(*environment_neigrs)[i];

  omega=1 / (1 + exp(param->beta2 * ef));

  if(VECTOR(*count)[i]>0){
    h=1;
  }
  else{h=-1;}

  if (WITH_PROB(omega)){

      VECTOR(*strategies)[i] = h;
    }

  return 0;
}




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
                                 igraph_real_t)) {
    

 if (WITH_PROB(seg_coevol_dynam_param->Pr1)){
    
    seg_strategy_imitation(graph, vid, utilities, strategies, seg_coevol_dynam_param, game_func, count_neigrs);
   
  } else {

    seg_environment_neigrs(graph, vid, strategies, ovd, count, environment_neigrs);
    seg_environment_strategy_imitation(graph, vid, strategies, seg_coevol_dynam_param, ovd, count,environment_neigrs);
  }

 

  IGRAPH_FINALLY_CLEAN(1);

  return IGRAPH_SUCCESS;

}


//修改

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
                                 igraph_real_t)) {
    

 if (WITH_PROB(seg_coevol_dynam_param->Pr2)){
    
    seg_strategy_imitation(graph, vid, utilities, strategies, seg_coevol_dynam_param, game_func, count_neigrs);
   
  } else {

    seg_environment_neigrs(graph, vid, strategies, ovd, count, environment_neigrs);
    seg_environment_strategy_imitation(graph, vid, strategies, seg_coevol_dynam_param, ovd, count,environment_neigrs);
  }

 

  IGRAPH_FINALLY_CLEAN(1);

  return IGRAPH_SUCCESS;

}







float seg_interactive_neigrs(const igraph_t *graph,
                             const  igraph_vector_t *strategies,
                             igraph_vector_t *count,             //统计一阶阶邻居合作数的变量
                             igraph_vector_t *interactive_neigrs
                             ){
  igraph_integer_t v_id;
  igraph_real_t nec;
  //igraph_real_t pr= 0.5;
  //igraph_real_t k= 0.6;
  igraph_integer_t  i=0;

  /*
    1. 
  */
  
   
   for (i=0; i<igraph_vcount(graph); i++){
    

    igraph_vs_t adj;
    igraph_vit_t vit;

    igraph_vs_adj(&adj, i, IGRAPH_OUT);
    igraph_vit_create(graph, adj, &vit);

    // printf("\n分析编号为%d点的情况\n",i);
     VECTOR(*count)[i]=0;
     //VECTOR()
     //igraph_vector_fill(count, 0);
     nec = 0;
        while (!IGRAPH_VIT_END(vit)){
          v_id = IGRAPH_VIT_GET(vit);

            //printf("编号为 %d 的节点它的出邻居节点为%li \n", i, (long int)v_id);
      
            if(VECTOR(*strategies)[v_id]>0){
             VECTOR(*count)[i] ++;
            }
           nec++;

          IGRAPH_VIT_NEXT(vit);
     
        }
        //printf("VECTOR(*count)[i]: %f\n",VECTOR(*count)[i]);
        //printf("编号为%d的节点邻居数为%f\n",i,nec);

        //VECTOR(*interactive_neigrs)[i] = 1 - 1/exp((float)VECTOR(*count)[i]/nec) ; 
        VECTOR(*interactive_neigrs)[i] = (float)(VECTOR(*count)[i]/nec) ;
       // printf("一阶邻居效应值为VECTOR(*interactive_neigrs)[i]:%f\n", VECTOR(*interactive_neigrs)[i]);
   
    //printf("编号为%d的节点邻居中选择合作的人数为%li\n",10,n);
    igraph_vit_destroy(&vit);
    igraph_vs_destroy(&adj);
    
  }
  return VECTOR(*interactive_neigrs)[i];
}



float seg_purchase_intention(const igraph_t *graph,
                             igraph_vector_t *purchase_intention){
  igraph_integer_t i=0;
  float a[5000]={0.0};  
  FILE *fpRead=fopen("data.txt","r");  
  if(fpRead==NULL)  
  {  
      return 0;  
  }  
  for(int i=0;i<5000;i++)  
  {  
      fscanf(fpRead,"%f",&a[i]);
  }
  for(i=0;i<igraph_vcount(graph); i++){
   // VECTOR(*purchase_intention)[i] =  RNG_UNIF(0,0.2);
    VECTOR(*purchase_intention)[i] =  a[i];
    //VECTOR(*purchase_intention)[i] =  0.2;
    // printf("节点%d的初始购买意愿为%f\n", i, VECTOR(*purchase_intention)[i]);
   // printf("购买意愿%f\n",VECTOR(*purchase_intention)[i]);
  }
  
 return VECTOR(*purchase_intention)[i];
}


float seg_purchase_effect1(const igraph_t *graph,
                          igraph_vector_t *interactive_neigrs,
                          
                          igraph_vector_t *interactive,
                          igraph_vector_t *purchase_intention,
                          igraph_real_t Pr1
                          //igraph_vector_t *purchase_effect
                          ){
  //igraph_integer_t count = 0;
  //igraph_integer_t nvert = 500;
  //igraph_integer_t  min_count=0;
  igraph_integer_t count=0;
  //igraph_integer_t max_count=0;
  igraph_integer_t i=0;
  igraph_real_t l=0.1;
  igraph_real_t b=0.1;
  //igraph_real_t pr = 0.1;
 // igraph_real_t a=0;
  //igraph_real_t a=0.5, b=0.5;
  //igraph_real_t purchase_effect_min = 0.6;
  igraph_real_t pc;
  for(i=0;i<igraph_vcount(graph); i++){ 
   VECTOR(*interactive)[i] = (1-Pr1)*(0.5*VECTOR(*interactive_neigrs)[i]+0.5*(0.6224*(b+l)-0.359))+Pr1*(-0.543*(b+l)*(b+l)+1.244*(b+l))  ;
    //VECTOR(*purchase_effect)[i] =  (VECTOR(*interactive)[i]) + (VECTOR(*purchase_intention)[i]);
    //RNG_UNIF01()
    //RNG_INTEGER(0, graph_init_param->nvert - 1);
    //printf("\n邻居对个体%d产生的影响为%f\n", i, VECTOR(*purchase_effect)[i]);
     //printf("邻居对个体%d产生的影响为%f\n", i, VECTOR(*interactive)[i] );
    
     if(VECTOR(*interactive)[i] > VECTOR(*purchase_intention)[i]){
      //if (VECTOR(*interactive)[i] >VECTOR(*purchase_intention)[i]){ 
         count ++;
        }
    /*if (VECTOR(*interactive)[i] <0.4){
     min_count ++;
    }
    if( (VECTOR(*interactive)[i]>=0.4) && (VECTOR(*interactive)[i]<0.6) ){
        count++;
    }
    if( VECTOR(*interactive)[i]>=0.6 ){
      max_count++;
    }
    */
  }
  
  //pc = (0.2*(float)min_count/500 + 0.4*(float)count/500 + 0.6*(float)max_count/500);
  pc = (float)count/5000;
  //printf("购买率为%lf\n",pc);
   return pc;
}


float seg_purchase_effect2(const igraph_t *graph,
                          igraph_vector_t *interactive_neigrs,
                          
                          igraph_vector_t *interactive,
                          igraph_vector_t *purchase_intention,
                          igraph_real_t Pr2
                          //igraph_vector_t *purchase_effect
                          ){
  //igraph_integer_t count = 0;
  //igraph_integer_t nvert = 500;
  //igraph_integer_t  min_count=0;
  igraph_integer_t count=0;
  //igraph_integer_t max_count=0;
  igraph_integer_t i=0;
  igraph_real_t l=0.1;
  igraph_real_t b=0.1;
  //igraph_real_t pr = 0.1;
 // igraph_real_t a=0;
  //igraph_real_t a=0.5, b=0.5;
  //igraph_real_t purchase_effect_min = 0.6;
  igraph_real_t pc;
  for(i=0;i<igraph_vcount(graph); i++){ 
   VECTOR(*interactive)[i] = (1-Pr2)*(0.5*VECTOR(*interactive_neigrs)[i]+0.5*(0.6224*(b+l)-0.359))+Pr2*(-0.543*(b+l)*(b+l)+1.244*(b+l))  ;
    //VECTOR(*purchase_effect)[i] =  (VECTOR(*interactive)[i]) + (VECTOR(*purchase_intention)[i]);
    //RNG_UNIF01()
    //RNG_INTEGER(0, graph_init_param->nvert - 1);
    //printf("\n邻居对个体%d产生的影响为%f\n", i, VECTOR(*purchase_effect)[i]);
     //printf("邻居对个体%d产生的影响为%f\n", i, VECTOR(*interactive)[i] );
    
     if(VECTOR(*interactive)[i] > VECTOR(*purchase_intention)[i]){
      //if (VECTOR(*interactive)[i] >VECTOR(*purchase_intention)[i]){ 
         count ++;
        }
    /*if (VECTOR(*interactive)[i] <0.4){
     min_count ++;
    }
    if( (VECTOR(*interactive)[i]>=0.4) && (VECTOR(*interactive)[i]<0.6) ){
        count++;
    }
    if( VECTOR(*interactive)[i]>=0.6 ){
      max_count++;
    }
    */
  }
  
  //pc = (0.2*(float)min_count/500 + 0.4*(float)count/500 + 0.6*(float)max_count/500);
  pc = (float)count/5000;
  //printf("购买率为%lf\n",pc);
   return pc;
}
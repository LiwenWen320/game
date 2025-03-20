/**/
#include <igraph.h>
#include <stdio.h>
#include <sys/time.h>

#include "microscopic_update.h"
#include "games.h"
//#include "seg_mongodb.h"
#include "analyze.h"

#include <assert.h>

static void print_vector(const igraph_vector_t *v, FILE *f) {
  long int i;
  for (i=0; i<igraph_vector_size(v); i++) {
    fprintf(f, " %li", (long int) VECTOR(*v)[i]);
  }
  fprintf(f, "\n");
}

long getCurrentTime()    
{    
   struct timeval tv;    
   gettimeofday(&tv,NULL);    
   return tv.tv_sec * 1000 + tv.tv_usec / 1000;    
}    


    

int seg_game(const graph_init_param_t *graph_init_param,
            const char *game_func_name,
            int (* const game_func)(const igraph_t *,
                             igraph_integer_t,
                             igraph_vector_t *,
                             const igraph_vector_t *,
                             igraph_real_t,
                             igraph_real_t,
                             igraph_real_t))
   {
    igraph_t graph;

    igraph_vector_t util, strat/*5.16, reput*/;

    igraph_vector_t vd, ovd, ivd;

    igraph_vector_t large, middle, small;

    igraph_vector_t cooperate_l, cooperate_m, cooperate_s;
//修改
    igraph_vector_t cnount, interactive_neigrs;
    //igraph_vector_t dncount, interactive_d_neigrs;
     
    igraph_vector_t interactive;
    //igraph_vector_t interactive_d;

    igraph_vector_t purchase_intention;
    igraph_vector_t purchase_effect1,purchase_effect2;
    igraph_vector_t count;
//
    igraph_vector_t environment_neigrs;

    pool_t count_neigrs;

    long int i,j,m,n;  

    igraph_integer_t max, max_index, min, min_index;
    
    igraph_integer_t vid;

    /* variables for analyzing */
    igraph_real_t fc, fc1;
 
 /*9.24修改*/
    igraph_real_t pc;
    igraph_real_t ave_pc;


   

    /* create graph, and initialize strategies*/
    /* k-regular directed graph, no multiple edges */
    #ifdef PRINT_DEBUG
    printf("create graph.\n");
    #endif
    //printf("开始建立网络");
    /*igraph_k_regular_game(&graph, graph_init_param->nvert, 
                            graph_init_param->k_deg, 
                            graph_init_param->directed,
                            graph_init_param->multiple);*/

    igraph_static_power_law_game(&graph, graph_init_param->nvert, 
                                    graph_init_param->edges,
                                    graph_init_param->exponent_out,
                                    //igraph_real_t exponent_out, 
                                    graph_init_param->exponent_in,
                                    graph_init_param->loops, 
                                    graph_init_param->multiple,
                                    graph_init_param->finite_size_correction);
    //printf("一次循环\n");
    //printf("完成建立网络");
    /* Strategies vector, one strategy for each vertex. Thus vec[i] is the */
    /* strategy of vertex i. */
    /* strategy: -1 -> defect, and 1 -> cooperate with probability cooper_ratio */
    //从图形中删除多边和自边
    igraph_simplify(&graph, 1, 1, /*edge_comb=*/ 0);
    //遍历每个节点，输出其度、出度和入度
    IGRAPH_CHECK(igraph_vector_init(&vd, graph_init_param->nvert));
    IGRAPH_CHECK(igraph_vector_init(&ovd, graph_init_param->nvert));
    IGRAPH_CHECK(igraph_vector_init(&ivd, graph_init_param->nvert));

    IGRAPH_CHECK(igraph_vector_init(&environment_neigrs, graph_init_param->nvert));
    IGRAPH_CHECK(igraph_vector_init(&count, graph_init_param->nvert));

    IGRAPH_CHECK(igraph_vector_init(&large, graph_init_param->large_nvert));
    IGRAPH_CHECK(igraph_vector_init(&middle, graph_init_param->middle_nvert));
    IGRAPH_CHECK(igraph_vector_init(&small, graph_init_param->small_nvert));

    IGRAPH_CHECK(igraph_vector_init(&cooperate_l, graph_init_param->initial_cooperator_l));
    IGRAPH_CHECK(igraph_vector_init(&cooperate_m, graph_init_param->initial_cooperator_m));
    IGRAPH_CHECK(igraph_vector_init(&cooperate_s, graph_init_param->initial_cooperator_s));

    igraph_degree(&graph, &vd, igraph_vss_all(), IGRAPH_ALL, IGRAPH_NO_LOOPS);
    igraph_degree(&graph, &ovd, igraph_vss_all(), IGRAPH_OUT, IGRAPH_NO_LOOPS);
    igraph_degree(&graph, &ivd, igraph_vss_all(), IGRAPH_IN, IGRAPH_NO_LOOPS);

    IGRAPH_CHECK(igraph_vector_init(&purchase_effect1, graph_init_param->nvert));
    IGRAPH_CHECK(igraph_vector_init(&purchase_effect2, graph_init_param->nvert));

    IGRAPH_CHECK(igraph_vector_init(&interactive, graph_init_param->nvert));
    IGRAPH_CHECK(igraph_vector_init(&interactive_neigrs, graph_init_param->nvert));
    IGRAPH_CHECK(igraph_vector_init(&purchase_intention, graph_init_param->nvert));
    //printf("print vector - in degree(ivd):\n");
    //输出初始网络结构
    /*printf("vd:");
    print_vector(&vd, stdout);
    printf("ovd:");
    print_vector(&ovd, stdout);
    printf("ivd:");
    print_vector(&ivd, stdout);
    printf("ovd:");
    print_vector(&ovd, stdout);
    
    
    

    return 0;
    */

    
    //自定义遍历输出
    /*for (i=0; i<graph_init_param->nvert; i++){
      printf("print id=%d:",i);
      printf("%d\n ", (int)VECTOR(ivd)[i]);  
    }*/
    
    //printf("print id=i:",i);
    //printf("print head/large element of ivd： %d\n", (long int)VECTOR(ivd)[0]);
    //printf("end.\n");
    
    //取头部(large_nvert)
    {
        m=graph_init_param->large_nvert;
        n=graph_init_param->nvert;

    for(i = 0;i<m;i++){  //取前m大的数
        max = VECTOR(ivd)[0];
        max_index = 0;
        for(j = 1;j<n;j++){ //n为原数组的大小，即ivd大小，即节点数
       if(VECTOR(ivd)[j]>max){
        max = VECTOR(ivd)[j];
        max_index = j;
        }
      }
      //printf("%d\n", max_index);
      VECTOR(large)[i]=max_index;
      VECTOR(ivd)[max_index] = 0;
     }
    //search_large_index(&ivd, graph_init_param->nvert,15);
    //5.30print_vector(&large, stdout);
    //7.17igraph_vector_sort(&large);
    }

    //middle_nvert
    {
        m=graph_init_param->middle_nvert;
        n=graph_init_param->nvert;

    for(i = 0;i<m;i++){  //取前m大的数
        max = VECTOR(ivd)[0];
        max_index = 0;
        for(j = 1;j<n;j++){ //n为原数组的大小，即ivd大小，即节点数
       if(VECTOR(ivd)[j]>max){
        max = VECTOR(ivd)[j];
        max_index = j;
        }
      }
      //printf("%d\n", max_index);
      VECTOR(middle)[i]=max_index;
      VECTOR(ivd)[max_index] = 0;
     }
    //search_large_index(&ivd, graph_init_param->nvert,15);
    //5.30print_vector(&middle, stdout);
    //7.17igraph_vector_sort(&middle);
    }


    //small_nvert
    { igraph_degree(&graph, &ivd, igraph_vss_all(), IGRAPH_IN, IGRAPH_NO_LOOPS);

        m=graph_init_param->small_nvert;
        n=graph_init_param->nvert;

    for(i = 0;i<m;i++){  //取前m小的数
        min = VECTOR(ivd)[0];
        min_index = 0;
        for(j = 1;j<n;j++){ //n为原数组的大小，即ivd大小，即节点数
       if(VECTOR(ivd)[j]<min){
        min = VECTOR(ivd)[j];
        min_index = j;
        }
      }
      //printf("%d\n", max_index);
      VECTOR(small)[i]=min_index;
      VECTOR(ivd)[min_index] = 10000;
     }
    //search_large_index(&ivd, graph_init_param->nvert,15);
    igraph_vector_sort(&small);
    //6.28printf("small:");
    //6.28print_vector(&small, stdout);
    }

 //选定初始合作者
    {IGRAPH_CHECK(igraph_vector_init(&cooperate_l, graph_init_param->initial_cooperator_l));
        for (i=0; i < graph_init_param->initial_cooperator_l; i++){
            igraph_vector_set(&cooperate_l,i,VECTOR(large)[i]);
       
        }
    //7.17print_vector(&cooperate_l, stdout);
    } 
    //6.28print_vector(&cooperate_l, stdout);
    
    
    {IGRAPH_CHECK(igraph_vector_init(&cooperate_m, graph_init_param->initial_cooperator_m));
        for (i=0; i < graph_init_param->initial_cooperator_m; i++){
            igraph_vector_set(&cooperate_m,i,VECTOR(middle)[i]);

        }
        
    //6.28print_vector(&cooperate_m, stdout);
    }
  
    {IGRAPH_CHECK(igraph_vector_init(&cooperate_s, graph_init_param->initial_cooperator_s));
        for (i=0; i < graph_init_param->initial_cooperator_s; i++){
            igraph_vector_set(&cooperate_s,i,VECTOR(small)[i]);
        }
    //6.28print_vector(&cooperate_s, stdout);
    }

   igraph_vector_sort(&cooperate_l);
   igraph_vector_sort(&cooperate_m);
   igraph_vector_sort(&cooperate_s);
   igraph_vector_sort(&large);
   igraph_vector_sort(&middle);
   //print_vector(&cooperate_l, stdout);


    RNG_BEGIN();

    #ifdef PRINT_DEBUG
    printf("Initialize strategies.\n");
    #endif
    //创建长度为顶点数量的策略集
    IGRAPH_CHECK(igraph_vector_init(&strat, graph_init_param->nvert));
    //初始化数组（给定每个节点初始策略）
     for (i = 0; i < graph_init_param->nvert; i++){
        if (igraph_vector_binsearch2(&cooperate_l,i)||igraph_vector_binsearch2(&cooperate_m,i))
            VECTOR(strat)[i] = 1;
        else
            VECTOR(strat)[i] = -1;
    }

    /* Utilities vector, one utility per vertex. Thus vec[i] is the */
    /* utility for vertex i. */
    #ifdef PRINT_DEBUG
    printf("Initialize utilities.\n");
    #endif
    //vid = 0;
    //创建长度为顶点数量的效用集
    IGRAPH_CHECK(igraph_vector_init(&util, graph_init_param->nvert));
    //进行博弈，链到coordination_game.c
    for (i=0; i < graph_init_param->nvert; i++){
        //vid=i;
        vid = RNG_INTEGER(0, graph_init_param->nvert - 1);
        //printf("i=%ld vid=%d\n",i,vid);
       
        game_func(&graph, vid, &util, &strat, 
                    graph_init_param->seg_coevol_dynam_param.b,
                    graph_init_param->seg_coevol_dynam_param.p,
                    graph_init_param->seg_coevol_dynam_param.c);

        
    }


    /* For intermediate data, temporarily storage for counting neighbors*/
    #ifdef PRINT_DEBUG
    printf("Initialize count_neigrs.\n");
    #endif
    //将pool_t结构类型的count_neigers中的参数v设置长度为顶点数量
    igraph_vector_init(&count_neigrs.v, graph_init_param->nvert);

    /* Main phase: play game and update strategies and partnership */
    //pc =0;
    //先演化time_steps次
   // {
    //fc=0;
    //fc1=0;
    for (i = 0; i < graph_init_param->time_steps; i++){
        /*if (i == 0)

        {   //fc1=fc;
            //fc1= (float)fc / i;
            fc1= frac_cooperators(&strat);
            printf ("%f\n ",fc1);
        }

     /*  if (i % 10000== 0)

        {   //fc1=fc;
            //fc1= (float)fc / i;
            fc1= frac_cooperators(&strat);
            printf ("fc1:%f\n ",fc1);
        }*/

        // RNG_BEGIN();
        // 随机选取一个节点进行
        vid = RNG_INTEGER(0, graph_init_param->nvert - 1);
        // RNG_END();

        //printf("动态演化1!\n");
        if  (igraph_vector_binsearch2(&large,vid)||igraph_vector_binsearch2(&middle,vid)){}
    
        else if(igraph_vector_binsearch2(&cooperate_s,vid)){
            //printf("hello world");
            seg_coevolution_dynamics1(&graph, vid, &util, &strat, &ovd, &count, &environment_neigrs, /*5.16&reput,*/ 
                                    &graph_init_param->seg_coevol_dynam_param, 
                                    &count_neigrs,
                                    IGRAPH_ALL,
                                    game_func);}
          
   

       else{
           //printf("%s:%d\n", __FILE__,__LINE__);
           seg_coevolution_dynamics2(&graph, vid, &util, &strat, &ovd, &count, &environment_neigrs, /*5.16&reput,*/ 
                                    &graph_init_param->seg_coevol_dynam_param, 
                                    &count_neigrs,
                                    IGRAPH_ALL,
                                    game_func);} 
        
    }
//}
   

  /* Extend phase: for averaging pc */
    //再演化average_time_steps次并计算输出
/*{
    fc=0;

    for (i = 0; i < graph_init_param->average_time_steps; i++){

        // RNG_BEGIN();
        vid = RNG_INTEGER(0, graph_init_param->nvert - 1);
        // RNG_END();
        
        //printf("动态演化2!\n");
        if (igraph_vector_binsearch2(&large,vid)||igraph_vector_binsearch2(&middle,vid)){}
        else seg_coevolution_dynamics(&graph, vid, &util, &strat, &ovd, &count, &environment_neigrs,
                                    &graph_init_param->seg_coevol_dynam_param, 
                                    &count_neigrs,
                                    IGRAPH_ALL,
                                    game_func);
        
        fc=fc + frac_cooperators(&strat);
   
    }
}
     fc= (float)fc / graph_init_param-> average_time_steps;
     
     //printf("fc:%f\n",fc);
     printf("fc:%f\n",fc);*/
//printf("%s:%d\n", __FILE__,__LINE__);
//修改
     ave_pc = 0;
      pc = 0;
   //ave_pb = 0;
  fc=0;
    for (i = 1; i <= graph_init_param->average_time_steps; i++){

        // RNG_BEGIN();
        vid = RNG_INTEGER(0, graph_init_param->nvert - 1);
        // RNG_END();
        //printf("%s:%d\n", __FILE__,__LINE__);
      if (igraph_vector_binsearch2(&large,vid)||igraph_vector_binsearch2(&middle,vid)){}
        else if(igraph_vector_binsearch2(&cooperate_s,vid)) 
        { 
        seg_coevolution_dynamics1(&graph, vid, &util, &strat, &ovd, &count, &environment_neigrs,/*5.16&reput, */
                                    &graph_init_param->seg_coevol_dynam_param, 
                                    &count_neigrs,
                                    IGRAPH_ALL,
                                    game_func);
                          
         //printf("%s:%d\n", __FILE__,__LINE__);
        /* reputation update */
       // seg_reputation_update(&graph, &strat, &reput, graph_init_param->sigma);
         //printf("%s:%d\n", __FILE__,__LINE__);
        //fc=fc + frac_cooperators(&strat);
        //seg_interactive_neigrs(&graph, &strat, &cnount, &dncount, &interactive_neigrs,&interactive_d_neigrs);
        seg_interactive_neigrs(&graph,  &strat, &count, &interactive_neigrs);
        // printf("%s:%d\n", __FILE__,__LINE__);
       // seg_interactive_neigrs_neigrs(&graph, &strat, &ncount, &dncount, &interactive_neigrs_neigrs,&interactive_d_neigrs_neigrs);
        //printf("%s:%d\n", __FILE__,__LINE__);
        if(i == 1){
            seg_purchase_intention(&graph, &purchase_intention);
            //printf("hello world");
        }
        //printf("%s:%d\n", __FILE__,__LINE__);
       // igraph_real_t pa=0;
        //printf("%s:%d\n", __FILE__,__LINE__);
       pc =  seg_purchase_effect1(&graph,  &interactive_neigrs, &interactive, &purchase_intention,graph_init_param->seg_coevol_dynam_param.Pr1);
        //printf("保守购买\n");
         }

       else 
        { 
        seg_coevolution_dynamics2(&graph, vid, &util, &strat, &ovd, &count, &environment_neigrs,/*5.16&reput, */
                                    &graph_init_param->seg_coevol_dynam_param, 
                                    &count_neigrs,
                                    IGRAPH_ALL,
                                    game_func);
                          
         //printf("%s:%d\n", __FILE__,__LINE__);
        /* reputation update */
       // seg_reputation_update(&graph, &strat, &reput, graph_init_param->sigma);
         //printf("%s:%d\n", __FILE__,__LINE__);
        //fc=fc + frac_cooperators(&strat);
        //seg_interactive_neigrs(&graph, &strat, &cnount, &dncount, &interactive_neigrs,&interactive_d_neigrs);
        seg_interactive_neigrs(&graph,  &strat, &count, &interactive_neigrs);
        // printf("%s:%d\n", __FILE__,__LINE__);
       // seg_interactive_neigrs_neigrs(&graph, &strat, &ncount, &dncount, &interactive_neigrs_neigrs,&interactive_d_neigrs_neigrs);
        //printf("%s:%d\n", __FILE__,__LINE__);
        if(i == 1){
            seg_purchase_intention(&graph, &purchase_intention);
            //printf("hello world");
        }
        //printf("%s:%d\n", __FILE__,__LINE__);
       // igraph_real_t pa=0;
        //printf("%s:%d\n", __FILE__,__LINE__);
       pc =  seg_purchase_effect2(&graph,  &interactive_neigrs, &interactive, &purchase_intention,graph_init_param->seg_coevol_dynam_param.Pr2);
         //printf("%s:%d\n", __FILE__,__LINE__);
         }
        
           // printf("%lf",pc);
          //  printf("");
        //printf("%s:%d\n", __FILE__,__LINE__);
        ave_pc = ave_pc + pc;
        //ave_pb = ave_pb + pb;
       fc = fc + frac_cooperators(&strat);
    }
     fc= (float)fc / graph_init_param-> average_time_steps;
     ave_pc = (float)ave_pc / graph_init_param-> average_time_steps;
     //ave_pb = (float)ave_pb / graph_init_param-> average_time_steps;
    
     
     printf("pc:%f\n", ave_pc);
    //printf("b:%f\n", ave_pb);  
   //printf("%s:%d\n", __FILE__,__LINE__);
//
    

    RNG_END();

    /* clean up */
    igraph_destroy(&graph);
    igraph_vector_destroy(&strat);
    igraph_vector_destroy(&util);
    igraph_vector_destroy(&vd);
    igraph_vector_destroy(&ivd);
    igraph_vector_destroy(&ovd);

    igraph_vector_destroy(&large);
    igraph_vector_destroy(&middle);
    igraph_vector_destroy(&small);

    igraph_vector_destroy(&cooperate_l);
    igraph_vector_destroy(&cooperate_m);
    igraph_vector_destroy(&cooperate_s);
    //5.16igraph_vector_destroy(&reput);
    igraph_vector_destroy(&count_neigrs.v);
    igraph_vector_destroy(&environment_neigrs);
    igraph_vector_destroy(&count);
//9.26
   //igraph_vector_destroy(&cnount);
    //igraph_vector_destroy(&dncount);
    igraph_vector_destroy(&interactive);
    //igraph_vector_destroy(&interactive_d);
    igraph_vector_destroy(&interactive_neigrs);
    //igraph_vector_destroy(&interactive_d_neigrs);
    igraph_vector_destroy(&purchase_intention);
   //
    return IGRAPH_SUCCESS;
}
#ifndef symboltable_HEADER
#define symboltable_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "variable.h"

struct st_s {
    struct st_node_s * nodes;
    struct st_s * next;
    struct st_s * top;
};

struct st_node_s {
    char * symbol;
    struct var_s * var;
    int level;
    struct st_node_s * left;
    struct st_node_s * right;
};


struct st_s *  st_create  ();
void           st_destroy (struct st_s * st);
struct st_s *  st_push    (struct st_s * st, struct st_s * node);
struct st_s *  st_pop     (struct st_s * st);
void           st_insert  (struct st_s * st, char * symbol, struct var_s * var);
struct var_s * st_find    (struct st_s * st, char * symbol);
void           st_delete  (struct st_s * st, char * symbol);
void           st_debug   (struct st_s * st);
struct var_s * st_find_scoped (struct st_s * st, char * symbol);



struct st_node_s * st_node_skew    (struct st_node_s * tree);
struct st_node_s * st_node_split   (struct st_node_s * tree);
struct st_node_s * st_node_insert  (struct st_node_s * tree,
                                    struct st_node_s * node);
struct st_node_s * st_node_find    (struct st_node_s * tree, char * symbol);
struct st_node_s * st_node_delete  (struct st_node_s * tree, char * symbol);
void               st_node_destroy (struct st_node_s * node);
void               st_node_debug   (struct st_node_s * tree, int depth);
struct st_node_s * st_node_predecessor     (struct st_node_s * tree);
struct st_node_s * st_node_successor       (struct st_node_s * tree);
void               st_node_delete_contents (struct st_node_s * node);
struct st_node_s * st_node_decrease_level  (struct st_node_s * tree);
                                   
#endif
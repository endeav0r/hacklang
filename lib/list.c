#include "list.h"

void lib_list_register (struct in_s * in)
{
    capi_register_function(in, lib_list_create,        "list");
    capi_register_function(in, lib_list_append,        "list_append");
    capi_register_function(in, lib_list_size,          "list_size");
    capi_register_function(in, lib_list_iter_create,   "iter");
    capi_register_function(in, lib_list_iter_next,     "iter_next");
    capi_register_function(in, lib_list_iter_continue, "iter_continue");
}


struct lib_list_s * lib_list_api_create ()
{
    struct lib_list_s * list;
    
    list = (struct lib_list_s *) malloc(sizeof(struct lib_list_s));
    list->type = LIST_TYPE_LIST;
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    
    return list;
}


void lib_list_api_destroy (struct lib_list_s * list)
{
    struct lib_list_item_s * item;
    struct lib_list_item_s * next;
    
    item = list->first;
    while (item != NULL) {
        next = item->next;
        var_destroy(item->var);
        free(item);
        item = next;
    }
    
    free(list);
}


void lib_list_api_append (struct lib_list_s * list, struct var_s * var)
{
    struct lib_list_item_s * item;
    
    item = (struct lib_list_item_s *) malloc(sizeof(struct lib_list_item_s));
    item->var = var;
    item->next = NULL;
    
    if (list->last == NULL) {
        item->prev = NULL;
        list->first = item;
        list->last = item;
    }
    else {
        list->last->next = item;
        item->prev = list->last;
        list->last = item;
    }
    list->size++;
}


int lib_list_api_size (struct lib_list_s * list)
{
    return list->size;
}


int lib_list_create (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_list_s * list;
    
    if (capi_size(capi) == 0) {
        list = lib_list_api_create();
        var = var_create_cdata(list, lib_list_copy, lib_list_free);
        capi_push(capi, var);
        return 1;
    }
    else {
        fprintf(stderr, "LIB_LIST_LIST: TAKES NO ARGUMENTS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_list_append (struct capi_s * capi)
{
    struct lib_list_s * list;
    
    if (capi_size(capi) == 2) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_LIST_APPEND_0: FIRST ARG MUST BE LIST (%d)\n",
                    capi_type(capi, 0));
            exit(-1);
        }
        list = capi_to_cdata(capi, 0);
        if (list->type != LIST_TYPE_LIST) {
            fprintf(stderr, "LIB_LIST_APPEND_1: FIRST ARG MUST BE LIST\n");
            exit(-1);
        }
        
        lib_list_api_append(list, var_copy(capi_to_var(capi, 1)));
        
        capi_pop(capi);
        capi_pop(capi);
    }
    else {
        fprintf(stderr, "LIB_LIST_APPEND_2: TAKES TWO ARGS (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 0;
}


int lib_list_size (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_list_s * list;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_LIST_SIZE: ONLY ARG MUST BE LIST\n");
            exit(-1);
        }
        list = (struct lib_list_s *) capi_to_cdata(capi, 0);
        if (list->type != LIST_TYPE_LIST) {
            fprintf(stderr, "LIB_LIST_SIZE: ONLY ARG MUST BE LIST\n");
            exit(-1);
        }
        
        var = var_create(TYPE_INT, NULL);
        var->i = lib_list_api_size(list);
        
        capi_pop(capi);
        capi_push(capi, var);
    }
    else {
        fprintf(stderr, "LIB_LIST_SIZE: ONLY ARG MUST BE LIST (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 1;
}


void * lib_list_copy (void * data)
{
    struct lib_list_s * list;
    struct lib_list_s * new_list;
    struct lib_list_item_s * list_item;
    struct lib_list_item_s * new_list_item;
    struct lib_list_item_s * new_list_item_prev;
    
    list = (struct lib_list_s *) data;
    
    new_list = (struct lib_list_s *) malloc(sizeof(struct lib_list_s));
    
    new_list->type = LIST_TYPE_LIST;
    new_list->size = list->size;
    new_list->first = NULL;
    new_list->last = NULL;
    
    list_item = NULL;
    
    if (list->first != NULL) {
        new_list_item_prev = (struct lib_list_item_s *) malloc(sizeof(struct lib_list_item_s));
        new_list_item_prev->var = var_copy(list->first->var);
        new_list_item_prev->prev = NULL;
        new_list_item_prev->next = NULL;
        new_list->first = new_list_item_prev;
        new_list->last = new_list_item_prev;
        list_item = list->first->next;
    }
    
    while (list_item != NULL) {
        new_list_item = (struct lib_list_item_s *) malloc(sizeof(struct lib_list_item_s));
        new_list_item->var = var_copy(list->first->var);
        new_list_item->prev = new_list_item_prev;
        new_list->last = new_list_item;
        new_list_item_prev = new_list_item;
        list_item = list_item->next;
    }
    
    return (void *) new_list;
}


void lib_list_free (void * data)
{
    struct lib_list_s * list;
        
    list = (struct lib_list_s *) data;
    
    lib_list_api_destroy(list);
}


int lib_list_iter_create (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_list_s * list;
    struct lib_list_iter_s * iter;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_LIST_ITER: ONLY ARG MUST BE LIST\n");
            exit(-1);
        }
        list = (struct lib_list_s *) capi_to_cdata(capi, 0);
        if (list->type != LIST_TYPE_LIST) {
            fprintf(stderr, "LIB_LIST_ITER: ONLY ARG MUST BE LIST\n");
            exit(-1);
        }
        
        iter = (struct lib_list_iter_s *) malloc(sizeof(struct lib_list_iter_s));
        iter->type = LIST_TYPE_ITER;
        iter->cur = list->first;
        
        var = var_create_cdata(iter, lib_list_iter_copy, lib_list_iter_free);
        
        capi_pop(capi);
        capi_push(capi, var);
    }
    else {
        fprintf(stderr, "LIB_LIST_ITER: ONLY ARG MUST BE LIST (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 1;
}


int lib_list_iter_next (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_list_iter_s * iter;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_LIST_ITER_NEXT: ONLY ARG MUST BE ITER\n");
            exit(-1);
        }
        iter = (struct lib_list_iter_s *) capi_to_cdata(capi, 0);
        if (iter->type != LIST_TYPE_ITER) {
            fprintf(stderr, "LIB_LIST_ITER_NEXT: ONLY ARG MUST BE ITER\n");
            exit(-1);
        }
        
        if (iter->cur == NULL) {
            fprintf(stderr, "LIB_LIST_ITER_NEXT: CALLED NEXT ON NULL ITER\n");
            exit(-1);
        }
        
        var = var_copy(iter->cur->var);
        iter->cur = iter->cur->next;
        
        capi_pop(capi);
        capi_push(capi, var);
    }
    else {
        fprintf(stderr, "LIB_LIST_ITER_NEXT: ONLY ARG MUST BE ITER (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 1;
}


int lib_list_iter_continue (struct capi_s * capi)
{
    struct var_s * var;
    struct lib_list_iter_s * iter;
    
    if (capi_size(capi) == 1) {
        if (capi_type(capi, 0) != CAPI_TYPE_CDATA) {
            fprintf(stderr, "LIB_LIST_ITER_CONTINUE: ONLY ARG MUST BE ITER\n");
            exit(-1);
        }
        iter = (struct lib_list_iter_s *) capi_to_cdata(capi, 0);
        if (iter->type != LIST_TYPE_ITER) {
            fprintf(stderr, "LIB_LIST_ITER_CONTINUE: ONLY ARG MUST BE ITER\n");
            exit(-1);
        }
        
        var = var_create(TYPE_BOOL, NULL);
        if (iter->cur == NULL)
            var->bool = FALSE;
        else
            var->bool = TRUE;
        
        capi_pop(capi);
        capi_push(capi, var);
    }
    else {
        fprintf(stderr, "LIB_LIST_ITER_CONTINUE: ONLY ARG MUST BE ITER (%d)\n",
                capi_size(capi));
        exit(-1);
    }
    
    return 1;
}


void * lib_list_iter_copy (void * data)
{
    struct lib_list_iter_s * iter;
    struct lib_list_iter_s * new_iter;
    
    iter = (struct lib_list_iter_s *) data;
    
    if (iter == NULL)
        return NULL;
    
    new_iter = (struct lib_list_iter_s *) malloc(sizeof(struct lib_list_iter_s));
    new_iter->type = LIST_TYPE_ITER;
    new_iter->cur = iter->cur;
    
    return new_iter;
}


void lib_list_iter_free (void * data)
{
    free(data);
}
#include "symboltable.h"


struct st_s * st_create () {
    struct st_s * st;
    
    st = (struct st_s *) malloc(sizeof(struct st_s));
    st->nodes = NULL;
    st->next = NULL;
    
    return st;
}


void st_destroy (struct st_s * st) {
    if (st->next != NULL)
        st_destroy(st->next);
        
    st_node_destroy(st->nodes);
    free(st);
}


struct st_s * st_push (struct st_s * st) {
    struct st_s * new = st_create();
    new->next = st;
    return new;
}


struct st_s * st_pop (struct st_s * st) {
    struct st_s * next;
    next = st->next;
    
    st_node_destroy(st->nodes);
    free(st);
    
    return next;
}
        

void st_insert (struct st_s * st, char * symbol, struct var_s * var) {
    struct st_node_s * node;
    
    node = (struct st_node_s *) malloc(sizeof(struct st_node_s));
    node->symbol = (char *) malloc(strlen(symbol) + 1);
    strcpy(node->symbol, symbol);
    node->var = var;
    node->level = 0;
    node->left = NULL;
    node->right = NULL;
    
    st->nodes = st_node_insert(st->nodes, node);
}


struct var_s * st_find (struct st_s * st, char * symbol) {
    struct st_node_s * node;
    
    while (st != NULL) {
        node = st_node_find(st->nodes, symbol);
        if (node != NULL)
            return node->var;
        st = st->next;
    }
    return NULL;
}


void st_delete (struct st_s * st, char * symbol) {
    st->nodes = st_node_delete(st->nodes, symbol);
}


void st_debug (struct st_s * st) {
    int i;
    for (i = 0; st != NULL; i++) {
        printf("st_stack %d\n", i);
        st_node_debug(st->nodes, 1);
        st = st->next;
    }
}


struct var_s * st_find_scoped (struct st_s * st, char * symbol) {
    struct st_node_s * node;
    node = st_node_find(st->nodes, symbol);
    if (node != NULL)
        return node->var;
    return NULL;
}


struct st_node_s * st_node_skew (struct st_node_s * tree) {
    struct st_node_s * L;
    if (tree == NULL)
        return NULL;
    if (tree->left != NULL) {
        if (tree->left->level == tree->level) {
            L = tree->left;
            tree->left = L->right;
            L->right = tree;
            return L;
        }
    }
    return tree;
}


struct st_node_s * st_node_split (struct st_node_s * tree) {
    struct st_node_s * R;
    if (tree == NULL)
        return NULL;
    if (tree->right != NULL) {
        if (tree->right->right != NULL) {
            if (tree->right->right->level == tree->level) {
                R = tree->right;
                tree->right = R->left;
                R->left = tree;
                R->level++;
                return R;
            }
        }
    }
    return tree;
}


struct st_node_s * st_node_insert (struct st_node_s * tree,
                                   struct st_node_s * node) {
    int cmp;
    if (tree == NULL)
        return node;
    
    cmp = strcmp(tree->symbol, node->symbol);
    if (cmp < 0)
        tree->left = st_node_insert(tree->left, node);
    else if (cmp > 0)
        tree->right = st_node_insert(tree->right, node);
    else {
        fprintf(stderr, "duplicate symbol %s %s\n", node->symbol, tree->symbol);
        exit(-1);
    }
    
    tree = st_node_skew(tree);
    tree = st_node_split(tree);

    return tree;
}


struct st_node_s * st_node_find (struct st_node_s * tree, char * symbol) {
    int cmp;
    if (tree == NULL)
        return NULL;
    cmp = strcmp(tree->symbol, symbol);
    if (cmp < 0)
        return st_node_find(tree->left, symbol);
    if (cmp > 0)
        return st_node_find(tree->right, symbol);
    else
        return tree;
}


struct st_node_s * st_node_decrease_level (struct st_node_s * tree) {
    int should_be;
    if (tree == NULL)
        return NULL;
    if (tree->left != NULL) {
        if ((tree->left->left != NULL) && (tree->right != NULL)) {
            should_be = tree->left->left->level;
            if (should_be > tree->right->level)
                should_be = tree->right->level;
            should_be++;
            if (should_be < tree->right->level)
                tree->right->level = should_be;
        }
    }
    return tree;
}


void st_node_delete_contents (struct st_node_s * node) {
    if (node->symbol != NULL)
        free(node->symbol);
    if (node->var != NULL)
        var_destroy(node->var);
}


struct st_node_s * st_node_successor (struct st_node_s * tree) {
    struct st_node_s * node;
    
    if (tree == NULL)
        return NULL;
    
    node = tree->right;
    while (node->left != NULL)
        node = node->left;
    
    return node;
}


struct st_node_s * st_node_predecessor (struct st_node_s * tree) {
    struct st_node_s * node;
    
    if (tree == NULL)
        return NULL;
    
    node = tree->left;
    while (node->right != NULL)
        node = node->right;
    
    return node;
}


struct st_node_s * st_node_delete (struct st_node_s * tree, char * symbol) {
    int cmp;
    struct st_node_s * L;
    
    if (tree == NULL)
        return NULL;
    
    cmp = strcmp(tree->symbol, symbol);
    if (cmp < 0)
        tree->left = st_node_delete(tree->left, symbol);
    else if (cmp > 0)
        tree->right = st_node_delete(tree->right, symbol);
    else {
        if ((tree->left == NULL) && (tree->right == NULL)) {
            st_node_destroy(tree);
            return NULL;
        }
        else if (tree->left == NULL) {
            L = st_node_successor(tree);
            st_node_delete_contents(tree);
            tree->symbol = L->symbol;
            tree->var    = L->var;
            L->symbol = NULL;
            L->var    = NULL;
            tree->right = st_node_delete(tree->right, NULL);
        }
        else {
            L = st_node_predecessor(tree);
            st_node_delete_contents(tree);
            tree->symbol = L->symbol;
            tree->var    = L->var;
            L->symbol = NULL;
            L->var    = NULL;
            tree->left = st_node_delete(tree->left, NULL);
        }
    }
    
    tree = st_node_decrease_level(tree);
    tree = st_node_skew(tree);
    if (tree->right != NULL) {
        tree->right = st_node_skew(tree->right);
        if (tree->right->right != NULL) {
            tree->right->right = st_node_skew(tree->right->right);
        }
    }
    tree = st_node_split(tree);
    if (tree->right != NULL)
        tree->right = st_node_split(tree->right);
    
    return tree;
}


void st_node_destroy (struct st_node_s * node) {
    if (node == NULL)
        return;
    
    st_node_destroy(node->left);
    st_node_destroy(node->right);

    st_node_delete_contents(node);
    free(node);
}


void st_node_debug (struct st_node_s * tree, int depth) {
    int i;
    if (tree == NULL)
        return;

    for (i = 0; i < depth; i++)
        printf(" ");
    
    printf("%s ", tree->symbol);fflush(stdout);
    printf("%s %d\n", var_to_string(tree->var), tree->level);fflush(stdout);
    
    if (tree->left != NULL)
        printf("l");
    st_node_debug(tree->left, depth + 1);
    if (tree->right != NULL)
        printf("r");
    st_node_debug(tree->right, depth + 1);
}
#include "symboltable.h"


struct st_s * st_skew (struct st_s * tree) {
	struct st_s * L;
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


struct st_s * st_split (struct st_s * tree) {
	struct st_s * R;
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


struct st_s * st_insert_node (struct st_s * tree, struct st_s * node) {
	int cmp;
	if (tree == NULL)
		return node;
	
	cmp = strcmp(tree->symbol, node->symbol);
	if (cmp < 0)
		tree->left = st_insert_node(tree->left, node);
	else if (cmp > 0)
		tree->right = st_insert_node(tree->right, node);
	else {
		fprintf(stderr, "duplicate symbol %s\n", node->symbol);
		exit(-1);
	}
	
	tree = st_skew(tree);
	tree = st_split(tree);

	return tree;
}


struct st_s * st_insert (struct st_s * tree, char * symbol, struct var_s * var) {
	struct st_s * node;
	
	node = (struct st_s *) malloc(sizeof(struct st_s));
	node->symbol = (char *) malloc(strlen(symbol) + 1);
	strcpy(node->symbol, symbol);
	node->var = var;
	node->level = 0;
	node->left = NULL;
	node->right = NULL;
	
	return st_insert_node(tree, node);
}


struct st_s * st_find_node (struct st_s * tree, char * symbol) {
	int cmp;
	if (tree == NULL)
		return NULL;
	cmp = strcmp(tree->symbol, symbol);
	if (cmp < 0)
		return st_find_node(tree->left, symbol);
	if (cmp > 0)
		return st_find_node(tree->right, symbol);
	else
		return tree;
}


struct var_s * st_find (struct st_s * tree, char * symbol) {
	struct st_s * node;
	
	node = st_find_node(tree, symbol);
	if (node == NULL)
		return NULL;
	return node->var;
}


struct st_s * st_decrease_level (struct st_s * tree) {
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


void st_delete_node_contents (struct st_s * node) {
	if (node->symbol != NULL)
		free(node->symbol);
	if (node->var != NULL)
		var_destroy(node->var);
}


void st_delete_node (struct st_s * node) {
	if (node != NULL) {
		st_delete_node_contents(node);
		free(node);
	}
}


struct st_s * st_successor (struct st_s * tree) {
	struct st_s * node;
	
	if (tree == NULL)
		return NULL;
	
	node = tree->right;
	while (node->left != NULL)
		node = node->left;
	
	return node;
}


struct st_s * st_predecessor (struct st_s * tree) {
	struct st_s * node;
	
	if (tree == NULL)
		return NULL;
	
	node = tree->left;
	while (node->right != NULL)
		node = node->right;
	
	return node;
}


struct st_s * st_delete (struct st_s * tree, char * symbol) {
	int cmp;
	struct st_s * L;
	
	if (tree == NULL)
		return NULL;
	
	cmp = strcmp(tree->symbol, symbol);
	if (cmp < 0)
		tree->left = st_delete(tree->left, symbol);
	else if (cmp > 0)
		tree->right = st_delete(tree->right, symbol);
	else {
		if ((tree->left == NULL) && (tree->right == NULL)) {
			st_delete_node(tree);
			return NULL;
		}
		else if (tree->left == NULL) {
			L = st_successor(tree);
			st_delete_node_contents(tree);
			tree->symbol = L->symbol;
			tree->var    = L->var;
			L->symbol = NULL;
			L->var    = NULL;
			tree->right = st_delete(tree->right, NULL);
		}
		else {
			L = st_predecessor(tree);
			st_delete_node_contents(tree);
			tree->symbol = L->symbol;
			tree->var    = L->var;
			L->symbol = NULL;
			L->var    = NULL;
			tree->left = st_delete(tree->left, NULL);
		}
	}
	
	tree = st_decrease_level(tree);
	tree = st_skew(tree);
	if (tree->right != NULL) {
		tree->right = st_skew(tree->right);
		if (tree->right->right != NULL) {
			tree->right->right = st_skew(tree->right->right);
		}
	}
	tree = st_split(tree);
	if (tree->right != NULL)
		tree->right = st_split(tree->right);
	
	return tree;
}


void st_destroy (struct st_s * tree) {
	if (tree == NULL)
		return;
	
	st_destroy(tree->left);
	st_destroy(tree->right);
	st_delete_node(tree);
}


void st_debug_node (struct st_s * tree, int depth) {
	int i;
	if (tree == NULL)
		return;

	for (i = 0; i < depth; i++)
		printf(" ");
	
	printf("%s %s %d\n", tree->symbol, var_to_string(tree->var), tree->level);
	
	if (tree->left != NULL)
		printf("l");
	st_debug_node(tree->left, depth + 1);
	if (tree->right != NULL)
		printf("r");
	st_debug_node(tree->right, depth + 1);
}


void st_debug (struct st_s * tree) {
	if (tree == NULL)
		return;
	st_debug_node(tree, 0);
}
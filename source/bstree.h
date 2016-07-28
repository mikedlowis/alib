/**
  @brief Simple binary search tree implementation.
  @author Michael D. Lowis
  @license BSD 2-clause License
*/

typedef struct bstree_node_t {
    struct bstree_node_t* left;
    struct bstree_node_t* right;
} bstree_node_t;

typedef int (*bstree_cmpfn_t)(bstree_node_t* a, bstree_node_t* b);

typedef struct {
    bstree_node_t* root;
    bstree_cmpfn_t cmpfn;
    bool allow_dups;
} bstree_t;

static void bstree_init(bstree_t* tree, bstree_cmpfn_t cmpfn, bool allow_dups) {
    tree->root = NULL;
    tree->cmpfn = cmpfn;
    tree->allow_dups = allow_dups;
}

static bool bstree_empty(bstree_t* tree) {
    return (tree->root == NULL);
}

static size_t subtree_size(bstree_node_t* node) {
    if (node == NULL)
        return 0;
    else
        return (1 + subtree_size(node->left) + subtree_size(node->right));
}

static size_t bstree_size(bstree_t* tree) {
    return subtree_size(tree->root);
}

static bstree_node_t** find_node(bstree_cmpfn_t cmpfn, bstree_node_t** root, bstree_node_t* node, bool allow_dups) {
    bstree_node_t** curr = root;
    while(*curr != NULL) {
        int cmp = cmpfn(node, *curr);
        if (cmp < 0)
            curr = &((*curr)->left);
        else if (cmp > 0)
            curr = &((*curr)->right);
        else if (allow_dups)
            curr = &((*curr)->right);
        else
            break;
    }
    return curr;
}

static void bstree_insert(bstree_t* tree, bstree_node_t* node) {
    bstree_node_t** curr = find_node(tree->cmpfn, &(tree->root), node, tree->allow_dups);
    if (*curr == NULL) {
        *curr = node;
        node->left = NULL;
        node->right = NULL;
    }
}

static bstree_node_t* bstree_lookup(bstree_t* tree, bstree_node_t* node) {
    bstree_node_t** curr = find_node(tree->cmpfn, &(tree->root), node, false);
    return *curr;
}

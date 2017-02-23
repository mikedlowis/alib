/*
    Simple binary search tree implementation.
    
    Copyright 2017, Michael D. Lowis
    
    Permission to use, copy, modify, and/or distribute this software
    for any purpose with or without fee is hereby granted, provided
    that the above copyright notice and this permission notice appear
    in all copies.
    
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
    OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.
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

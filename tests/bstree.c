// Unit Test Framework Includes
#include "atf.h"

// File To Test
#include <util.h>
#include <bstree.h>

typedef struct {
    bstree_node_t node;
    int val;
} int_node_t;

int compare(bstree_node_t* a, bstree_node_t* b)
{
    int_node_t* nodea = container_of(a, int_node_t, node);
    int_node_t* nodeb = container_of(b, int_node_t, node);
    return (nodea->val < nodeb->val) ? -1 : (nodea->val > nodeb->val) ? 1 : 0;
}

//-----------------------------------------------------------------------------
// Begin Unit Tests
//-----------------------------------------------------------------------------
TEST_SUITE(BSTree) {
    //-------------------------------------------------------------------------
    // bstree_init
    //-------------------------------------------------------------------------
    TEST(Verify_bstree_init_should_init_the_tree)
    {
        bstree_t tree;
        bstree_init(&tree, NULL, true);
        CHECK(tree.root == NULL);
        CHECK(tree.cmpfn == NULL);
        CHECK(tree.allow_dups == true);
    }

    //-------------------------------------------------------------------------
    // bstree_empty
    //-------------------------------------------------------------------------
    TEST(Verify_bstree_empty_should_return_true_if_the_tree_is_empty)
    {
        bstree_t tree = { 0, 0, 0 };
        CHECK(bstree_empty(&tree));
    }

    TEST(Verify_bstree_empty_should_return_false_if_the_tree_is_not_empty)
    {
        bstree_t tree = { (bstree_node_t*)0x1234, NULL, false };
        CHECK(!bstree_empty(&tree));
    }

    //-------------------------------------------------------------------------
    // bstree_size
    //-------------------------------------------------------------------------
    TEST(Verify_bstree_size_should_return_0_for_empty_tree)
    {
        bstree_t tree = { 0, 0, 0 };
        CHECK(0 == bstree_size(&tree));
    }

    TEST(Verify_bstree_size_should_return_1)
    {
        bstree_node_t node1 = { NULL, NULL };
        bstree_t tree = { &node1, NULL, false };
        CHECK(1 == bstree_size(&tree));
    }

    TEST(Verify_bstree_size_should_return_2)
    {
        bstree_node_t node2 = { NULL, NULL };
        bstree_node_t node1 = { &node2, NULL };
        bstree_t tree = { &node1, NULL, false };
        CHECK(2 == bstree_size(&tree));
    }

    TEST(Verify_bstree_size_should_return_3)
    {
        bstree_node_t node3 = { NULL, NULL };
        bstree_node_t node2 = { NULL, NULL };
        bstree_node_t node1 = { &node2, &node3 };
        bstree_t tree = { &node1, NULL, false };
        CHECK(3 == bstree_size(&tree));
    }

    //-------------------------------------------------------------------------
    // bstree_insert
    //-------------------------------------------------------------------------
    TEST(Verify_bstree_insert_should_insert_into_empty_tree)
    {
        int_node_t node = { {0,0}, 42 };
        bstree_t tree = { NULL, compare, false };
        bstree_insert(&tree, &(node.node));
        CHECK(tree.root = &(node.node));
    }

    TEST(Verify_bstree_insert_should_insert_to_the_left)
    {
        int_node_t node1 = { {0,0}, 42 };
        int_node_t node2 = { {0,0}, 41 };
        bstree_t tree = { &(node1.node), compare, false };
        bstree_insert(&tree, &(node2.node));
        CHECK(tree.root->left = &(node2.node));
    }

    TEST(Verify_bstree_insert_should_insert_to_the_right)
    {
        int_node_t node1 = { {0,0}, 42 };
        int_node_t node2 = { {0,0}, 43 };
        bstree_t tree = { &(node1.node), compare, false };
        bstree_insert(&tree, &(node2.node));
        CHECK(tree.root->right = &(node2.node));
    }

    //-------------------------------------------------------------------------
    // bstree_lookup
    //-------------------------------------------------------------------------
    TEST(Verify_bstree_lookup_should_not_find_the_item_in_an_empty_tree)
    {
        int_node_t node1 = { {0,0}, 42 };
        bstree_t tree = { NULL, compare, false };
        CHECK(bstree_lookup(&tree, &(node1.node)) == NULL);
    }

    TEST(Verify_bstree_lookup_should_find_the_item_at_the_root)
    {
        int_node_t node1 = { {0,0}, 42 };
        bstree_t tree = { &(node1.node), compare, false };
        CHECK(bstree_lookup(&tree, &(node1.node)) == &(node1.node));
    }

    TEST(Verify_bstree_lookup_should_find_the_item_to_the_left)
    {
        int_node_t node2 = { {0,0}, 41 };
        int_node_t node1 = { {&(node2.node),0}, 42 };
        bstree_t tree = { &(node1.node), compare, false };
        CHECK(bstree_lookup(&tree, &(node2.node)) == &(node2.node));
    }

    TEST(Verify_bstree_lookup_should_find_the_item_to_the_right)
    {
        int_node_t node2 = { {0,0}, 43 };
        int_node_t node1 = { {0,&(node2.node)}, 42 };
        bstree_t tree = { &(node1.node), compare, false };
        CHECK(bstree_lookup(&tree, &(node2.node)) == &(node2.node));
    }

}

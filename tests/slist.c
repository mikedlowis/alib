// Unit Test Framework Includes
#include "atf.h"

// File To Test
#include <slist.h>

//-----------------------------------------------------------------------------
// Begin Unit Tests
//-----------------------------------------------------------------------------
TEST_SUITE(SList) {
    //-------------------------------------------------------------------------
    // slist_init
    //-------------------------------------------------------------------------
    TEST(Verify_slist_init_initializesr_the_list)
    {
        slist_t list;
        slist_init(&list);
        CHECK(list.head == NULL);
    }

    //-------------------------------------------------------------------------
    // slist_empty
    //-------------------------------------------------------------------------
    TEST(Verify_slist_empty_returns_true_when_list_is_empty)
    {
        slist_t list = { NULL };
        CHECK(true == slist_empty(&list));
    }

    TEST(Verify_slist_empty_returns_false_when_list_is_not_empty)
    {
        slist_t alist = { (slist_node_t*)0x1234 };
        CHECK(false == slist_empty(&alist));
    }

    //-------------------------------------------------------------------------
    // slist_size
    //-------------------------------------------------------------------------
    TEST(Verify_slist_size_should_return_0)
    {
        slist_t list = { 0 };
        CHECK(0 == slist_size(&list));
    }

    TEST(Verify_slist_size_should_return_1)
    {
        slist_node_t node = { NULL };
        slist_t list = { &node };
        CHECK(1 == slist_size(&list));
        CHECK(node.next == NULL);
    }

    TEST(Verify_slist_size_should_return_2)
    {
        slist_node_t node2 = { NULL };
        slist_node_t node1 = { &node2 };
        slist_t list = { &node1 };
        CHECK(2 == slist_size(&list));
        CHECK(node1.next == &node2);
        CHECK(node2.next == NULL);
    }

    //-------------------------------------------------------------------------
    // slist_front
    //-------------------------------------------------------------------------
    TEST(Verify_slist_front_should_return_the_head)
    {
        slist_node_t node2 = { NULL };
        slist_node_t node1 = { &node2 };
        slist_t list = { &node1 };
        CHECK(&node1 == slist_front(&list));
    }

    //-------------------------------------------------------------------------
    // slist_push_front
    //-------------------------------------------------------------------------
    TEST(Verify_slist_push_front_should_push_the_node_to_the_head)
    {
        slist_node_t node = { NULL };
        slist_t list = { 0 };
        slist_push_front(&list, &node);
        CHECK(!slist_empty(&list));
        CHECK(list.head == &node);
    }

    //-------------------------------------------------------------------------
    // slist_pop_front
    //-------------------------------------------------------------------------
    TEST(Verify_slist_pop_front_should_remove_the_head_element_from_the_list_and_return_it)
    {
        slist_node_t node = { NULL };
        slist_t list = { &node };
        CHECK(&node == slist_pop_front(&list));
        CHECK(slist_empty(&list));
        CHECK(list.head == NULL);
    }

    //-------------------------------------------------------------------------
    // slist_back
    //-------------------------------------------------------------------------
    TEST(Verify_slist_back_should_return_NULL_if_the_list_is_empty)
    {
        slist_t list = { NULL };
        CHECK(NULL == slist_back(&list));
    }

    TEST(Verify_slist_back_should_return_the_head_if_only_one_item_in_list)
    {
        slist_node_t node = { NULL };
        slist_t list = { &node };
        CHECK(&node == slist_back(&list));
    }

    TEST(Verify_slist_back_should_return_the_last_item_if_two_items_in_list)
    {
        slist_node_t node2 = { NULL };
        slist_node_t node1 = { &node2 };
        slist_t list = { &node1 };
        CHECK(&node2 == slist_back(&list));
    }

    //-------------------------------------------------------------------------
    // slist_push_back
    //-------------------------------------------------------------------------
    TEST(Verify_slist_push_back_should_push_to_the_head)
    {
        slist_node_t node = { NULL };
        slist_t list = { NULL };
        slist_push_back(&list, &node);
        CHECK(&node == list.head);
    }

    TEST(Verify_slist_push_back_should_push_to_the_tail)
    {
        slist_node_t node2 = { NULL };
        slist_node_t node1 = { NULL };
        slist_t list = { &node1 };
        slist_push_back(&list, &node2);
        CHECK(&node2 == list.head->next);
    }

    //-------------------------------------------------------------------------
    // slist_pop_back
    //-------------------------------------------------------------------------
    TEST(Verify_slist_pop_back_should_do_nothing_if_list_empty)
    {
        slist_t list = { NULL };
        CHECK(NULL == slist_pop_back(&list));
        CHECK(NULL == list.head);
    }

    TEST(Verify_slist_pop_back_should_remove_the_only_item_in_the_list)
    {
        slist_node_t node = { NULL };
        slist_t list = { &node };
        CHECK(&node == slist_pop_back(&list));
        CHECK(NULL == list.head);
    }

    TEST(Verify_slist_pop_back_should_remove_the_last_item_in_the_list)
    {
        slist_node_t node2 = { NULL };
        slist_node_t node1 = { &node2 };
        slist_t list = { &node1 };
        CHECK(&node2 == slist_pop_back(&list));
        CHECK(&node1 == list.head);
        CHECK(NULL == list.head->next);
    }

    //-------------------------------------------------------------------------
    // slist_node_has_next
    //-------------------------------------------------------------------------
    TEST(Verify_slist_has_next_should_return_false_if_null)
    {
        slist_node_t node = { NULL };
        CHECK(!slist_node_has_next(&node));
    }

    TEST(Verify_slist_has_next_should_return_false_if_not_null)
    {
        slist_node_t node = { (slist_node_t*)0x1234 };
        CHECK(slist_node_has_next(&node));
    }

    //-------------------------------------------------------------------------
    // slist_node_next
    //-------------------------------------------------------------------------
    TEST(Verify_slist_node_next_should_return_the_next_node)
    {
        slist_node_t node = { (slist_node_t*)0x1234 };
        CHECK((slist_node_t*)0x1234 == slist_node_next(&node));
    }
}


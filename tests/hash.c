// Unit Test Framework Includes
#include "atf.h"

// File To Test
#include <util.h>
#include <hash.h>

static const uint Num_Iterations = 1000000;

typedef struct {
    hash_entry_t link;
    uint val;
} int_node_t;

static unsigned int hash_func(const hash_entry_t* entry)
{
    int_node_t* node = container_of(entry, int_node_t, link);
    return node->val;
}

static int compare_func(const hash_entry_t* entry1, const hash_entry_t* entry2)
{
    int_node_t* node1 = container_of(entry1, int_node_t, link);
    int_node_t* node2 = container_of(entry2, int_node_t, link);
    return node1->val - node2->val;
}

static void delete_func(hash_entry_t* entry)
{
    free(container_of(entry, int_node_t, link));
}

//-----------------------------------------------------------------------------
// Begin Unit Tests
//-----------------------------------------------------------------------------
TEST_SUITE(Hash) {
    TEST(Verify sequential hash inserts and lookups)
    {
        hash_t hash;
        hash_init(&hash, hash_func, compare_func, delete_func);
        for (uint i = 0; i < Num_Iterations; i++)
        {
            int_node_t* entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = i;
            hash_set(&hash, &(entry->link));
            CHECK(i+1 == hash_size(&hash));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
        }
        for (uint i = 0; i < Num_Iterations; i++)
        {
            int_node_t search = { .val = i };
            hash_entry_t* entry = hash_get(&hash, &(search.link));
            int_node_t* ientry = container_of(entry, int_node_t, link);
            CHECK(entry != NULL);
            CHECK(ientry != NULL);
            CHECK(search.val == ientry->val);
        }
        hash_deinit(&hash);
    }

    TEST(Verify ping pong hash inserts and lookups)
    {
        hash_t hash;
        hash_init(&hash, hash_func, compare_func, delete_func);
        for (uint i = 0; i < (Num_Iterations/2); i++) {
            /* Insert the lower number */
            int_node_t* entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = i;
            hash_set(&hash, &(entry->link));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
            /* Insert the higher number */
            entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = Num_Iterations - i;
            hash_set(&hash, &(entry->link));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
        }
        for (uint i = 0; i < (Num_Iterations/2); i++)
        {
            /* Find the lower number */
            int_node_t search = { .val = i };
            hash_entry_t* entry = hash_get(&hash, &(search.link));
            int_node_t* ientry = container_of(entry, int_node_t, link);
            CHECK(entry != NULL);
            CHECK(search.val == ientry->val);
            /* Find the higher number */
            search.val = Num_Iterations-i;
            entry = hash_get(&hash, &(search.link));
            ientry = container_of(entry, int_node_t, link);
            CHECK(entry != NULL);
            CHECK(search.val == ientry->val);
        }
        hash_deinit(&hash);
    }

    TEST(Verify random hash inserts and lookups)
    {
        hash_t hash;
        hash_init(&hash, hash_func, compare_func, delete_func);
        for (uint i = 0; i < 1000000; i++)
        {
            int_node_t* entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = (uint)rand();
            hash_set(&hash, &(entry->link));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
        }
        hash_deinit(&hash);
    }

    TEST(Verify sequential inserts and deletions)
    {
        hash_t hash;
        hash_init(&hash, hash_func, compare_func, delete_func);
        for (uint i = 0; i < Num_Iterations; i++)
        {
            int_node_t* entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = i;
            hash_set(&hash, &(entry->link));
            CHECK(i+1 == hash_size(&hash));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
        }
        for (uint i = 0; i < Num_Iterations; i++)
        {
            int_node_t search = { .val = i };
            CHECK(hash_del(&hash, &(search.link)));
            CHECK((Num_Iterations - (i+1)) == hash_size(&hash));
        }
        hash_deinit(&hash);
    }

    TEST(Verify reverse inserts and deletions)
    {
        hash_t hash;
        hash_init(&hash, hash_func, compare_func, delete_func);
        for (uint i = 0; i < Num_Iterations; i++)
        {
            int_node_t* entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = Num_Iterations - i;
            hash_set(&hash, &(entry->link));
            CHECK(i+1 == hash_size(&hash));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
        }
        for (uint i = 0; i < Num_Iterations; i++)
        {
            int_node_t search = { .val = i+1 };
            CHECK(hash_del(&hash, &(search.link)));
            CHECK((Num_Iterations - (i+1)) == hash_size(&hash));
        }
        hash_deinit(&hash);
    }

    TEST(Verify ping pong hash inserts and deletions)
    {
        hash_t hash;
        hash_init(&hash, hash_func, compare_func, delete_func);
        for (uint i = 0; i < (Num_Iterations/2); i++) {
            /* Insert the lower number */
            int_node_t* entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = i;
            hash_set(&hash, &(entry->link));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
            /* Insert the higher number */
            entry = (int_node_t*)malloc(sizeof(int_node_t));
            entry->val = Num_Iterations - i;
            hash_set(&hash, &(entry->link));
            CHECK(&(entry->link) == hash_get(&hash, &(entry->link)));
        }
        for (uint i = 0; i < Num_Iterations; i++)
        {
            int_node_t search = { .val = i };
            if (hash_get(&hash, &(search.link)))
                CHECK(hash_del(&hash, &(search.link)));
        }
        hash_deinit(&hash);
    }
}

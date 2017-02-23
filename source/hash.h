/*
    Simple generic hash table implementation.
    
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

typedef struct hash_entry_t {
    unsigned int hash;
    struct hash_entry_t* next;
} hash_entry_t;

typedef unsigned int (*hash_hashfn_t)(const hash_entry_t* entry);

typedef int (*hash_cmpfn_t)(const hash_entry_t* entry1, const hash_entry_t* entry2);

typedef void (*hash_freefn_t)(hash_entry_t* key);

typedef struct {
    size_t size;
    size_t bkt_count;
    hash_entry_t** buckets;
    hash_hashfn_t hashfn;
    hash_cmpfn_t cmpfn;
    hash_freefn_t delfn;
} hash_t;

#define NUM_PRIMES (sizeof(Primes)/sizeof(unsigned int))

static unsigned int Primes[] = {
    5, 13, 23, 53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593,
    49157, 98317, 196613, 393241, 786433, 1572869, 3145739, 6291469,
    12582917, 25165843, 50331653, 100663319, 201326611, 402653189,
    805306457, 1610612741
};

static inline unsigned int num_buckets(unsigned int idx) {
    return Primes[idx];
}

static void rehash(hash_t* hash);
static void hash_set(hash_t* hash, hash_entry_t* entry);

static uint64_t hash64(uint64_t key) {
    key = (~key) + (key << 21); // key = (key << 21) - key - 1;
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8); // key * 265
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4); // key * 21
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}

static uint32_t hash32(uint32_t key) {
   key = (key+0x7ed55d16) + (key<<12);
   key = (key^0xc761c23c) ^ (key>>19);
   key = (key+0x165667b1) + (key<<5);
   key = (key+0xd3a2646c) ^ (key<<9);
   key = (key+0xfd7046c5) + (key<<3);
   key = (key^0xb55a4f09) ^ (key>>16);
   return key;
}

static uint32_t hash_bytes(uint8_t* key, size_t len) {
    uint32_t a=31415u, b=27183u, hash;
    for (hash=0; len > 0; key++, len--, a*=b)
        hash = (a * hash) + *key;
    return hash32(hash);
}

static void find_entry(hash_t* hash, hash_entry_t** parent, hash_entry_t** current, hash_entry_t* entry) {
    while(*current != NULL) {
        if (((*current)->hash == entry->hash) &&
            (0 == hash->cmpfn(*current, entry)))
            break;
        *parent  = *current;
        *current = (*current)->next;
    }
}

static void rehash(hash_t* hash) {
    if ((hash->bkt_count+1) < NUM_PRIMES) {
        unsigned int oldcount = hash->bkt_count++;
        hash_entry_t** oldbuckets = hash->buckets;
        hash->buckets = (hash_entry_t**)calloc(sizeof(hash_entry_t*), num_buckets(hash->bkt_count));
        hash->size = 0;
        /* Iterate over all of the old buckets */
        for (unsigned int i = 0; i < num_buckets(oldcount); i++) {
            hash_entry_t* node = oldbuckets[i];
            /* re-insert all entries in the bucket into the new bucket table */
            while (node != NULL) {
                hash_entry_t* entry = node;
                node = entry->next;
                hash_set(hash, entry);
            }
        }
        /* Free the old bucket table */
        free(oldbuckets);
    }
}

static void hash_init(hash_t* hash, hash_hashfn_t hashfn, hash_cmpfn_t cmpfn, hash_freefn_t delfn) {
    hash->size      = 0;
    hash->bkt_count = 0;
    hash->hashfn    = hashfn;
    hash->cmpfn     = cmpfn;
    hash->delfn     = delfn;
    hash->buckets   = (hash_entry_t**)calloc(sizeof(hash_entry_t*), num_buckets(hash->bkt_count));
}

static void hash_clr(hash_t* hash) {
    /* Delete all the entries in the hash */
    for (unsigned int i = 0; i < num_buckets(hash->bkt_count); i++) {
        hash_entry_t* node = hash->buckets[i];
        hash->buckets[i] = NULL;
        while (node != NULL) {
            hash_entry_t* deadite = node;
            node = node->next;
            hash->delfn(deadite);
        }
    }
}

static void hash_deinit(hash_t* hash) {
    hash_clr(hash);
    free(hash->buckets);
}

static size_t hash_size(hash_t* hash) {
    return hash->size;
}

static void hash_set(hash_t* hash, hash_entry_t* entry) {
    if (hash->size >= num_buckets(hash->bkt_count))
        rehash(hash);
    entry->hash = hash->hashfn(entry);
    unsigned int index    = (entry->hash % num_buckets(hash->bkt_count));
    hash_entry_t* parent  = NULL;
    hash_entry_t* node    = hash->buckets[index];
    hash_entry_t* deadite = NULL;
    find_entry(hash, &parent, &node, entry);
    if ((parent == NULL) && (node == NULL)) {
        hash->buckets[index] = entry;
        entry->next = NULL;
        hash->size++;
    } else if (node == NULL) {
        parent->next = entry;
        entry->next = NULL;
        hash->size++;
    } else if (parent == NULL) {
        deadite = node;
        entry->next = deadite->next;
        hash->buckets[index] = entry;
    } else {
        deadite = node;
        entry->next = deadite->next;
        parent->next = entry;
    }
    if (deadite != NULL)
        hash->delfn(deadite);
}

static hash_entry_t* hash_get(hash_t* hash, hash_entry_t* entry) {
    entry->hash = hash->hashfn(entry);
    unsigned int index   = (entry->hash % num_buckets(hash->bkt_count));
    hash_entry_t* parent = NULL;
    hash_entry_t* node   = hash->buckets[index];
    find_entry(hash, &parent, &node, entry);
    (void)parent;
    return node;
}

static bool hash_del(hash_t* hash, hash_entry_t* entry) {
    bool ret = false;
    entry->hash = hash->hashfn(entry);
    unsigned int index = (entry->hash % num_buckets(hash->bkt_count));
    hash_entry_t* parent = NULL;
    hash_entry_t* node = hash->buckets[index];
    find_entry(hash, &parent, &node, entry);
    if (node != NULL) {
        hash_entry_t* deadite = node;
        node = node->next;
        if (parent != NULL)
            parent->next = node;
        else
            hash->buckets[index] = node;
        hash->delfn(deadite);
        ret = true;
        hash->size--;
    }
    return ret;
}

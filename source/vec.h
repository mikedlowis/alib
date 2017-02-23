/**
  @brief Generic vector implementation.
  @author Michael D. Lowis
  @license BSD 2-clause License
*/
#ifndef VEC_H
#define VEC_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t   elem_count;
    size_t   elem_size;
    size_t   elem_capacity;
    uint8_t* elem_buffer;
} vec_t;

typedef int (*vec_cmpfn_t)(const void*,const void*);

#ifndef DEFAULT_VEC_CAPACITY
#define DEFAULT_VEC_CAPACITY (size_t)8
#endif

static void vec_init(vec_t* vec, size_t elem_size) {
    vec->elem_size     = elem_size;
    vec->elem_count    = 0;
    vec->elem_capacity = DEFAULT_VEC_CAPACITY;
    vec->elem_buffer   = malloc(elem_size * vec->elem_capacity);
}

static size_t vec_size(vec_t* vec) {
    return vec->elem_count;
}

static bool vec_empty(vec_t* vec) {
    return (vec->elem_count == 0);
}

static size_t vec_capacity(vec_t* vec) {
    return vec->elem_capacity;
}

static size_t vec_next_capacity(size_t req_size) {
    size_t next_power = req_size;
    size_t num_bits = sizeof(size_t) * 8;
    size_t bit_n;

    /* Find the next highest power of 2 */
    next_power--;
    for (bit_n = 1; bit_n < num_bits; bit_n = bit_n << 1)
        next_power = next_power | (next_power >> bit_n);
    next_power++;

    return next_power;
}

static void vec_reserve(vec_t* vec, size_t size) {
    vec->elem_buffer   = realloc(vec->elem_buffer, size * vec->elem_size);
    vec->elem_capacity = size;
}

static void vec_resize(vec_t* vec, size_t count, void* fillval) {
    if (count > vec->elem_count) {
        vec_reserve(vec, vec_next_capacity(count+1));
        for (; vec->elem_count < count; vec->elem_count++)
            memcpy(&(vec->elem_buffer[vec->elem_count * vec->elem_size]), fillval, vec->elem_size);
    } else if (count < vec->elem_count) {
        vec->elem_count = count;
    }
}

static void vec_shrink_to_fit(vec_t* vec) {
    vec->elem_buffer   = realloc(vec->elem_buffer, vec->elem_count * vec->elem_size);
    vec->elem_capacity = vec->elem_count;
}

static void* vec_at(vec_t* vec, size_t index) {
    return &(vec->elem_buffer[index * vec->elem_size]);
}

static void vec_set(vec_t* vec, size_t index, void* data) {
    memcpy(&(vec->elem_buffer[index * vec->elem_size]), data, vec->elem_size);
}

static bool vec_insert(vec_t* vec, size_t index, size_t num_elements, ...) {
    (void)vec;
    (void)index;
    (void)num_elements;
    return false;
}

static bool vec_erase(vec_t* vec, size_t start_idx, size_t end_idx) {
    (void)vec;
    (void)start_idx;
    (void)end_idx;
    return false;
}

static void vec_push_back(vec_t* vec, void* data) {
    vec_resize(vec, vec->elem_count+1, data);
}

static void vec_pop_back(vec_t* vec, void* outdata) {
    vec->elem_count--;
    memcpy(outdata, &(vec->elem_buffer[vec->elem_count * vec->elem_size]), vec->elem_size);
}

static void vec_clear(vec_t* vec) {
    vec->elem_count = 0;
}

static void vec_sort(vec_t* vec, int (*cmpfn)(const void*,const void*)) {
    qsort(vec->elem_buffer, vec->elem_count, vec->elem_size, cmpfn);
}

#endif /* VEC_H */

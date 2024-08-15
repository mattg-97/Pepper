#include "arraylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArrayList *array_list_init()
{
    ArrayList *da = malloc(sizeof(ArrayList));
    da->items = calloc(DEFAULT_CAPACITY, sizeof(void *));
    da->capacity = DEFAULT_CAPACITY;

    return da;
}

void *array_list_add(ArrayList *da, const void *value)
{
    if (da->size >= da->capacity)
    {
        void **newItems =
            realloc(da->items, (da->capacity <<= 1) * sizeof(void **));

        da->items = newItems;
    }

    void *copy_value = retrive_copy_of_value(value);
    da->items[da->size++] = copy_value;

    return copy_value;
}

void *array_list_put(ArrayList *da, const void *value, const unsigned index)
{
    if (!array_list_contains(da->size, index))
        return INDEX_OUT_OF_BOUNDS;

    free(da->items[index]);
    void *copy_value = retrive_copy_of_value(value);
    da->items[index] = copy_value;

    return copy_value;
}

void *array_list_get(ArrayList *da, const unsigned index)
{
    if (!array_list_contains(da->size, index))
        return INDEX_OUT_OF_BOUNDS;

    return da->items[index];
}

void array_list_delete(ArrayList *da, const unsigned index)
{
    if (!array_list_contains(da->size, index))
        return;

    for (unsigned i = index; i < da->size; i++)
    {
        da->items[i] = da->items[i + 1];
    }

    da->size--;

    free(da->items[da->size]);
}

unsigned array_list_contains(const unsigned size, const unsigned index)
{
    if (size >= 0 && index < size)
        return 1;

    printf("index [%d] out of bounds!\n", index);
    return 0;
}

void *retrive_copy_of_value(const void *value)
{
    void *value_copy = malloc(sizeof(void *));
    memcpy(value_copy, value, sizeof(void *));

    return value_copy;
}
#ifndef pepper_array_list_h
#define pepper_array_list_h
#define DEFAULT_CAPACITY 1 << 4
#define INDEX_OUT_OF_BOUNDS NULL

typedef struct dynamic_array
{
    void **items;
    unsigned size;
    unsigned capacity;
} ArrayList;

extern ArrayList *array_list_init();

extern void *array_list_add(ArrayList *da, const void *value);

extern void *array_list_put(ArrayList *da, const void *value, unsigned index);

extern void *array_list_get(ArrayList *da, const unsigned index);

extern void array_list_delete(ArrayList *da, const unsigned index);

unsigned array_list_contains(const unsigned size, const unsigned index);

extern void *retrive_copy_of_value(const void *value);

#endif
#ifndef pepper_hashtable_h
#define pepper_hashtable_h

#define MAXELEMENTS 256

typedef struct {
    /*
        void* array for generic use of the dictionary.
        there actual saves the entries.
    */
    void *elements[MAXELEMENTS];

    /* contains the number of elements in this dictionary */
    int number_of_elements;

} HashTable;

/*
    hash_table_init: is a simple constructor for creating
                a dictionary and setting up the
                member field 'number_of_elements'
                and prepares the inner array 'elements'
*/
HashTable *hash_table_init(void);

/*
    hash_table_add: adds item (void*) to the dictionary at given label
    returns 0 if adding was sucessful otherwise -1
*/
int hash_table_add(HashTable *, char label[], void *);

/*
    hash_table_add_index: adds item (void*) to the dictionary at given index (int)
    returns 0 if adding was sucessful otherwise -1
*/
int hash_table_add_index(HashTable *, int index, void *);

/*
    get_element: returns the element at given label
*/
void *hash_table_get(HashTable *, char[]);

/*
    get_element: returns the element at given index
*/
void *hash_table_get_index(HashTable *, int);

/*
    simple destrcutor function
*/
void hash_table_destroy(HashTable *);
int get_hash(char s[]);

#endif

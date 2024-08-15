#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>

HashTable *hash_table_init(void)
{
    HashTable *p_dic = malloc(sizeof(HashTable));
    if (p_dic)
    {
        p_dic->number_of_elements = 0;

        for (int i = 0; i < MAXELEMENTS; i++)
        {
            p_dic->elements[i] = NULL;
        }

        return p_dic;
    }
    else
    {
        printf("unable to create a dictionary\n");
        return NULL;
    }
}

/*
    utility function
    sdbm hash algorithm
    returns a hashcode for the given string 's'
*/
int get_hash(char s[])
{
    unsigned int hash_code = 0;

    /* iterates over string at each character */
    for (int counter = 0; s[counter] != '\0'; counter++)
    {
        /* actual computing of the hash code */
        hash_code =
            (unsigned int)s[counter] + (hash_code << 6) + (hash_code << 16) - hash_code;
    }

    /* % modulo is for fitting the index in array. */
    return hash_code % MAXELEMENTS;
}

int hash_table_add(HashTable *dic, char label[], void *item)
{
    unsigned int index = (unsigned int)get_hash(label);

    /* make sure index is fitting */
    if (index < MAXELEMENTS)
    {
        dic->elements[index] = item;
        return 0;
    }

    /* error case */
    return -1;
}

int hash_table_add_index(HashTable *dic, int index, void *item)
{
    /* make sure whether this place is already given */
    if (!dic->elements[index])
    {
        dic->elements[index] = item;
        return 0;
    }

    /* error case */
    return -1;
}

void *hash_table_get(HashTable *dict, char s[])
{
    int index = get_hash(s);
    if (dict->elements[index])
    {
        return dict->elements[index];
    }

    printf("None entry at given label\n");
    return NULL;
}

void *hash_table_get_index(HashTable *dict, int index)
{
    if (index >= 0 && index < MAXELEMENTS)
    {
        return dict->elements[index];
    }

    printf("index out of bounds!\n");
    return NULL;
}

void hash_table_destroy(HashTable *dict) { free(dict); }

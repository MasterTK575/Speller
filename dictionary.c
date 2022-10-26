// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <strings.h>

#include "dictionary.h"


// Number of buckets in hash table
const unsigned int N = 26*26*26;

// Hash table
node *table[N];

void set_table(void)
{
    // set pointers to NULL
    for (int i = 0; i < N; i++)
    {
        table[i] = NULL;
    }
}


// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // run word through hash function
    unsigned int hashcode = hash(word);
    // look through the hashed list to find word
    return check_list(table[hashcode], word);
}

bool check_list(node* next, const char* word)
{
    // if we get to the end without finding it, return false
    if (next == NULL)
    {
        return false;
    }
    // if we find it return true
    if (strcasecmp(next->word, word) == 0)
    {
        return true;
    }
    // recursively look through the entire linked list
    return check_list(next->next, word);
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // Hashfunction using the first three letters
    unsigned int hashcode;
    // the first letter needs to be 26*26 spaces apart
    // to account for the other two following letters in the index
    hashcode = (toupper(word[0]) - 'A') * (26*26);
    
    // if we are at the end of the word, return
    if (word[1] == '\0')
    {
        return hashcode;
    }
    hashcode = hashcode + (toupper(word[1]) - 'A') * 26;

    if (word[2] == '\0')
    {
        return hashcode;
    }
    hashcode = hashcode + (toupper(word[2]) - 'A');
    return hashcode;
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        return false;
    }
    char c;
    char word[LENGTH + 1];
    int index = 0;
    // repeat until end of file, one char at a time
    while (fread(&c, sizeof(char), 1, file))
    {
        // meaning end of a word
        // make sure to use single quotes for characters (and double quotes for strings)
        if (c == '\n' && index > 0)
        {
            word[index] = '\0';

            // make node out of word
            node* new_node = make_new(word);

            // commit word to hash table
            commit(new_node);

            // start new
            index = 0;
            continue;
        }
        // keep adding up characters until end of word
        word[index] = c;
        index++;
    }
    fclose(file);
    return true;
}

node* make_new(char word[])
{
    // allocate memory for a new node
    node* new_node = malloc(sizeof(node));
    new_node->next = NULL;
    int n = 0;
    // copy the word into new_node
    while (word[n] != '\0')
    {
        new_node->word[n] = word[n];
        n++;
    }
    new_node->word[n] = '\0';
    return new_node;
}

void commit(node* new_node)
{
    // run word through hash function
    unsigned int hashcode = hash(new_node->word);

    // commit to hash table
    if (table[hashcode] != NULL)
    {
        // if there is a linked-list already, save to the next field of new_node
        new_node->next = table[hashcode];
    }
    // put new_node at the beginning
    table[hashcode] = new_node;
    return;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    unsigned int numb_words = 0;
    // loop through each linked list in the hash table
    for (int i = 0; i < N; i++)
    {
        // follow linked list and add up words
        list_counting(table[i], &numb_words);
    }
    // printf("Words in dict: %i\n", numb_words);
    return numb_words;
}

void list_counting(node* node, unsigned int* numb_words)
{
    if (node == NULL)
    {
        return;
    }
    *numb_words = *numb_words + 1;
    list_counting(node->next, numb_words);
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // unload each index in the table
    for (int i = 0; i < N; i++)
    {
        unload_list(table[i]);
    }
    return true;
}

void unload_list(node* next)
{
    if (next == NULL)
    {
        return;
    }
    // free rest of the list first, then the actual element
    unload_list(next->next);
    free(next);
}
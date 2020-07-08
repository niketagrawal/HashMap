#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "hashmap.h"


/*
* Creating the hashmap
*
* This function allocates enough memory to fit key_space buckets
* key_space - the number of buckets in the hash map. 
*/
HashMap* create_hashmap(size_t key_space) {
    HashMap* hm = (HashMap*) calloc (key_space, sizeof(HashMap));
    hm -> map = (Node**) calloc (key_space, sizeof(Node*));
    hm -> key_space = key_space;
    return hm;
}

/*
* Calculating hash
*
* A hash function maps a string (i.e. an array of chars ending with a null
* character) to an index, so it returns a unsigned int. The parameter of a hash
* function is simply a
    • key - a null-terminated string of characters.
*
* This function sums all ASCII values of the characters of the key
* Modulo key_space is applied to the result such that the value will be in the available bucket range.
*/
unsigned int hash(const char* key) {
    unsigned int index = 0;
    for (unsigned int i=0; key[i] != '\0'; i++) {
        index += key[i];
    }
    return index;
}


/*
* Inserting Data

* This function has parameters
*   • hm, a pointer to a hash map;
*   • key, a null-terminated string of characters;
*   • data, a void pointer to the source data;
*   • resolve_collision, a ResolveCollisionCallback (see below).
* 
* The function stores the data pointer and a copy of the key in the bucket that can be found by applying the 
* hash function on the key. 
* In case of a collision, i.e. when there already is data with the same key in the hash map, the
* resolve_collision function is called with the the previously stored data and data as arguments and the 
* returned void pointer is stored in the bucket instead.
* 
* ResolveCollisionCallback, a pointer to a function that returns a void pointer and has two parameters:
    • old_data - a void pointer to the previously stored data;
    • new_data - a void pointer to the data that is being newly inserted.
* The function determines what data is stored in the hash map in case of a key collision by returning the void 
* pointer to the data that is to be stored. 
*/
void insert_data(HashMap* hm, const char* key, void* data, void* (*resolve_collision)(void* old_data, void* new_data)) {
    if (hm == NULL) {
        printf("HashMap doesn't exist\n");
        return;
    }
    HashMap* temphash = hm;
    unsigned int bucket = hash(key) % (temphash->key_space);  //to get index in the range of array index of HashMap
    void* new_data;

    if (temphash->map[bucket] == NULL) {
        Node* new_node = (Node*) calloc (1, sizeof(Node));
        new_node->key = key;
        new_node->data = data;
        temphash->map[bucket] = new_node;
        new_node->next = NULL;
    }
    else {
        Node* temp = temphash->map[bucket];
        while(temp != NULL) {  //iterate through the linked list to check if there already exists a data with the same key
            if(temp->key == key) {
                new_data = resolve_collision(temp->data, data); 
                temp->data = new_data;
                return;
            }
            temp = temp->next;
        }
        Node* new_node = (Node*) calloc (1, sizeof(Node)); /*no duplicate data/key found, create and insert a 
                                                                new node at the beginning */
        new_node->key = key;
        new_node->data = data;
        new_node->next = temphash->map[bucket];
        temphash->map[bucket] = new_node;
    }
}

/*
* Retrieving Data
*
* This function has parameters
*   • hm - a pointer to a hash map;
*   • key - a null-terminated string of characters.
* The function returns the data pointer (a void pointer) in the hash map that is associated with the key. 
* If the key is not present in the hash map, NULL is returned.
*/
void* get_data (HashMap* hm, const char* key) {
    if (hm == NULL) {
        printf("HashMap doesn't exist\n");
        return 0;
    } 
    HashMap* temphash = hm;
    unsigned int bucket = 0;
    bucket = hash(key) % (temphash->key_space);
    Node* temp = temphash->map[bucket];
    while (temp != NULL) {
        if (temp->key == key) {
            return temp->data;
        }
        temp = temp->next;
    }
    return NULL;
}


/*
* Iterator
*
* Implement a function iterate that has parameters
*   • hm - a pointer to a hash map;
*   • callback - a pointer to a function that returns void and has two parameters:
*       • key - a null-terminated string of characters;
*       • data - a void pointer to the data.
* This function iterates over the entire hash map. For each data element it finds, the callback function is
* called with the two members of the element
*/
void iterate(HashMap* hm, void(*callback)(const char* key, void* data)) {
    if (hm == NULL) {
        printf("HashMap doesn't exist\n");
        return;
    }
    HashMap* temphash = hm;
    for (unsigned int i=0; i < temphash->key_space; i++) {
        Node* temp = temphash->map[i];
        if (temp == NULL) {
            continue;
        }
        while (temp != NULL) {
            callback (temp->key, temp->data);
            temp = temp->next;
        }
    }
}

/*
* Removing Data
*
* This function has parameters
*   • hm - a pointer to a hash map;
*   • key - a null-terminated string of characters.
*   • destroy_data, a DestroyDataCallback (see below).
* This function removes the element in the hash map that is associated with the given key and frees the allocated memory.
* If the destroy_data parameter is non-NULL it is called with the data pointer of the element as argument. 
* If the key is not present, the hash map remains untouched. As the remove_data function cannot fail, its return type is void.
* DestroyDataCallback, a pointer to to a function that has a void return type and has one parameter:
*   • data, a void pointer.
*/
void remove_data (HashMap* hm, const char* key, void(*destroy_data)(void* data)) {
    if (hm == NULL) {
        printf("HashMap doesn't exist\n");
        return;
    }
    HashMap* temphash = hm;
    unsigned int bucket = (hash(key) % (temphash->key_space));
    Node* temp = temphash->map[bucket];
    Node* prev;
    if (temp == NULL) {   // check if the linked list doesn't exist
        return;
    }
    if (temp->key == key) {   //if duplicate key/data pair is found at the first node in the bucket
        if (destroy_data != NULL) {
            destroy_data (temp->data);
        }
        temphash->map[bucket] = temp->next;
        free(temp);
        temp = NULL;
        return;
    }
    while ((temp != NULL) && (temp->key != key)) { // iterate until matching key/data pair is found while keeping track of the
                                                       // previous node simultaneously for link adjustments later 
        prev = temp;
        temp = temp->next;
    }
    if(temp == NULL) {
        return;
    }
    if (temp->key == key) {
        if (destroy_data != NULL) {
            destroy_data(temp->data);
        }
        prev->next = temp->next;
        free(temp);
        temp = NULL;
        return;
    }
}

/*
* Hashmap Deletion
*
* This function has parameters
*   • hm, a pointer to the hash map that is to be deleted;
*   • destroy_data, a DestroyDataCallback.
* The function deallocates all memory that was allocated by the hash map.
* If the destroy_data parameter is non-NULL it is be called for every data element that is stored in the hash map 
* with the data pointer of the element as argument.
*/
void delete_hashmap (HashMap* hm, void(*destroy_data)(void* data)) {
    if (hm == NULL) {
        printf("HashMap doesn't exist\n");
        return;
    }
    HashMap* temphash = hm;
    for (unsigned int i=0; i < temphash->key_space; i++) {
        Node* temp = temphash->map[i];
        Node* next;
        if (temp == NULL) {
            continue;
        }
        while (temp != NULL) {
            next = temp->next;        //store the next link of temp in a temporary ptr before freeing temp
            if (destroy_data != NULL) {
                destroy_data (temp->data);
            }
            free(temp);
            temp = next;
        }
        temphash->map[i] = NULL;
    }
    free(temphash->map);
    temphash->map= NULL;
    free(hm);
    hm = NULL;
}


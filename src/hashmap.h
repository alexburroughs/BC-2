#ifndef HASHMAP_H
#define HASHMAP_H

#define MAX_KEY_SIZE 40

typedef void (*free_ptr_t)(void*);

struct Hashmap_node
{
    struct Hashmap_node* next;
    char* key;
    void* val;
};

typedef struct Hashmap_node Hashmap_Node;

struct hashmap 
{
    Hashmap_Node* first;
    int size;
    free_ptr_t free_ptr;
};

typedef struct hashmap Hashmap;

Hashmap* Hashmap_new(free_ptr_t free_ptr);
int Hashmap_insert(Hashmap* map, char* key, void* val);
int Hashmap_insert_or_set(Hashmap* map, char* key, void* val);
void* Hashmap_get(Hashmap* map, char* key);
void Hashmap_free(Hashmap* map);
Hashmap_Node* Hashmap_iter_next(Hashmap_Node* node);
Hashmap_Node* Hashmap_get_iter(Hashmap* map);

#endif
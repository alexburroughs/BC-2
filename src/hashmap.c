#include "hashmap.h"
#include "stdlib.h"
#include "string.h"

Hashmap* Hashmap_new(free_ptr_t free_ptr)
{
    Hashmap* map = (Hashmap*)malloc(sizeof(Hashmap));
    map->size = 0;
    map->free_ptr = free_ptr;

    return map;
}

Hashmap_Node* get_node(Hashmap* map, char* key)
{
    Hashmap_Node* current = map->first;

    for (int i = 0; i < map->size; ++i) {

        if (!strcmp(current->key, key)) 
            return current;
        if (i == map->size)
            break;
        current = current->next;
    }

    return NULL;
}

int Hashmap_insert(Hashmap* map, char* key, void* val)
{
    Hashmap_Node* node = (Hashmap_Node*)malloc(sizeof(Hashmap_Node));
    
    node->key = (char*)malloc(sizeof(char) * MAX_KEY_SIZE);
    strcpy(node->key, key);

    node->val = val;

    if (map->size == 0) {
        map->first = node;
    }
    else {
        
        Hashmap_Node* current = map->first;

        for (int i = 1; i < map->size; ++i) {
            current = current->next;
        }

        current->next = node;
    }

    ++map->size;

    return 0;
}

int Hashmap_insert_or_set(Hashmap* map, char* key, void* val)
{
    Hashmap_Node* current_node = get_node(map, key);

    if (current_node == NULL) {
        Hashmap_insert(map, key, val);
    }
    else {
        map->free_ptr(current_node->val);
        current_node->val = val;
    }

    return 0;
}

void* Hashmap_get(Hashmap* map, char* key)
{

    Hashmap_Node* current = get_node(map, key);

    if (current != NULL) {
        return current->val;
    }    

    return NULL;
}

Hashmap_Node* Hashmap_iter_next(Hashmap_Node* node)
{
    return node->next;
}

Hashmap_Node* Hashmap_get_iter(Hashmap* map) 
{
    return map->first;
}

void Hashmap_free(Hashmap* map) 
{
    
    Hashmap_Node* current = map->first;
    Hashmap_Node* last = current;

    for (int i = 0; i < map->size; ++i) {

        current = current->next;
        free(last->key);
        map->free_ptr(last->val);
        free(last);
        last = current;
    }

    free(map);
}
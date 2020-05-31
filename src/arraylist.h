#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdint.h>

#define ARRAYLIST_DEFAULT_SIZE 100
#define ARRAYLIST_DEFAULT_SCALE 1.5

typedef void (*free_ptr_t)(void*);

struct arraylist 
{
    int32_t size;
    int32_t max_size;
    free_ptr_t free_ptr;
    void** arr;
};

typedef struct arraylist Arraylist;

Arraylist* Arraylist_new(free_ptr_t ptr);
Arraylist* Arraylist_new_with_size(free_ptr_t ptr, 
    int size);
int Arraylist_add(Arraylist* array, void* value);
int Arraylist_set(Arraylist* array, 
    void* value, int position);
void* Arraylist_get(Arraylist* array, int position);
int Arraylist_remove(Arraylist* array, int position);
int Arraylist_size(Arraylist* array); 
void Arraylist_free(Arraylist* array);

#endif
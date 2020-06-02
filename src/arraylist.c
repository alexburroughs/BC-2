#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "arraylist.h"

#define ARRAYLIST_POSITION_IN_BOUNDS(pos, size)  !(pos >= size || pos < 0)

Arraylist* Arraylist_new(free_ptr_t ptr)
{
    return Arraylist_new_with_size(ptr, ARRAYLIST_DEFAULT_SIZE);
}


Arraylist* Arraylist_new_with_size(free_ptr_t ptr, int size) 
{
    Arraylist* array = (Arraylist*)malloc(sizeof(Arraylist));
    array->arr = (void**)malloc(sizeof(void*) * size);
    array->size = 0;
    array->max_size = size;
    array->free_ptr = ptr;

    return array;
}

int Arraylist_expand_by_scale(Arraylist* array, int scale)
{
    array->max_size = (scale * array->max_size);
    array->arr = (void**)realloc(array->arr, sizeof(void*) * array->max_size);

    if (array->arr == NULL)
        return -1;
    return 0;
}

int Arraylist_expand(Arraylist* array) 
{
    return Arraylist_expand_by_scale(array, ARRAYLIST_DEFAULT_SCALE);
}

int Arraylist_add(Arraylist* array, void* value)
{
    return Arraylist_set(array, value, array->size);
}

int Arraylist_set(Arraylist* array, void* value, int position)
{
    if (position > array->size)
        return -1;

    if (position == array->size)
        ++array->size;

    if (array->size > array->max_size)
        if (Arraylist_expand(array))
            return -1;

    array->arr[position] = value;

    return 0;
}

void* Arraylist_get(Arraylist* array, int position)
{
    if (!ARRAYLIST_POSITION_IN_BOUNDS(position, array->size)) {
        return NULL;
    }

    return array->arr[position];
}

int Arraylist_remove(Arraylist* array, int position)
{
    
    if (!ARRAYLIST_POSITION_IN_BOUNDS(position, array->size))
        return -1;

    array->free_ptr(array->arr[position]);
    
    for (int i = position+1; i < array->size; i++)
        array->arr[i-1] = array->arr[i];
    
    --array->size;

    return 0;
}

int Arraylist_size(Arraylist* array)
{
    return array->size;
}

void Arraylist_free(Arraylist* array)
{
    for (int i = 0; i < array->size; i++) 
        if (array->arr[i])
            array->free_ptr(array->arr[i]);

    free(array->arr);
    free(array);
}
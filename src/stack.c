#include "stack.h"
#include "arraylist.h"

#include <stddef.h>

Stack* Stack_new(free_ptr_t ptr)
{
    return Arraylist_new(ptr);
}

int Stack_push(Stack* stack, void* val)
{
    return Arraylist_add(stack, val);
}

void* Stack_pop(Stack* stack)
{
    if (stack->size == 0)
        return NULL;

    void* ret  = Arraylist_get(stack, stack->size-1);
    --stack->size;
    return ret;
}

void Stack_pop_and_free(Stack* stack)
{
    Arraylist_remove(stack, stack->size-1);
}

void Stack_free(Stack* stack)
{
    Arraylist_free(stack);
}
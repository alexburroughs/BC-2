#ifndef STACK_H
#define STACK_H

#include "arraylist.h"

typedef struct arraylist Stack;

Stack* Stack_new(free_ptr_t ptr);
int Stack_push(Stack* stack, void* val);
void* Stack_pop(Stack* stack);
void Stack_pop_and_free(Stack* stack);
void Stack_free(Stack* stack);
void* Stack_peek(Stack* stack);

#endif
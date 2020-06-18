#ifndef TESTS_H
#define TESTS_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arraylist.h"
#include "hashmap.h"
#include "tokenizer.h"
#include "token.h"
#include "ast.h"

#define assert_begin() bool c = false
#define assert(check, msg) if(!check) {printf("Assertion Error: %s\n", msg);}
#define assert_pass(check, msg, pass) c = check; (*pass = c && *pass); assert(c, msg)
#define assert_fail(check, msg, pass) assert_pass(!check, msg, pass)

bool Arraylist_test() 
{
    assert_begin();

    bool pass = true;

    //new
    
    Arraylist* arr = Arraylist_new(*free);

    //add

    int* test_int = (int*)malloc(sizeof(int));
    *test_int = 5;

    int* test_int2 = (int*)malloc(sizeof(int));
    *test_int2 = 6;

    int* test_int3 = (int*)malloc(sizeof(int));
    *test_int3 = 3;

    assert_pass(Arraylist_add(arr, test_int) == 0, "add, returned -1", &pass);
    assert_pass(Arraylist_add(arr, test_int2) == 0, "add, returned -1", &pass);

    assert_pass(*((int*)arr->arr[0]) == 5, "add, array at 0 != 5", &pass);
    assert_pass(*((int*)arr->arr[1]) == 6, "add, array at 0 != 6", &pass);

    //set

    assert_pass(Arraylist_set(arr, test_int3, 0) == 0, "set, returned -1", &pass);

    assert_pass(*((int*)arr->arr[0]) == 3, "set, array at 0 != 3", &pass);
    
    //get

    assert_pass(*((int*)Arraylist_get(arr, 0)) == *((int*)arr->arr[0]), "get, array at 0 != get at 0", &pass);
    assert_pass(Arraylist_get(arr, 2) == NULL, "get, array returned 0 on index out of bounds", &pass);    
    assert_pass(Arraylist_get(arr, -1) == NULL, "get, array returned 0 on index out of bounds", &pass);    

    //remove
    assert_pass(Arraylist_remove(arr, 0) == 0, "remove, return -1", &pass);
    assert_pass(6 == *((int*)arr->arr[0]), "remove, array at 0 != 6", &pass);

    assert_fail(Arraylist_remove(arr, -1) == 0, "remove, returned 0 on -1", &pass);
    assert_fail(Arraylist_remove(arr,  2) == 0, "remove, returned 0 on index out of bouds", &pass);
    
    //free
    Arraylist_free(arr);

    free(test_int);
    
    return pass;
    
}

bool Hashmap_tests()
{
    assert_begin();

    bool pass = true;

    int* var1 = (int*)malloc(sizeof(int));
    *var1 = 4;

    int* var2 = (int*)malloc(sizeof(int));
    *var2 = 2;

    int* var3 = (int*)malloc(sizeof(int));
    *var3 = 45;

    Hashmap * map = Hashmap_new(free);

    Hashmap_insert(map, "hello", var1);
    Hashmap_insert(map, "hi", var2);

    assert_pass(var1 == Hashmap_get(map, "hello"), "var1 != get", &pass);
    assert_pass(*(var1) == *((int*)Hashmap_get(map, "hello")), "var1 != get", &pass);
    assert_pass(*(var2) == *((int*)Hashmap_get(map, "hi")), "var2 != get", &pass);
    assert_pass(NULL == Hashmap_get(map, "aaaaaa"), "Null != get", &pass);

    free(var3);

    Hashmap_free(map);
    return pass;
}

bool Tokenizer_tests()
{
    bool pass = true;
    assert_begin();

    Arraylist *l = tokenize("function { } = if else 1321321321 \nhello = == != ; . -> if == \"string\" ");
    
    assert_pass(strcmp(((Token*)Arraylist_get(l, 7))->name,"hello") == 0, "Name not tokenized properly", &pass);

    assert_pass((((Token*)Arraylist_get(l, 0))->type) == Function, "Function, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 1))->type) == OpenBrace, "OpenBrace, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 2))->type) == CloseBrace, "CloseBrace, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 3))->type) == Assign, "Assign, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 4))->type) == If, "If, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 5))->type) == Else, "Else, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 6))->type) == Number, "Number, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 7))->type) == Identifier, "Identifier, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 8))->type) == Assign, "Assign, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 9))->type) == Equal, "Equal, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 10))->type) == NotEqual, "NotEqual, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 11))->type) == Semicolon, "Semicolon, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 12))->type) == Dot, "Dot, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 13))->type) == Arrow, "Arrow, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 14))->type) == If, "If, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 15))->type) == Equal, "Equal, Not tokenized properly", &pass);
    assert_pass((((Token*)Arraylist_get(l, 16))->type) == String, "String, Not tokenized properly", &pass);

    Arraylist_free(l);

    return pass;
}

bool run_tests()
{
    return Arraylist_test() && Hashmap_tests() && Tokenizer_tests();
}

#endif
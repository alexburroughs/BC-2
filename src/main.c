#include <stdio.h>
#include "arraylist.h"
#include <stdlib.h>
#include <stdbool.h>

#include "tests.h"

#include "tokenizer.h"
#include "ast.h"
#include "nonamegenerator.h"

#include "stringbuilder.h"

int main(int argc, char** argv) 
{
    #ifdef TESTS
    if (run_tests())
        printf("pass\n");
    else
        printf("fail\n");
    #else
    if (argv[1] == NULL)
        exit(1);
    char* filename = argv[1];

    char* code = read_file(filename);
    //printf("code: %s \n", code);

    Arraylist* tokens = tokenize(code);
    AST* ast = AST_from(tokens);
    char* gen = ast_to_nni(ast);
    //Arraylist_free(tokens);
    //AST_free(ast);
    printf("%s\n", gen);
    #endif
}
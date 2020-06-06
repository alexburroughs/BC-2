#ifndef STRINGBUILDER_H
#define STRINGBUILDER_H

#define INITIAL_STRING_SIZE 40
#define EXPAND_SCALAR 2

struct stringbuilder 
{
    int current_size;
    int size;
    char* str;
};

typedef struct stringbuilder StringBuilder;

StringBuilder* StringBuilder_new();
int StringBuilder_add(StringBuilder* sb, char val);
int StringBuilder_add_arr(StringBuilder* sb, char* arr);
char* StringBuilder_get(StringBuilder* sb);
void StringBuilder_clear(StringBuilder* sb);
void StringBuilder_free(StringBuilder* sb);

char* String_from(char* str);

#endif
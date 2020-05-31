#include "stringbuilder.h"

#include <stdlib.h>
#include <string.h>

void expand(StringBuilder* sb)
{
    sb->current_size *= EXPAND_SCALAR;
    sb->str = realloc(sb->str, sb->current_size);
}

StringBuilder* StringBuilder_new()
{
    StringBuilder* sb = (StringBuilder*)malloc(sizeof(StringBuilder));

    sb->current_size = INITIAL_STRING_SIZE;
    sb->size = 0;
    sb->str = (char*)malloc(sizeof(char) * INITIAL_STRING_SIZE);

    return sb;
}

int StringBuilder_add(StringBuilder* sb, char val)
{
    if (++sb->size >= sb->current_size)
        expand(sb);
    sb->str[sb->size-1] = val;

    return 0;
}

int StringBuilder_add_arr(StringBuilder* sb, char* arr)
{
    int size = strlen(arr);
    
    sb->size += size;

    if (sb->size >= sb->current_size)
        expand(sb);

    for (int i = 0; i < size; ++i) {
        sb->str[i+sb->size-1] = arr[i];
    }

    return 0;
}

char* StringBuilder_get(StringBuilder* sb)
{
    char* ret = malloc(sizeof(char) * sb->size +1);

    strcpy(ret, sb->str);
    ret[sb->size] = '\0';
    
    return ret;
}

void StringBuilder_free(StringBuilder* sb)
{
    free(sb->str);
    free(sb);
}

char* String_from(char* str)
{
    char* c = malloc(sizeof(str));
    strcpy(c, str);

    return c;
}
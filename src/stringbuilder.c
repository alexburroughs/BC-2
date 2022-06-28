#include "stringbuilder.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

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

    while (sb->size >= sb->current_size)
        expand(sb);

    for (int i = 0; i < size; ++i) {
        sb->str[i+sb->size-size] = arr[i];
    }

    return 0;
}

char* StringBuilder_get(StringBuilder* sb)
{
    char* ret = malloc(sizeof(char) * sb->size +1);

    memcpy(ret, sb->str, sizeof(char) * sb->size);
    ret[sb->size] = '\0';
    
    return ret;
}

void StringBuilder_clear(StringBuilder* sb)
{
    sb->size = 0;
}

void StringBuilder_free(StringBuilder* sb)
{
    free(sb->str);
    free(sb);
}

char* String_from(char* str)
{
    char* c = malloc(strlen(str)+1 * (sizeof(char)));
    memcpy(c, str, strlen(str)+1 * (sizeof(char)));

    return c;
}

char* String_from_int(int num) 
{
    int size =  11 * sizeof(char);
    char* c = malloc(size);
    sprintf(c, "%i\0", num);
    return c;
}

char* read_file(char* filename)
{
    char buf[255];
    FILE* ptr;
    char ch;
    ptr = fopen(filename, "r");
 
    if (NULL == ptr) {
        printf("file can't be opened \n");
    }
    StringBuilder* sb = StringBuilder_new();
    while (fgets(buf, 255, ptr) != NULL) { 
        StringBuilder_add_arr(sb, buf);
    }
    fclose(ptr);

    char* ret = StringBuilder_get(sb);
    StringBuilder_free(sb);
    return ret;
}
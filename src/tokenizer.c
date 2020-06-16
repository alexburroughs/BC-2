#include "tokenizer.h"
#include "arraylist.h"
#include "token.h"
#include "panic.h"
#include "stringbuilder.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define UNEXPECTED_TOKEN(position) \
panic("Unexpected token %c at line: %i:%i:%i", tokens[position], line, col, pos);

#define IS_NUMERIC(x) (x >= '0' && x <= '9')

#define IS_ALPHA(x) (x >= 'A' && x <= 'Z') ||\
                    (x >= 'a' && x <= 'z')

#define IS_ALPHANUMERIC(x) IS_ALPHA(x) || IS_NUMERIC(x)

#define IS_WHITESPACE(x) (x == ' ' || x == '\n' || x == '\r' || x == '\t')

#define ADVANCE(x) \
    pos+=x;\
    col+=x;

#define COMPARE(matcher, type, name) \
if (compare(tokens, matcher, pos)) {\
    Arraylist_add(token_list, Token_new(type, name, line, col));\
    ADVANCE(sizeof(matcher)-1);\
}

#define COMPARE_SINGLE(matcher, type, name, position) \
if (tokens[position] == matcher) {\
    Arraylist_add(token_list, Token_new(type, name, line, col));\
    ADVANCE(1)\
}

bool compare(char* str1, const char* str2, int pos)
{
    for (int i = 0; i < strlen(str2); ++i)
        if (str1[pos+i] != str2[i])
            return false;
    return true;
}

Arraylist* tokenize(char* tokens) 
{
    Arraylist* token_list = Arraylist_new(Token_free);
    int size = strlen(tokens);           

    int line = 1;
    int col = 1;
    int pos = 0;

    while (pos < size) {

        if (IS_WHITESPACE(tokens[pos])) {
            
            if (tokens[pos] == '\n') {
                //Reset to 0
                col = 0;
                ++line;
            }
            // Advance col to 1
            ADVANCE(1)
        }
        else if (IS_ALPHA(tokens[pos])) {
            COMPARE("function", Function, NULL)
            else COMPARE("if", If, NULL)
            else COMPARE("else", Else, NULL)
            else COMPARE("while", While, NULL)
            else COMPARE("var", Variable, NULL)
            else COMPARE("i64", I64, NULL)
            else COMPARE("i32", I32, NULL)
            else COMPARE("i16", I16, NULL)
            else COMPARE("i8", I8, NULL)
            else COMPARE("u64", U64, NULL)
            else COMPARE("u32", U32, NULL)
            else COMPARE("u16", U16, NULL)
            else COMPARE("u8", U8, NULL)
            else COMPARE("f64", F64, NULL)
            else COMPARE("f32", F32, NULL)
            else COMPARE("char", Char, NULL)
            else COMPARE("ptr", Ptr, NULL)
            else COMPARE("ref", Ref, NULL)
            else COMPARE("effect", Effect, NULL)
            else COMPARE("handle", Handle, NULL)
            else {
                
                StringBuilder *sb = StringBuilder_new();

                int i = 0;
        
                for (; IS_ALPHANUMERIC(tokens[pos+i]); ++i)
                {
                    StringBuilder_add(sb, tokens[pos+i]);
                }

                ADVANCE(i)
                Arraylist_add(token_list, Token_new(Identifier, StringBuilder_get(sb), line, col));

                StringBuilder_free(sb);
            }
        }
        else if (IS_NUMERIC(tokens[pos])) {
            
            int i = 0;

            StringBuilder *sb = StringBuilder_new();

            for (; IS_NUMERIC(tokens[pos+i]); ++i) {
                StringBuilder_add(sb, tokens[pos+i]);
            }
            
            ADVANCE(i)
            Arraylist_add(token_list, Token_new(Number, StringBuilder_get(sb), line, col));

            StringBuilder_free(sb);
        }
        else {
            switch (tokens[pos])
            {
            case '=':
                COMPARE_SINGLE('=', Equal, NULL, pos+1)
                else 
                    Arraylist_add(token_list, Token_new(Assign, NULL, line, col));
                ADVANCE(1)
                break;
            case '!':
                COMPARE_SINGLE('=', NotEqual, NULL, pos+1)
                else 
                    Arraylist_add(token_list, Token_new(Not, NULL, line, col));
                ADVANCE(1)
                break;
            case '<':
                COMPARE_SINGLE('=', LessEqual, NULL, pos+1)
                else 
                    Arraylist_add(token_list, Token_new(Less, NULL, line, col));
                ADVANCE(1)
                break;
            case '>':
                COMPARE_SINGLE('=', GreaterEqual, NULL, pos+1)
                else 
                    Arraylist_add(token_list, Token_new(GreaterEqual, NULL, line, col));
                ADVANCE(1)
                break;
            case '-':
                COMPARE_SINGLE('>', Arrow, NULL, pos+1)
                else
                    Arraylist_add(token_list, Token_new(Subtract, NULL, line, col));
                ADVANCE(1)
                break;
            case '+':
                Arraylist_add(token_list, Token_new(Add, NULL, line, col));
                ADVANCE(1)
                break;
            case '*':
                Arraylist_add(token_list, Token_new(Multiply, NULL, line, col));
                ADVANCE(1)
                break;
            case '/':
                Arraylist_add(token_list, Token_new(Divide, NULL, line, col));
                ADVANCE(1)
                break;
            case '%':
                Arraylist_add(token_list, Token_new(Modulus, NULL, line, col));
                ADVANCE(1)
                break;
            case '&':
                COMPARE_SINGLE('&', Or, NULL, pos+1)
                else
                    UNEXPECTED_TOKEN(pos+1)
                ADVANCE(1)
                break;
            case '|':
                COMPARE_SINGLE('|', Or, NULL, pos+1)
                else 
                    UNEXPECTED_TOKEN(pos+1)
                ADVANCE(1)
                break;
            case '.':
                Arraylist_add(token_list, Token_new(Dot, NULL, line, col));
                ADVANCE(1)
                break;
            case ';':
                Arraylist_add(token_list, Token_new(Semicolon, NULL, line, col));
                ADVANCE(1)
                break;
            case ':':
                Arraylist_add(token_list, Token_new(Colon, NULL, line, col));
                ADVANCE(1)
                break;
            case '(':
                Arraylist_add(token_list, Token_new(OpenBracket, NULL, line, col));
                ADVANCE(1)
                break;
            case ')':
                Arraylist_add(token_list, Token_new(CloseBracket, NULL, line, col));
                ADVANCE(1)
                break;
            case '[':
                Arraylist_add(token_list, Token_new(OpenSquare, NULL, line, col));
                ADVANCE(1)
                break;
            case ']':
                Arraylist_add(token_list, Token_new(CloseSquare, NULL, line, col));
                ADVANCE(1)
                break;
            case '{':
                Arraylist_add(token_list, Token_new(OpenBrace, NULL, line, col));
                ADVANCE(1)
                break;
            case '}':
                Arraylist_add(token_list, Token_new(CloseBrace, NULL, line, col));
                ADVANCE(1)
                break;
            case ',':
                Arraylist_add(token_list, Token_new(Comma, NULL, line, col));
                ADVANCE(1)
                break;

            case '"': ;
                StringBuilder* sb = StringBuilder_new();
                ADVANCE(1)
                while(tokens[pos] != '"') {
                    StringBuilder_add(sb, tokens[pos]);
                    ADVANCE(1)
                }
                Arraylist_add(token_list, Token_new(String, StringBuilder_get(sb), line, col));
                StringBuilder_free(sb);
                ADVANCE(1)
                break;
            default:
                UNEXPECTED_TOKEN(pos)
                break;
            }
        }
    }

    return token_list;
}
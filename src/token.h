#ifndef TOKEN_H
#define TOKEN_H

#define MAX_NAME_SIZE 40

enum tokentype
{
    Function,
    Variable,
    OpenBracket,
    CloseBracket,
    OpenBrace,
    CloseBrace,
    OpenSquare,
    CloseSquare,
    Number,
    String,
    Identifier,
    Assign,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulus,
    And,
    Or,
    Greater,
    Less,
    GreaterEqual,
    LessEqual,
    NotEqual,
    Not,
    Equal,
    If,
    Else,
    While,
    Semicolon,
    Colon,
    Dot,
    Arrow,
    I64,
    I32,
    I16,
    I8,
    U64,
    U32,
    U16,
    U8,
    F64,
    F32,
    Char,
    Ptr,
    Ref,
    Effect,
    Handle
};

typedef enum tokentype TokenType;

struct token
{
    int line;
    int column;
    TokenType type;
    char* name;
};

typedef struct token Token;

Token* Token_new(TokenType type, char* name, int line, int column);
void Token_free(Token* tk);

#endif
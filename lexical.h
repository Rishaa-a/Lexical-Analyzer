#ifndef LEXICAL_H
#define LEXICAL_H

#define MAX_KEYWORDS 20
#define MAX_TOKEN_SIZE 100

typedef enum
{
    KEYWORD,
    OPERATOR,
    SPECIAL_CHARACTER,

    INTEGER_CONSTANT,
    FLOAT_CONSTANT,
    CHARACTER_CONSTANT,
    STRING_CONSTANT,

    HEXADECIMAL_CONSTANT,
    OCTAL_CONSTANT,
    BINARY_CONSTANT,

    IDENTIFIER,

    ERROR_TOKEN,
    END_OF_FILE

}TokenType;

typedef struct
{
    char lexeme[MAX_TOKEN_SIZE];
    TokenType type;
} Token;

void initializeLexer(const char *filename);
Token getNextToken();
void categorizeToken(Token *token);

int isKeyword(const char *str);
int isOperator(const char *str);
int isSpecialCharacter(char ch);
int isConstant(const char *str);
int isIdentifier(const char *str);

#endif
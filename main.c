/*
Name        : RISHA A
Date        : 30-07-2026
Description : Project :- Lexical Analyzer

              Functionalities:
              1. Keyword Identification :- Identifies C language keywords such as int, float, if, while, return, etc.
              2. Identifier Recognition :- Detects valid identifiers and reports invalid identifiers.
              3. Constant Detection :- Identifies integer, floating-point, character and string literals.
              4. Operator Recognition :- Identifies arithmetic, relational, logical and assignment operators.
              5. Special Character Detection :- Detects special symbols such as (), {}, [], ; and ,.
              6. Token Categorization :- Classifies each lexeme into Keyword, Identifier, Constant, Operator or Special Character.
              7. Error Detection :- Detects lexical errors such as invalid identifiers, invalid hexadecimal, binary, octal and floating-point constants, unmatched brackets and unmatched quotes.
              8. File Handling :- Reads the input C source file, tokenizes it and displays the corresponding tokens with their categories.
*/

#include <stdio.h>
#include <stdlib.h>
#include "lexical.h"

int main(int argc,char *argv[])
{
    if(argc != 2)
    {
        printf("Usage : ./a.out <input.c>\n");
        return 1;
    }

    initializeLexer(argv[1]);

    Token token;

    while(1)
    {
        token = getNextToken();

        if(token.type == END_OF_FILE)
            break;

        switch(token.type)
        {
            case KEYWORD:
                printf("Keyword      : %s\n",token.lexeme);
                break;

            case IDENTIFIER:
                printf("Identifier   : %s\n",token.lexeme);
                break;

            case INTEGER_CONSTANT:
                printf("Integer Constant   : %s\n", token.lexeme);
                break;

            case HEXADECIMAL_CONSTANT:
                printf("Hexadecimal Constant   : %s\n", token.lexeme);
                break;
                
            case BINARY_CONSTANT:
                printf("Binary Constant   : %s\n", token.lexeme);
                break;    

            case OCTAL_CONSTANT:
                printf("Octal Constant   : %s\n", token.lexeme);
                break;

            case FLOAT_CONSTANT:
                printf("Float Constant     : %s\n", token.lexeme);
                break;

            case CHARACTER_CONSTANT:
                printf("Character Constant : %s\n", token.lexeme);
                break;

            case STRING_CONSTANT:
                printf("String Constant    : %s\n", token.lexeme);
                break;

            case OPERATOR:
                printf("Operator     : %s\n",token.lexeme);
                break;

            case SPECIAL_CHARACTER:
                printf("Special Char : %s\n",token.lexeme);
                break;

            case ERROR_TOKEN:
                printf("%s\n",token.lexeme);
                printf("\nParsing : Stopped due to lexical error\n");
                return 1;

            default:
                break;
        }
    }

    // printf("\nParsing : Done\n");

    return 0;
}
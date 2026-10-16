#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"lexical.h"

FILE *fp;

/* Counters for bracket matching */
int round_count = 0;
int curly_count = 0;
int square_count = 0;

static const char *keywords[MAX_KEYWORDS] = {"int","float","return","if","else","while","for","do","break","continue","char","double","void","switch","case","default","const","static","sizeof","struct"};

static const char *operators = "+-*/%=!<>|&";

static const char *specialCharacters = ",;{}()[]";

void initializeLexer(const char *filename)
{
    fp = fopen(filename,"r");

    if(fp == NULL)
    {
        printf("Open : %s : Failed\n",filename);
        exit(1);
    }

    printf("Open : %s : Success\n",filename);
    printf("Parsing : %s : Started\n\n",filename);
}

int isHexDigit(char ch)
{
    if(isdigit(ch))
        return 1;

    if(ch>='A' && ch<='F')
        return 1;

    if(ch>='a' && ch<='f')
        return 1;

    return 0;
}

int isBinaryDigit(char ch)
{
    return (ch=='0' || ch=='1');
}

int isOctalDigit(char ch)
{
    return (ch>='0' && ch<='7');
}


Token getNextToken()
{
    Token token;
    token.lexeme[0] = '\0';
    token.type = END_OF_FILE;

    int c;

    while((c = fgetc(fp)) != EOF)
    {
        if(isspace(c))
            continue;

        
        int i = 0;

        /* ---------- Identifier / Keyword ---------- */
        if(isalpha(c) || c == '_')
        {
            token.lexeme[i++] = c;

            while((c = fgetc(fp)) != EOF &&
                  (isalnum(c) || c == '_'))
            {
                token.lexeme[i++] = c;
            }

            token.lexeme[i] = '\0';

            if(c != EOF)
                ungetc(c, fp);

            categorizeToken(&token);
            return token;
        }

        /* ---------- Numbers ---------- */

        if(isdigit(c))
        {
            token.lexeme[i++] = c;
            int k;

            while((c = fgetc(fp)) != EOF &&
                  (isalnum(c) || c == '.' || c == '_'))
            {
                token.lexeme[i++] = c;
            }

            token.lexeme[i] = '\0';

            if(c != EOF)
                ungetc(c, fp);


            /* Hexadecimal */

            if(token.lexeme[0]=='0' &&
               (token.lexeme[1]=='x' ||
                token.lexeme[1]=='X'))
            {
                k = 2;

                if(token.lexeme[k]=='\0')
                {
                    sprintf(token.lexeme,
                    "Error : Invalid Hexadecimal Constant");
                    token.type = ERROR_TOKEN;
                    return token;
                }

                while(token.lexeme[k])
                {
                    if(!isHexDigit(token.lexeme[k]))
                    {
                        sprintf(token.lexeme,
                        "Error : Invalid Hexadecimal Constant");
                        token.type = ERROR_TOKEN;
                        return token;
                    }

                    k++;
                }

                token.type = HEXADECIMAL_CONSTANT;
                return token;
            }            

            /* Binary */

            if(token.lexeme[0]=='0' &&
               (token.lexeme[1]=='b' ||
                token.lexeme[1]=='B'))
            {
                k = 2;

                if(token.lexeme[k]=='\0')
                {
                    sprintf(token.lexeme,
                    "Error : Invalid Binary Constant");
                    token.type = ERROR_TOKEN;
                    return token;
                }

                while(token.lexeme[k])
                {
                    if(!isBinaryDigit(token.lexeme[k]))
                    {
                        sprintf(token.lexeme,
                        "Error : Invalid Binary Constant");
                        token.type = ERROR_TOKEN;
                        return token;
                    }

                    k++;
                }

                token.type = BINARY_CONSTANT;
                return token;
            }


            /* Float */

            char *dot = strchr(token.lexeme,'.');

            if(dot)
            {
                if(dot[1]=='\0')
                {
                    sprintf(token.lexeme,"Error : Invalid Float Constant");
                    token.type = ERROR_TOKEN;
                    return token;
                }

                k = (dot-token.lexeme)+1;

                while(token.lexeme[k])
                {
                    if((token.lexeme[k]=='f' ||
                        token.lexeme[k]=='F') &&
                       token.lexeme[k+1]=='\0')
                    {
                        break;
                    }

                    if(!isdigit(token.lexeme[k]))
                    {
                        sprintf(token.lexeme,"Error : Invalid Float Constant");
                        token.type = ERROR_TOKEN;
                        return token;
                    }

                    k++;
                }

                token.type = FLOAT_CONSTANT;
                return token;
            }
            
            /* Octal */

            if(token.lexeme[0]=='0' &&strlen(token.lexeme)>1)
            {
                k=1;

                while(token.lexeme[k])
                {
                    if(!isOctalDigit(token.lexeme[k]))
                    {
                        sprintf(token.lexeme,
                                "Error : Invalid Octal Constant");
                        token.type=ERROR_TOKEN;
                        return token;
                    }
                    k++;
                }

                token.type=OCTAL_CONSTANT;
                return token;
            }

            /* Invalid Identifier */

            k = 1;

            while(token.lexeme[k])
            {
                if(isalpha(token.lexeme[k]) &&
                  !(token.lexeme[0]=='0' &&
                   (token.lexeme[1]=='x' ||
                    token.lexeme[1]=='X' ||
                    token.lexeme[1]=='b' ||
                    token.lexeme[1]=='B')))
                {
                    sprintf(token.lexeme,"Error : Invalid Identifier");
                    token.type = ERROR_TOKEN;
                    return token;
                }
                k++;
            }


            token.type = INTEGER_CONSTANT;
            return token;
        }
        /* ---------- String Literal ---------- */

        if(c == '"')
        {
            token.lexeme[i++] = c;

            int found = 0;

            while((c = fgetc(fp)) != EOF)
            {
                if(c=='\n')
                    break;

                token.lexeme[i++] = c;

                if(c == '"')
                {
                    found = 1;
                    break;
                }
            }

            token.lexeme[i] = '\0';

            if(!found)
            {
                sprintf(token.lexeme,"Error : Double quotes opened but not closed");
                token.type = ERROR_TOKEN;
                return token;
            }

            token.type = STRING_CONSTANT;
            return token;
        }

        /* ---------- Character Literal ---------- */

        if(c=='\'')
        {
            token.lexeme[i++]=c;

            int found=0;

            while((c=fgetc(fp))!=EOF)
            {
                if(c=='\n')
                    break;
                
                token.lexeme[i++]=c;

                if(c=='\'')
                {
                    found=1;
                    break;
                }
            }

            token.lexeme[i]='\0';

            if(!found)
            {
                sprintf(token.lexeme,
                        "Error : Single quotes opened but not closed");
                token.type=ERROR_TOKEN;
                return token;
            }

            /* Validate character constant */

            int len = strlen(token.lexeme);

            /* Valid: 'A' -> length = 3 */
            if(len == 3)
            {
                token.type = CHARACTER_CONSTANT;
                return token;
            }

            /* Valid escape sequence: '\n' -> length = 4 */
            if(len == 4 && token.lexeme[1] == '\\')
            {
                token.type = CHARACTER_CONSTANT;
                return token;
            }

            sprintf(token.lexeme,
                    "Error : Invalid Character Constant");
            token.type = ERROR_TOKEN;
            return token;
        }

        /* ---------- Multi-character Operators ---------- */

        token.lexeme[0] = c;
        token.lexeme[1] = '\0';

        if(c=='=' || c=='!' || c=='<' || c=='>' ||
           c=='+' || c=='-' || c=='&' || c=='|')
        {
            int next = fgetc(fp);

            if((c=='=' && next=='=') ||
               (c=='!' && next=='=') ||
               (c=='<' && next=='=') ||
               (c=='>' && next=='=') ||
               (c=='+' && next=='+') ||
               (c=='-' && next=='-') ||
               (c=='&' && next=='&') ||
               (c=='|' && next=='|'))
            {
                token.lexeme[0] = c;
                token.lexeme[1] = next;
                token.lexeme[2] = '\0';
            }
            else
            {
                if(next != EOF)
                    ungetc(next, fp);
            }

            token.type = OPERATOR;
            return token;
        }

        /* ---------- Bracket Count ---------- */

        if(c == '(')
            round_count++;

        else if(c == ')')
            round_count--;

        else if(c == '{')
            curly_count++;

        else if(c == '}')
            curly_count--;

        else if(c == '[')
            square_count++;

        else if(c == ']')
            square_count--;

        categorizeToken(&token);
        return token;
    }

    /* ---------- EOF ---------- */

    if(round_count != 0)
        printf("Error : '(' opened but not closed\n");

    if(curly_count != 0)
        printf("Error : '{' opened but not closed\n");

    if(square_count != 0)
        printf("Error : '[' opened but not closed\n");

    token.type = END_OF_FILE;
    return token;
}
void categorizeToken(Token *token)
{
    if(isKeyword(token->lexeme))
        token->type = KEYWORD;

    else if(isOperator(token->lexeme))
        token->type = OPERATOR;

    else if(strlen(token->lexeme) == 1 &&
            isSpecialCharacter(token->lexeme[0]))
        token->type = SPECIAL_CHARACTER;


    else if(isIdentifier(token->lexeme))
        token->type = IDENTIFIER;

    else
        token->type = ERROR_TOKEN;
}
int isKeyword(const char *str)
{
    int i;

    for(i = 0; i < MAX_KEYWORDS; i++)
    {
        if(strcmp(str, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

int isOperator(const char *str)
{
    char *op[] ={"+","-","*","/","%","=","==","!=","<",">","<=",">=","++","--","&&","||","&","|","!"};

    int i;

    for(i = 0; i < 19; i++)
    {
        if(strcmp(str, op[i]) == 0)
            return 1;
    }

    return 0;
}
int isSpecialCharacter(char ch)
{
    return strchr(specialCharacters, ch) != NULL;
}
int isConstant(const char *str)
{
    int i;
    int dot = 0;

    if(str[0] == '"' && str[strlen(str)-1] == '"')
        return 1;

    if(str[0] == '\'' && str[strlen(str)-1] == '\'')
        return 1;

    for(i = 0; str[i]; i++)
    {
        if(str[i] == '.')
        {
            dot++;

            if(dot > 1)
                return 0;
        }

        else if(!isdigit(str[i]))
            return 0;
    }

    return strlen(str) > 0;
}
int isIdentifier(const char *str)
{
    int i;

    if(!(isalpha(str[0]) || str[0] == '_'))
        return 0;

    for(i = 1; str[i]; i++)
    {
        if(!(isalnum(str[i]) || str[i] == '_'))
            return 0;
    }

    return 1;
}
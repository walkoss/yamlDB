/*
**  Filename: parser.h
**
**  Made by: Walkoss on 22/11/2017.
**
**  Description: 
*/

#include "../database/database.h"
#include "../lexer/lexer.h"
#include "../statement/statement.h"

#ifndef YAMLDB_PARSER_H
#define YAMLDB_PARSER_H

typedef struct {
    Lexer *lexer;
    char *error;
    int hasError;
} Parser;

typedef struct SStmtFunction {
    int (*functionPtr)(Parser *, Database **);
} StmtFunction;

Parser *parserInit(char *buffer);

void parserFree(Parser *parser);

int parse(Parser *parser, Database **database);

int is(Parser *parser, TokenType tokenType);

int accept(Parser *parser, TokenType tokenType);

#endif //YAMLDB_PARSER_H

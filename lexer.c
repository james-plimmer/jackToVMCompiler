/************************************************************************
University of Leeds
School of Computing
COMP2932- Compiler Design and Construction
Lexer Module

I confirm that the following code has been developed and written by me and it is entirely the result of my own work.
I also confirm that I have not copied any parts of this program from another person or any other source or facilitated someone to copy this program from me.
I confirm that I will not publish the program online or share it with anyone without permission of the module leader.

Student Name: James Plimmer
Student ID: 201523428
Email: sc21jpwp@leeds.ac.uk
Date Work Commenced: 17/2/2023
*************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// YOU CAN ADD YOUR OWN FUNCTIONS, DECLARATIONS AND VARIABLES HERE
/* error code to detect EOF in a comment    */
#define EOFCOM (-2)


/* array of all keywords - ending with boolean false            */
const char* KEYWORDS[] = {"class", "constructor", "method", "function", "int", "boolean", "char", "void",
                          "var", "static", "field", "let", "do", "if", "else", "while", "return",
                          "true", "false", "null", "this", 0};

/* array of all legal symbols - ended with boolean false        */
const char SYMBS[] = {'(',')', '[', ']', '{', '}', '<', '>', '+', '-', '*', '/',
                      '=', ',', ';', '.', '&', '|', '~', 0};


/* peekNextToken stack - matches max size of the lexeme   */
int stack[128];
/* pointer to top of stack - initialised to -1 for an empty stack   */
int sP;
/* pointer to the file                          */
FILE* sCode;
/* file name                                    */
char filename[128];
/* variable to hold value of next read char     */
int nextChar;
/* variable to hold current line number         */
int ln;
/* determine if '/'  and following char needs replacing after reading potential comment     */
int replaceChars;
/* declare token    */
Token tok;
/* push and pop methods below are used throughout token identifier/builder methods
 * to enable the peekNextToken method to put the read chars back
 * the stack is not used in getNextToken                            */
/* push to the stack                            */
void push(int c){
    sP ++;
    stack[sP] = c;
}
/* pop from stack                               */
int pop(){
    int c = stack[sP];
    sP--;
    return c;
}

/* read next character, checking for newline    */
int readNext(){
    nextChar = getc(sCode);
    if (nextChar == '\n') ln++;

    return nextChar;
}

/* remove leading whitespace                    */
int rmWhitespace(){

    /* pass through all whitespace              */
    while (isspace(nextChar)){
        nextChar = readNext();
    }

    /* return first non-whitespace character    */
    return nextChar;
}


/* find the end of a multi-line comment - checking for unexpected EOF   */
int findComEnd(){
    /* read next character                  */
    nextChar = readNext();
    if (nextChar == EOF) return nextChar = EOFCOM;

    /* keep reading comment until a '*' is found    */
    while (nextChar != '*') {
        nextChar = readNext();
        if (nextChar == EOF) return nextChar = EOFCOM;
    }

    /* check if char after the '*' is a '/' - closing the comment */
    nextChar = readNext();
    if (nextChar == EOF) return nextChar = EOFCOM;
    if (nextChar == '/'){
        nextChar = readNext();
    }
    /* if not, keep searching for the next '*'      */
    else {
        findComEnd();
    }

    return nextChar;
}

/* comment remover                              */
int skipComment(){

    if (nextChar == '/'){
        nextChar = getc(sCode);
        /* single line comment                */
        if (nextChar == '/'){
            /* scan characters until new line or EOF is reached */
            while (nextChar != '\n' && nextChar != EOF){
                nextChar = getc(sCode);
            }
            if (nextChar == '\n'){
                ln++;
                nextChar = readNext();
            }
        }

        /* multi-line comment                 */
        else if (nextChar == '*'){
            findComEnd();
        }

        /* not a comment, put char  and '/' back       */
        else {
            replaceChars = 1;
        }
    }

    return nextChar;
}

int findNextT(){

    /* get next character                       */
    nextChar = readNext();

    replaceChars = 0;
    /* check to remove whitespace or a potential comment  */
    while (isspace(nextChar) || nextChar == '/'){
        nextChar = rmWhitespace();
        nextChar = skipComment();
    }

    /* replace slash and following non-space char if '/' wasn't the start of a comment   */
    if (replaceChars == 1){
        ungetc(nextChar, sCode);
        ungetc('/', sCode);
        nextChar = readNext();
    }

    return nextChar;
}


/* EOF token              */
Token eof(Token t){

    t.tp = EOFile;
    strcpy(t.lx,"End Of File");
    t.ln = ln;

    return t;
}

/* string token         */
Token string(Token t){
    nextChar = getc(sCode);
    push(nextChar);

    /* lexeme index to assign char to       */
    int i = 0;
    while(nextChar != '\"'){
        /* check for EOF mid-string         */
        if (nextChar == EOF){
            t.tp = ERR;
            strcpy(t.lx, "Error: unexpected eof in string constant");
            t.ec = EofInStr;
            t.ln = ln;
            return t;
        }

        /* check for newline mid-string         */
        if (nextChar == '\n'){
            t.tp = ERR;
            strcpy(t.lx, "Error: new line in string constant");
            t.ec = NewLnInStr;
            t.ln = ln;
            return t;
        }

        /* otherwise add character to string */
        /* truncate lexemes longer than 128 chars */
        if (i < 127) {
            t.lx[i] = (char) nextChar;
            i++;
        }
        nextChar = getc(sCode);
        push(nextChar);
    }
    /* add end of string char to lexeme */
    t.lx[i] = '\0';

    /* create string literal token  */
    t.tp = STRING;
    t.ln = ln;

    return t;
}

/* keyword or ID token      */
Token keyID(Token t){
    /* lexeme index to assign char to       */
    int i = 0;
    /* add trailing alphanumeric characters to lexeme */
    while (isalnum(nextChar) || nextChar=='_'){
        /* truncate lexemes longer than 128 chars */
        if (i < 127) {
            t.lx[i] = (char) nextChar;
            i++;
        }
        nextChar = getc(sCode);
        push(nextChar);
    }
    /* add end of string char to lexeme */
    t.lx[i] = '\0';
    /* put non-alphanumeric char back  */
    ungetc(nextChar, sCode);
    pop();

    /* determine token type by looking up lexeme in keyword list */
    /* index all keywords               */
    int j = 0;
    /* for all keywords                             */
    while (KEYWORDS[j]){
        /* if lexeme is a keyword               */
        if (strcmp(KEYWORDS[j], t.lx) == 0){
            /* create reserved word token           */
            t.tp = RESWORD;
            t.ln = ln;
            return t;
        }
        j++;
    }

    /* otherwise must be an identifier     */
    t.tp = ID;
    t.ln = ln;
    return t;
}

/* integer token assigner   */
Token integer(Token t){
    /* lexeme index to assign char to       */
    int i = 0;
    /* add trailing numeric characters to lexeme */
    while (isdigit(nextChar)){
        /* truncate lexemes longer than 128 chars */
        if (i < 127){
            t.lx[i] = (char)nextChar;
            i++;
        }
        nextChar = getc(sCode);
        push(nextChar);
    }
    /* add end of string char to lexeme */
    t.lx[i] = '\0';
    /* put non-digit char back  */
    ungetc(nextChar, sCode);
    pop();

    /* create token         */
    t.tp = INT;
    t.ln = ln;
    return t;
}

/* symbol token assigner        */
Token symbol(Token t){
    /* determine if symbol is legal by looking up lexeme in legal symbol list   */
    /* index all legal symbols               */
    int j = 0;
    /* for all symbols                             */
    while (SYMBS[j]){
        /* if lexeme is a legal symbol               */
        if (SYMBS[j] == nextChar){
            /* create symbol token           */
            t.tp = SYMBOL;
            t.lx[0] = (char)nextChar;
            /* add end of string char to lexeme */
            t.lx[1] = '\0';
            t.ln = ln;
            return t;
        }
        j++;
    }

    /* otherwise must be an illegal symbol     */
    t.tp = ERR;
    strcpy(t.lx, "Error: illegal symbol in source file");
    t.ec = IllSym;
    t.ln = ln;
    return t;
}

// IMPLEMENT THE FOLLOWING functions
//***********************************

// Initialise the lexer to read from source file
// file_name is the name of the source file
// This requires opening the file and making any necessary initialisations of the lexer
// If an error occurs, the function should return 0
// if everything goes well the function should return 1
int InitLexer (char* file_name){

    strcpy(filename, file_name);
    /* attempt to open JACK source file       */
    sCode = fopen(file_name, "r");
    /* ensures file opens correctly             */
    if (sCode == NULL){
        printf("Error: File failed to open.\n");
        return 0;
    }

    sP = -1;
    /* set ln to 1                          */
    ln = 1;

    /* successful file open                     */
    return 1;
}


// Get the next token from the source file
Token GetNextToken (){

    strcpy(tok.fl, filename);

    /* consume all leading whitespace and comments        */
    nextChar = findNextT();

    /* EOF in a comment                       */
    if (nextChar == EOFCOM){
        tok.tp = ERR;
        strcpy(tok.lx, "Error: unexpected eof in comment");
        tok.ec = EofInCom;
        tok.ln = ln;
        return tok;
    }

    /* check for EOF                          */
    if (nextChar == EOF){
        tok = eof(tok);
        return tok;
    }

    /* string literal                         */
    else if (nextChar == '\"'){
        push('\"');
        tok = string(tok);
        return tok;
    }

    /* keyword or identifier        */
    else if (isalpha(nextChar) || nextChar == '_'){
        tok = keyID(tok);
        return tok;
    }

    /* number               */
    else if (isdigit(nextChar)){
        tok = integer(tok);
        return tok;
    }

    /* if none of above, must be a symbol       */
    else{
        tok = symbol(tok);
        return tok;
    }
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken (){

    strcpy(tok.fl, filename);
    sP = -1;

    /* same process as getNextToken for whitespace and comments, as these are not tokens, hence can be removed */
    /* consume all leading whitespace and comments        */
    nextChar = findNextT();

    /* EOF in a comment                       */
    if (nextChar == EOFCOM){
        tok.tp = ERR;
        strcpy(tok.lx, "Error: unexpected eof in comment");
        tok.ec = EofInCom;
        tok.ln = ln;
        return tok;
    }


    /* peekNextToken methods must now not consume the characters as they are read
     * this is done by pushing characters read from the file to a stack, popping to ungetc after the token is read   */
    /* push char read by the whitespace / comment skipper   */
    push(' ');
    push(nextChar);

    /* eof                  */
    if (nextChar == EOF){
        tok = eof(tok);
    }

    /* string literal                         */
    else if (nextChar == '\"'){
        tok = string(tok);
    }

    /* keyword or identifier        */
    else if (isalpha(nextChar) || nextChar=='_'){
        tok = keyID(tok);
    }

    /* number               */
    else if (isdigit(nextChar)){
        tok = integer(tok);
    }

    /* must be a symbol if none of the above    */
    else {
        tok = symbol(tok);
    }

    /* pop all elements in the stack, replacing them into the source file   */
    while (sP != -1){
        int lastChar = pop();
        ungetc(lastChar, sCode);
    }

    return tok;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer (){
    /* close file                           */
    fclose(sCode);
    /* empty filename */
    memset(filename, 0, 128);
    return 0;
}

// do not remove the next line
#ifndef TEST
int main(int argc, char *argv[]){
    // implement your main function here
    // NOTE: the auto-grader will not use your main function
    if (argc == 2) {
        InitLexer(argv[1]);
        Token t, s;

        s = PeekNextToken();
        t = GetNextToken();
        while (t.tp != EOFile) {
            printf("<PEEK: %s, %d, %s, %d >\n", s.fl, s.ln, s.lx, s.tp);
            printf("< GET: %s, %d, %s, %d >\n", t.fl, t.ln, t.lx, t.tp);
            if (t.tp == ERR) {
                exit(1);
            }
            s = PeekNextToken();
            t = GetNextToken();
        }
        printf("<PEEK: %s, %d, %s, %d >\n", s.fl, s.ln, s.lx, s.tp);
        printf("< GET: %s, %d, %s, %d >\n", t.fl, t.ln, t.lx, t.tp);

        StopLexer();
        return 0;
    }

    else {
        return 1;
    }

}
// do not remove the next line
#endif
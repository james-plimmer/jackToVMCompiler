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
#define ERRTOK (-2)


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
int sP = -1;
/* pointer to the file                          */
FILE* sCode;
/* file name                                    */
char* filename;
/* variable to hold value of next read char     */
int nextChar;
/* variable to hold current line number         */
int ln = 1;
/* determine if '/' needs replacing after reading potential comment     */
int replaceSlash;

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
    if (nextChar == EOF) return nextChar = ERRTOK;

    /* keep reading comment until a '*' is found    */
    while (nextChar != '*') {
        nextChar = readNext();
        if (nextChar == EOF) return nextChar = ERRTOK;
    }

    /* check if char after the '*' is a '/' - closing the comment */
    nextChar = readNext();
    if (nextChar == EOF) return nextChar = ERRTOK;
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
            ungetc(nextChar, sCode);
            replaceSlash = 1;
        }
    }

    return nextChar;
}

int findNextT(){

    /* get next character                       */
    nextChar = readNext();

    replaceSlash = 0;
    /* check to remove whitespace or a potential comment  */
    while (isspace(nextChar) || nextChar == '/'){
        nextChar = rmWhitespace();
        nextChar = skipComment();
    }

    /* replace slash if '/' wasn't the start of a comment   */
    if (replaceSlash == 1){
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
            strcpy(t.lx, "Error: unexpected newline in string constant");
            t.ec = NewLnInStr;
            t.ln = ln;
            return t;
        }

        /* otherwise add character to string */
        t.lx[i] = (char)nextChar;
        i++;
        nextChar = getc(sCode);
    }

    /* create string literal token  */
    t.tp = STRING;
    t.ln = ln;

    /* skip past closing '"'      */
    nextChar = readNext();
    return t;
}


/* keyword or ID token      */
Token keyID(Token t){
    /* lexeme index to assign char to       */
    int i = 0;
    /* add trailing alphanumeric characters to lexeme */
    while (isalnum(nextChar)){
        t.lx[i] = (char)nextChar;
        i++;
        nextChar = getc(sCode);
    }
    /* put non-alphanumeric char back  */
    ungetc(nextChar, sCode);

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
        t.lx[i] = (char)nextChar;
        i++;
        nextChar = getc(sCode);
    }
    /* put non-digit char back  */
    ungetc(nextChar, sCode);

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
            t.ln = ln;
            return t;
        }
        j++;
    }

    /* otherwise must be an illegal symbol     */
    t.tp = ERR;
    strcpy(t.lx, "ERROR: Illegal Symbol");
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

    filename = file_name;
    /* attempt to open JACK source file       */
    sCode = fopen(file_name, "r");

    /* ensures file correctly             */
    if (sCode == NULL){
        printf("Error: File failed to open.\n");
        return 0;
    }
    /* successful file open                     */
    return 1;
}


// Get the next token from the source file
Token GetNextToken (){

    /* initialise token                       */
	Token t;
    strcpy(t.fl, filename);

    /* clear lexeme                     */
    memset(t.lx,0,sizeof(t.lx));

    /* consume all leading whitespace and comments        */
    nextChar = findNextT();

    /* EOF in a comment                       */
    if (nextChar == ERRTOK){
        t.tp = ERR;
        strcpy(t.lx, "Error: unexpected eof in comment");
        t.ec = EofInCom;
        t.ln = ln;
        return t;
    }

    /* check for EOF                          */
    if (nextChar == EOF){
        t= eof(t);
        return t;
    }

    /* string literal                         */
    else if (nextChar == '\"'){
        t = string(t);
        return t;
    }

    /* keyword or identifier        */
    else if (isalpha(nextChar)){
        t = keyID(t);
        return t;
    }

    /* number               */
    else if (isdigit(nextChar)){
        t = integer(t);
        return t;
    }

    /* if none of above, must be a symbol       */
    else{
        t = symbol(t);
        return t;
    }
}

// peek (look) at the next token in the source file without removing it from the stream
//TODO: stack implementation, each time new character is read, push to stack (modified readNext method)
Token PeekNextToken (){
    /* initialise token                       */
    Token t;
    strcpy(t.fl, filename);

    /* same process as getNextToken for whitespace and comments, as these are not tokens, hence can be removed */
    /* clear lexeme                     */
    memset(t.lx,0,sizeof(t.lx));

    /* consume all leading whitespace and comments        */
    nextChar = findNextT();

    /* EOF in a comment                       */
    if (nextChar == ERRTOK){
        t.tp = ERR;
        strcpy(t.lx, "Error: unexpected eof in comment");
        t.ec = EofInCom;
        t.ln = ln;
        return t;
    }


    /* getNextToken methods must now not consume the characters as they are read
     * this is done by pushing characters read from the file to a stack, popping to ungetc after the token is read   */
    push(nextChar);
    if (nextChar == EOF){
        t = eof(t);
    }
    else {
        t = symbol(t);
    }


    while (sP != -1){
        printf("%d", sP);
        int lastChar = pop();
        ungetc(lastChar, sCode);
    }

    return t;
}

// clean out at end, e.g. close files, free memory, ... etc
int StopLexer (){
    /* close file                           */
    fclose(sCode);
	return 0;
}

// do not remove the next line
#ifndef TEST
int main(int argc, char *argv[]){
    // implement your main function here
    // NOTE: the auto-grader will not use your main function
    if (argc == 2) {
        InitLexer(argv[1]);
        Token t;
        for (int i = 0; i < 4; i ++){
            t = PeekNextToken();
            printf("< %s, %d, %s, %d >\n", t.fl, t.ln, t.lx, t.tp);
        }


//        t = GetNextToken();
//        printf("GET");
//        while (t.tp != EOFile) {
//            printf("< %s, %d, %s, %d >\n", t.fl, t.ln, t.lx, t.tp);
//            if (t.tp == ERR) {
//                exit(1);
//            }
//            t = GetNextToken();
//        }
//        printf("< %s, %d, %s, %d >\n", t.fl, t.ln, t.lx, t.tp);

        StopLexer();
        return 0;
    }

    else {
        return 1;
    }

}
// do not remove the next line
#endif
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
#define ERRTOK (-2)


/* pointer to the file                          */
FILE* sCode;
/* file name                                    */
char* filename;
/* variable to hold value of next read char     */
int nextChar;
/* variable to hold current line number         */
int ln = 1;


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

int findComEnd(){
    nextChar = readNext();
    if (nextChar == EOF) return nextChar = ERRTOK;

    while (nextChar != '*') {
        nextChar = readNext();
        if (nextChar == EOF) return nextChar = ERRTOK;
    }

    nextChar = readNext();
    if (nextChar == EOF) return nextChar = ERRTOK;

    if (nextChar == '/'){
        nextChar = getc(sCode);
    }
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

        /* not a comment, put char back       */
        else {
            ungetc(nextChar, sCode);    //TODO:confirm what nextChar is now storing
        }
    }

    return nextChar;
}

int findNextT(){

    /* get next character                       */
    nextChar = readNext();

    /* check to remove whitespace or a potential comment  */
    while (isspace(nextChar) || nextChar == '/'){
        nextChar = rmWhitespace();
        nextChar = skipComment();
    }

    return nextChar;
}


/* see if next char is EOF, creating token appropriately if so  */
Token checkEOF(Token t){

    if (nextChar == EOF){
        t.tp = EOFile;
        strcpy(t.lx,"End Of File");
        t.ln = ln;
        strcpy(t.fl, filename);
    }

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
    t.tp = ERR;

    /* consume all leading whitespace and comments        */
    nextChar = findNextT();

    /* EOF in a comment                       */
    if (nextChar == ERRTOK){
        t.tp = ERR;
        strcpy(t.lx, "Error: unexpected eof in comment");
        t.ec = EofInCom;
        t.ln = ln;
        strcpy(t.fl, filename);
        return t;
    }

    /* string literal                         */
    if (nextChar == '\"'){
        /* create empty string to append to */
        char str[0];
        nextChar = getc(sCode);
        while(nextChar != '\"'){
            /* check for EOF mid-string         */
            if (nextChar == EOF){
                t.tp = ERR;
                strcpy(t.lx, "Error: unexpected eof in string constant");
                t.ec = EofInStr;
                t.ln = ln;
                strcpy(t.fl, filename);
                return t;
            }

            /* check for newline mid-string         */
            if (nextChar == '\n'){
                t.tp = ERR;
                strcpy(t.lx, "Error: unexpected newline in string constant");
                t.ec = NewLnInStr;
                t.ln = ln;
                strcpy(t.fl, filename);
                return t;
            }

            /* otherwise add character to string */
            //TODO:implement string builder to build lexeme for token 
            nextChar = getc(sCode);
        }
    }

    /* check for EOF                          */
    t = checkEOF(t);
    if (t.tp == EOFile) return t;

    return t;
}

// peek (look) at the next token in the source file without removing it from the stream
Token PeekNextToken (){

    /* initialise token                       */
    Token t;
    t.tp = ERR;

    /* still remove all whitespace, not needing to be replaced after the peek   */
    nextChar = rmWhitespace();
    /* check for EOF                          */
    t = checkEOF(t);
    /* replace checked character              */
    ungetc(nextChar, sCode);
    if (t.tp == EOFile) return t;

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
    // NOTE: the autograder will not use your main function
    InitLexer(argv[1]);
    Token t;
    //t = PeekNextToken();
    //printf("< %s, %d, %s, %d >\n", t.fl, t.ln, t.lx, t.tp);
    t = GetNextToken();
    printf("< %s, %d, %s, %d >\n", t.fl, t.ln, t.lx, t.tp);
    StopLexer();
	return 0;
}
// do not remove the next line
#endif

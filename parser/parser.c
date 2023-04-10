#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"


/* function prototypes      */
ParserInfo classDeclar(ParserInfo pi);
ParserInfo memberDeclar(ParserInfo pi);
ParserInfo classVarDeclar(ParserInfo pi);
ParserInfo type(ParserInfo pi);
ParserInfo subroutineDeclar(ParserInfo pi);
ParserInfo paramList(ParserInfo pi);
ParserInfo subroutineBody(ParserInfo pi);
ParserInfo statement(ParserInfo pi);
ParserInfo varDeclarStatement(ParserInfo pi);
ParserInfo letStatement(ParserInfo pi);
ParserInfo ifStatement(ParserInfo pi);
ParserInfo whileStatement(ParserInfo pi);
ParserInfo doStatement(ParserInfo pi);
ParserInfo subroutineCall(ParserInfo pi);
ParserInfo expressionList(ParserInfo pi);
ParserInfo returnStatement(ParserInfo pi);
ParserInfo expression(ParserInfo pi);
ParserInfo relationalExpression(ParserInfo pi);
ParserInfo arithmeticExpression(ParserInfo pi);
ParserInfo term(ParserInfo pi);
ParserInfo factor(ParserInfo pi);
ParserInfo operand(ParserInfo pi);

//TODO: modularise common token checks

/* function definitions         */
ParserInfo classDeclar(ParserInfo pi){
    /* class keyword            */
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, "class") == 0){
        // pass
    }
    else{
        pi.er = classExpected;
        pi.tk = tok;
        return pi;
    }
    /* class identifier         */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (tok.tp == ID){
        //pass
    }
    else{
        pi.er = idExpected;
        pi.tk = tok;
        return pi;
    }
    /* {                        */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, "{") == 0){
        //pass
    }
    else{
        pi.er = openBraceExpected;
        pi.tk = tok;
        return pi;
    }
    /* 0 or more member declarations    */
    tok = PeekNextToken();    // use peak as can be 0 or more
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "static") == 0) ||
           (strcmp(tok.lx, "field") == 0) ||
           (strcmp(tok.lx, "function") == 0) || // check for member declarations
           (strcmp(tok.lx, "constructor") == 0) ||
           (strcmp(tok.lx, "method") == 0)){

        pi = memberDeclar(pi);
        /* check for no errors in member parsing    */
        if (pi.er != none){
            return pi;
        }
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
    }
    /* }                        */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, "}") == 0){
        //pass
    }
    else{
        pi.er = closeBraceExpected;
        pi.tk = tok;
        return pi;
    }
    return pi;
}

ParserInfo memberDeclar(ParserInfo pi){
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "static") == 0) || (strcmp(tok.lx, "field") == 0)){ // check if a class variable
        pi = classVarDeclar(pi);
    }
    else if ((strcmp(tok.lx, "constructor") == 0) || (strcmp(tok.lx, "function") == 0) || (strcmp(tok.lx, "method") == 0)){ // subroutine declaration
        pi = subroutineDeclar(pi);
    }
    else {
        pi.er = memberDeclarErr;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo classVarDeclar(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, "static") == 0){
        //pass
    }
    else if (strcmp(tok.lx, "field") == 0){
        //pass
    }
    else {
        pi.er = classVarErr;
        pi.tk = tok;
        return pi;
    }
    /* check for type keyword or identifier  */
    pi = type(pi);
    /* check for errors     */
    if (pi.er != none) return pi;
    /* check next token is an identifier */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (tok.tp == ID){
        //pass
    }
    else{
        pi.er = idExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for 0 or more ',identifier' tokens */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while (strcmp(tok.lx,",") == 0){
        GetNextToken(); //eat the ','
        /* check next token is an identifier */
        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (tok.tp == ID){
            //pass
        }
        else{
            pi.er = idExpected;
            pi.tk = tok;
            return pi;
        }
        /* peek to look for another "," */
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
    }
    /* check for ";"    */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,";") == 0){
        //pass
    }
    else{
        pi.er = semicolonExpected;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo type(ParserInfo pi){
    /* check for type keyword or identifier  */
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "int") == 0) ||
        (strcmp(tok.lx, "char") == 0) || // check for int char or boolean
        (strcmp(tok.lx, "boolean") == 0) ||
        (tok.tp == ID)){ // check for identifier
        //pass
    }
    else {
        pi.er = illegalType;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo subroutineDeclar(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, "constructor") == 0){
        //pass
    }
    else if (strcmp(tok.lx, "function") == 0){
        //pass
    }
    else if (strcmp(tok.lx, "method") == 0){
        //pass
    }
    else {
        pi.er = subroutineDeclarErr;
        pi.tk = tok;
        return pi;
    }
    /* check for a type or a void */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, "void") == 0){
        GetNextToken(); // eat the void
    }
    else if ((strcmp(tok.lx, "int") == 0) ||
             (strcmp(tok.lx, "char") == 0) || // check for int char or boolean
             (strcmp(tok.lx, "boolean") == 0) ||
             (tok.tp == ID)){
        pi = type(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
    }
    else{
        pi.er = illegalType;
        pi.tk = tok;
        return pi;
    }
    /* check next token is an identifier */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (tok.tp == ID){
        //pass
    }
    else{
        pi.er = idExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for "("    */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, "(") == 0){
        //pass
    }
    else{
        pi.er = openParenExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for parameter list */
    pi = paramList(pi);
    /* check for errors     */
    if (pi.er != none) return pi;
    /* check for ")"    */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, ")") == 0){
        //pass
    }
    else {
        pi.er = closeParenExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for subroutine body    */
    pi = subroutineBody(pi);
    return pi;
}

ParserInfo paramList(ParserInfo pi){
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "int") == 0) ||
        (strcmp(tok.lx, "char") == 0) || // check for int char or boolean
        (strcmp(tok.lx, "boolean") == 0) ||
        (tok.tp == ID)){
        pi = type(pi);
        /* check for errors     */
        if (pi.er != none) return pi;

        /* check for identifier */
        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (tok.tp == ID){
            //pass
        }
        else{
            pi.er = idExpected;
            pi.tk = tok;
            return pi;
        }

        /* check for 0 or more ', type identifier' tokens */
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        while (strcmp(tok.lx,",") == 0){
            GetNextToken(); // eat the ','
            /* check for type token */
            pi = type(pi);
            /* check next token is an identifier */
            tok = GetNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
            if (tok.tp == ID){
                //pass
            }
            else{
                pi.er = idExpected;
                pi.tk = tok;
                return pi;
            }
            /* peek to look for another "," */
            tok = PeekNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
        }
    }
    return pi;
}

ParserInfo subroutineBody(ParserInfo pi){
    /* check for "{"    */
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,"{") == 0) {
        //pass
    }
    else {
        pi.er = openBraceExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for a statement    */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "var") == 0) ||
           (strcmp(tok.lx, "while") == 0) ||
           (strcmp(tok.lx, "if") == 0) || // check for statement declarations
           (strcmp(tok.lx, "let") == 0) ||
           (strcmp(tok.lx, "do") == 0) ||
           (strcmp(tok.lx, "return") == 0)){

        pi = statement(pi);
        /* check for errors */
        if (pi.er != none){
            return pi;
        }
        else{
            tok = PeekNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
        }
    }
    /* check for "}"    */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,"}") == 0) {
        //pass
    }
    else {
        pi.er = closeBraceExpected;
        pi.tk = tok;
        return pi;
    }
    return pi;
}

ParserInfo statement(ParserInfo pi){
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check which type of statement */
    if (strcmp(tok.lx,"var") == 0){
        pi = varDeclarStatement(pi);
    }
    else if (strcmp(tok.lx,"let") == 0){
        pi = letStatement(pi);
    }
    else if (strcmp(tok.lx,"if") == 0){
        pi = ifStatement(pi);
    }
    else if (strcmp(tok.lx,"while") == 0){
        pi = whileStatement(pi);
    }
    else if (strcmp(tok.lx,"do") == 0){
        pi = doStatement(pi);
    }
    else if (strcmp(tok.lx,"return") == 0){
        pi = returnStatement(pi);
    }
    /* not a statement  */
    else {
        pi.er = syntaxError;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo varDeclarStatement(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for var keyword    */
    if (strcmp(tok.lx,"var") == 0){
        //pass
    }
    else {
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }
    /* check for type   */
    pi = type(pi);
    /* check for errors in type */
    if (pi.er != none) return pi;
    /* check for identifier     */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (tok.tp == ID){
        //pass
    }
    else{
        pi.er = idExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for 0 or more ',identifier' tokens */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while (strcmp(tok.lx,",") == 0){
        GetNextToken(); // eat the ','
        /* check next token is an identifier */
        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (tok.tp == ID){
            //pass
        }
        else{
            pi.er = idExpected;
            pi.tk = tok;
            return pi;
        }
        /* peek to look for another "," */
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
    }
    /* check for ";"    */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,";") == 0){
        //pass
    }
    else{
        pi.er = semicolonExpected;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo letStatement(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for let keyword    */
    if (strcmp(tok.lx,"let") == 0){
        //pass
    }
    else {
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }
    /* check for identifier     */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (tok.tp == ID){
        //pass
    }
    else{
        pi.er = idExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for "["    */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,"[") == 0){
        GetNextToken(); // eat the '['
        pi = expression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;

        /* check for "]"    */
        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (strcmp(tok.lx, "]") == 0){
            //pass
        }
        else{
            pi.er = closeBracketExpected;
            pi.tk = tok;
            return pi;
        }
    }

    /* check for "="        */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,"=") == 0){
        //pass
    }
    else {
        pi.er = equalExpected;
        pi.tk = tok;
        return pi;
    }

    /* check for expression */
    pi = expression(pi);
    /* check for errors     */
    if (pi.er != none) return pi;

    /* check for ";"    */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx, ";") == 0){
        //pass
    }
    else {
        pi.er = semicolonExpected;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo ifStatement(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for 'if' keyword   */
    if (strcmp(tok.lx, "if") == 0){
        //pass
    }
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }

    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for "("   */
    if (strcmp(tok.lx, "(") == 0){
        //pass
    }
    else{
        pi.er = openParenExpected;
        pi.tk = tok;
        return pi;
    }
    pi = expression(pi);
    /* check for errors     */
    if (pi.er != none) return pi;

    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for ")"   */
    if (strcmp(tok.lx, ")") == 0){
        //pass
    }
    else{
        pi.er = closeParenExpected;
        pi.tk = tok;
        return pi;
    }

    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for "{"   */
    if (strcmp(tok.lx, "{") == 0){
        //pass
    }
    else{
        pi.er = openBraceExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for a statement    */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "var") == 0) ||
           (strcmp(tok.lx, "while") == 0) ||
           (strcmp(tok.lx, "if") == 0) || // check for statement declarations
           (strcmp(tok.lx, "let") == 0) ||
           (strcmp(tok.lx, "do") == 0) ||
           (strcmp(tok.lx, "return") == 0)){

        pi = statement(pi);
        /* check for errors */
        if (pi.er == none){
            tok = PeekNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
        }
        else{
            return pi;
        }
    }

    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for "}"   */
    if (strcmp(tok.lx, "}") == 0){
        //pass
    }
    else{
        pi.er = closeBraceExpected;
        pi.tk = tok;
        return pi;
    }

    /* check for else statement     */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,"else") == 0){
        GetNextToken(); // eat the else

        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        /* check for "{"   */
        if (strcmp(tok.lx, "{") == 0){
            //pass
        }
        else{
            pi.er = openBraceExpected;
            pi.tk = tok;
            return pi;
        }
        /* check for a statement    */
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        while ((strcmp(tok.lx, "var") == 0) ||
               (strcmp(tok.lx, "while") == 0) ||
               (strcmp(tok.lx, "if") == 0) || // check for statement declarations
               (strcmp(tok.lx, "let") == 0) ||
               (strcmp(tok.lx, "do") == 0) ||
               (strcmp(tok.lx, "return") == 0)){

            pi = statement(pi);
            /* check for errors */
            if (pi.er == none){
                tok = PeekNextToken();
                /* check for error in token */
                if (tok.tp == ERR){
                    pi.er = lexerErr;
                    pi.tk = tok;
                    return pi;
                }
            }
            else{
                return pi;
            }
        }
        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        /* check for "}"   */
        if (strcmp(tok.lx, "}") == 0){
            //pass
        }
        else{
            pi.er = closeBraceExpected;
            pi.tk = tok;
            return pi;
        }
    }
    return pi;
}

ParserInfo whileStatement(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for 'while' keyword   */
    if (strcmp(tok.lx, "while") == 0){
        //pass
    }
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for "("   */
    if (strcmp(tok.lx, "(") == 0){
        //pass
    }
    else{
        pi.er = openParenExpected;
        pi.tk = tok;
        return pi;
    }
    pi = expression(pi);
    /* check for errors     */
    if (pi.er != none) return pi;
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for ")"   */
    if (strcmp(tok.lx, ")") == 0){
        //pass
    }
    else{
        pi.er = closeParenExpected;
        pi.tk = tok;
        return pi;
    }

    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }

    /* check for "{"   */
    if (strcmp(tok.lx, "{") == 0){
        //pass
    }
    else{
        pi.er = openBraceExpected;
        pi.tk = tok;
        return pi;
    }
    /* check for a statement    */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "var") == 0) ||
           (strcmp(tok.lx, "while") == 0) ||
           (strcmp(tok.lx, "if") == 0) || // check for statement declarations
           (strcmp(tok.lx, "let") == 0) ||
           (strcmp(tok.lx, "do") == 0) ||
           (strcmp(tok.lx, "return") == 0)){

        pi = statement(pi);
        /* check for errors */
        if (pi.er == none){
            tok = PeekNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
        }
        else{
            return pi;
        }
    }
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for "}"   */
    if (strcmp(tok.lx, "}") == 0){
        //pass
    }
    else{
        pi.er = closeBraceExpected;
        pi.tk = tok;
        return pi;
    }
    return pi;
}

ParserInfo doStatement(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for 'do' keyword */
    if (strcmp(tok.lx,"do") == 0){
        //pass
    }
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }
    pi = subroutineCall(pi);
    /* check for errors     */
    if (pi.er != none) return pi;
    /* check for semicolon  */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,";") == 0){
        //pass
    }
    else{
        pi.er = semicolonExpected;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo subroutineCall(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for identifier     */
    if (tok.tp == ID){
        //pass
    }
    else{
        pi.er = idExpected;
        pi.tk = tok;
        return pi;
    }
    /* peek for a "."   */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,".") == 0){
        GetNextToken();// eat the "."
        /* check for identifier     */
        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (tok.tp == ID){
            //pass
        }
        else{
            pi.er = idExpected;
            pi.tk = tok;
            return pi;
        }
    }

    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for "("   */
    if (strcmp(tok.lx, "(") == 0){
        //pass
    }
    else{
        pi.er = openParenExpected;
        pi.tk = tok;
        return pi;
    }
    pi = expressionList(pi);
    /* check for errors     */
    if (pi.er != none) return pi;
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    /* check for ")"   */
    if (strcmp(tok.lx, ")") == 0){
        //pass
    }
    else{
        pi.er = closeParenExpected;
        pi.tk = tok;
        return pi;
    }
    return pi;
}

ParserInfo expressionList(ParserInfo pi){
    /* check for an expression      */
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "-") == 0) ||
        (strcmp(tok.lx, "~") == 0) ||
        (strcmp(tok.lx, "(") == 0) ||
        (strcmp(tok.lx, "true") == 0) ||
        (strcmp(tok.lx, "false") == 0) ||
        (strcmp(tok.lx, "null") == 0) ||
        (strcmp(tok.lx, "this") == 0) ||
        (tok.tp == ID) ||
        (tok.tp == STRING) ||
        (tok.tp == INT)){
        pi = expression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;

        /* check for further expressions    */
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        while (strcmp(tok.lx, ",") == 0){
            GetNextToken(); //eat the ","
            pi = expression(pi);
            /* check for errors     */
            if (pi.er != none) return pi;
            /* look for another "," */
            tok = PeekNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
        }
    }
    return pi;
}

ParserInfo returnStatement(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,"return") == 0){
        //pass
    }
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }

    /* check for an expression          */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "-") == 0) ||
        (strcmp(tok.lx, "~") == 0) ||
        (strcmp(tok.lx, "(") == 0) ||
        (strcmp(tok.lx, "true") == 0) ||
        (strcmp(tok.lx, "false") == 0) ||
        (strcmp(tok.lx, "null") == 0) ||
        (strcmp(tok.lx, "this") == 0) ||
        (tok.tp == ID) ||
        (tok.tp == STRING) ||
        (tok.tp == INT)) {
        pi = expression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
    }

    /* check for ";"        */
    tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if (strcmp(tok.lx,";") == 0){
        //pass
    }
    else {
        pi.er = semicolonExpected;
        pi.tk = tok;
    }
    return pi;
}

ParserInfo expression(ParserInfo pi){
    /* check for a relational expression      */
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "-") == 0) ||
        (strcmp(tok.lx, "~") == 0) ||
        (strcmp(tok.lx, "(") == 0) ||
        (strcmp(tok.lx, "true") == 0) ||
        (strcmp(tok.lx, "false") == 0) ||
        (strcmp(tok.lx, "null") == 0) ||
        (strcmp(tok.lx, "this") == 0) ||
        (tok.tp == ID) ||
        (tok.tp == STRING) ||
        (tok.tp == INT)) {
        pi = relationalExpression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
    }
    /* not an expression    */
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }

    /* look for more relational expressions */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "&") == 0) ||
           (strcmp(tok.lx, "|") == 0)){
        GetNextToken(); // eat char
        pi = relationalExpression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
    }
    return pi;
}

ParserInfo relationalExpression(ParserInfo pi){
    /* check for an arithmetic expression      */
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "-") == 0) ||
        (strcmp(tok.lx, "~") == 0) ||
        (strcmp(tok.lx, "(") == 0) ||
        (strcmp(tok.lx, "true") == 0) ||
        (strcmp(tok.lx, "false") == 0) ||
        (strcmp(tok.lx, "null") == 0) ||
        (strcmp(tok.lx, "this") == 0) ||
        (tok.tp == ID) ||
        (tok.tp == STRING) ||
        (tok.tp == INT)) {
        pi = arithmeticExpression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
    }
        /* not a relational expression    */
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }

    /* look for more arithmetic expressions */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "=") == 0) ||
           (strcmp(tok.lx, "<") == 0) ||
           (strcmp(tok.lx, ">") == 0)){
        GetNextToken(); // eat the char
        pi = arithmeticExpression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
    }
    return pi;
}

ParserInfo arithmeticExpression(ParserInfo pi){
    /* check for a term       */
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "-") == 0) ||
        (strcmp(tok.lx, "~") == 0) ||
        (strcmp(tok.lx, "(") == 0) ||
        (strcmp(tok.lx, "true") == 0) ||
        (strcmp(tok.lx, "false") == 0) ||
        (strcmp(tok.lx, "null") == 0) ||
        (strcmp(tok.lx, "this") == 0) ||
        (tok.tp == ID) ||
        (tok.tp == STRING) ||
        (tok.tp == INT)) {
        pi = term(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
    }
    /* not an arithmetic expression    */
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }

    /* look for more terms */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "+") == 0) ||
           (strcmp(tok.lx, "-") == 0)){
        GetNextToken();
        pi = term(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
    }
    return pi;
}

ParserInfo term(ParserInfo pi){
    /* check for a factor      */
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "-") == 0) ||
        (strcmp(tok.lx, "~") == 0) ||
        (strcmp(tok.lx, "(") == 0) ||
        (strcmp(tok.lx, "true") == 0) ||
        (strcmp(tok.lx, "false") == 0) ||
        (strcmp(tok.lx, "null") == 0) ||
        (strcmp(tok.lx, "this") == 0) ||
        (tok.tp == ID) ||
        (tok.tp == STRING) ||
        (tok.tp == INT)) {
        pi = factor(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
    }
    /* not a term    */
    else{
        pi.er = syntaxError;
        pi.tk = tok;
        return pi;
    }

    /* look for more factors */
    tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    while ((strcmp(tok.lx, "*") == 0) ||
           (strcmp(tok.lx, "/") == 0)){
        GetNextToken();
        pi = factor(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
    }
    return pi;
}

ParserInfo factor(ParserInfo pi){
    /* check for "-" or "~" */
    Token tok = PeekNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "-") == 0) ||
        (strcmp(tok.lx, "~") == 0)){
        GetNextToken(); // eat the char
    }
    pi = operand(pi);
    /* check for errors     */
    if (pi.er != none) return pi;
    return pi;
}

ParserInfo operand(ParserInfo pi){
    Token tok = GetNextToken();
    /* check for error in token */
    if (tok.tp == ERR){
        pi.er = lexerErr;
        pi.tk = tok;
        return pi;
    }
    if ((strcmp(tok.lx, "true") == 0) ||
        (strcmp(tok.lx, "false") == 0) ||
        (strcmp(tok.lx, "null") == 0) ||
        (strcmp(tok.lx, "this") == 0) ||
        (tok.tp == STRING) ||
        (tok.tp == INT)){
        //pass
    }

    /* check for '(expression)' */
    else if (strcmp(tok.lx, "(") == 0){
        pi = expression(pi);
        /* check for errors     */
        if (pi.er != none) return pi;
        /* check for ")"    */
        tok = GetNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (strcmp(tok.lx, ")") == 0){
            //pass
        }
        else{
            pi.er = closeParenExpected;
            pi.tk = tok;
            return pi;
        }
    }

    /* check for identifier     */
    else if (tok.tp == ID){
        /* peek for a "."   */
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (strcmp(tok.lx,".") == 0){
            GetNextToken(); // eat the "."
            /* check for identifier     */
            tok = GetNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
            if (tok.tp == ID){
                //pass
            }
            else{
                pi.er = idExpected;
                pi.tk = tok;
                return pi;
            }
        }

        /* peek for a "[" or "("        */
        tok = PeekNextToken();
        /* check for error in token */
        if (tok.tp == ERR){
            pi.er = lexerErr;
            pi.tk = tok;
            return pi;
        }
        if (strcmp(tok.lx, "[") == 0){
            GetNextToken(); // eat the "["
            pi = expression(pi);
            /* check for errors     */
            if (pi.er != none) return pi;
            /* check for "]"        */
            tok = GetNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
            if (strcmp(tok.lx, "]") == 0){
                //pass
            }
            else{
                pi.er = closeBracketExpected;
                pi.tk = tok;
                return pi;
            }
        }
        else if (strcmp(tok.lx, "(") == 0){
            GetNextToken(); // eat the "("
            pi = expressionList(pi);
            /* check for errors     */
            if (pi.er != none) return pi;
            /* check for ")"        */
            tok = GetNextToken();
            /* check for error in token */
            if (tok.tp == ERR){
                pi.er = lexerErr;
                pi.tk = tok;
                return pi;
            }
            if (strcmp(tok.lx, ")") == 0){
                //pass
            }
            else{
                pi.er = closeParenExpected;
                pi.tk = tok;
                return pi;
            }
        }
    }

    /* not an operand       */
    else{
        pi.er = syntaxError;
        pi.tk = tok;
    }
    return pi;
}






int InitParser (char* file_name){
    /* initialise lexer         */
    InitLexer(file_name);
	return 1;
}

ParserInfo Parse (){
	ParserInfo pi;
	pi.er = none;
    /* start parsing at the start symbol        */
    pi = classDeclar(pi);
	return pi;
}


int StopParser (){
    StopLexer();
	return 1;
}

#ifndef TEST_PARSER
int main (int argc, char* argv[]){
    if (argc == 2) {
        InitParser(argv[1]);
        Parse();
        StopParser();
    }
	return 1;
}
#endif

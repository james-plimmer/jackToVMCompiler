all:	lexer

clean:
	rm lexer *.o

lexer:	lexer.o
	gcc lexer.o -o lexer

lexer.o:	lexer.c
	gcc -c lexer.c -o lexer.o

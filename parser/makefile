CC=gcc

CCFLAGS=-g -std=c99 -Wall

TARGETS=parser

MAINS=parser.o lexer.o


all:	parser

clean:
	rm ${TARGETS} ${MAINS}

#targets 	dependencies
parser:	parser.o lexer.o

	${CC}	${CCFLAGS}	-o parser	parser.c lexer.c

#mains		implementation	interfaces
parser.o:	parser.c	parser.h lexer.h

	${CC}	  -c ${CCFLAGS}	    -o parser.o    parser.c

lexer.o:      lexer.c		lexer.h

	${CC}     -c ${CCFLAGS}     -o lexer.o     lexer.c
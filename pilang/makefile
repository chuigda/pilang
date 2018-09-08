all: docs pli

docs: doc/makefile
	cd doc && make && tree -P "*.html" > docs && cd ..

pli : mstring.o lexer.o parser.o clist.o ast.o
	$(CC) mstring.o parser.o clist.o ast.o lexer.o -o pli

ast.o : ast.c jjvalue.h
	$(CC) -c ast.c -o ast.o -g

clist.o : clist.c clist.h
	$(CC) -c clist.c -o clist.o -g

mstring.o : mstring.c mstring.h
	$(CC) -c mstring.c -o mstring.o -g

parser.o : y.tab.c mstring.h jjvalue.h
	$(CC) -c y.tab.c -o parser.o -g

lexer.o : lexer.c y.tab.h
	$(CC) -c lexer.c -o lexer.o -g

y.tab.c y.tab.h : parser.y
	yacc -d -v parser.y

clean:
	rm -rf y.tab.*
	rm -rf y.output
	rm -rf *.o
	rm -rf pli
	cd doc && make clean && cd ..
	rm -rf doc/docs

all: docs pli

docs: doc/makefile
	cd doc && make && tree -P "*.html" > docs && cd ..

pli : mstring.o lexer.o parser.o clist.o ast.o tree_dump.o
	$(CC) mstring.o parser.o clist.o ast.o lexer.o tree_dump.o -o pli

tree_dump.o: tree_dump.c tree_dump.h ast.h mstring.h
	$(CC) -c tree_dump.c -o tree_dump.o -g

ast.o : ast.c jjvalue.h mstring.h util.h
	$(CC) -c ast.c -o ast.o -g

clist.o : clist.c clist.h
	$(CC) -c clist.c -o clist.o -g

mstring.o : mstring.c mstring.h util.h
	$(CC) -c mstring.c -o mstring.o -g

parser.o : y.tab.c mstring.h jjvalue.h tree_dump.h
	$(CC) -c y.tab.c -o parser.o -g

lexer.o : lexer.c y.tab.h util.h
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

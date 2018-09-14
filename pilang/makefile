all: docs pli tests

docs: doc/makefile
	cd doc && make && tree -P "*.html" > docs && cd ..

pli: mstring.o lexer.o parser.o clist.o ast.o tree_dump.o plheap.o \
     stack.o
	$(CC) mstring.o parser.o clist.o ast.o lexer.o tree_dump.o plheap.o \
	      stack.o -o pli

tests: test_stack test_gc

test_gc: test_gc.o plheap.o clist.o mstring.o
	$(CC) test_gc.o plheap.o clist.o mstring.o -o test_gc

test_gc.o: test_gc.c plheap.h mstring.h
	$(CC) -c test_gc.c -o test_gc.o -g

test_stack: stack.o test_stack.o clist.o mstring.o
	$(CC) stack.o test_stack.o clist.o mstring.o -o test_stack

test_stack.o: test_stack.c stack.h
	$(CC) -c test_stack.c -o test_stack.o -g

stack.o: stack.c stack.h clist.h mstring.h util.h
	$(CC) -c stack.c -o stack.o -g

plheap.o: plheap.c plheap.h clist.h util.h
	$(CC) -c plheap.c -o plheap.o -g

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
	rm -rf test_stack
	cd doc && make clean && cd ..
	rm -rf doc/docs

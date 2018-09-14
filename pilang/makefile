all: docs pli plast tests

docs: doc/makefile
	cd doc && make && tree -P "*.html" > docs && cd ..

pli: mstring.o lexer.o parser.o clist.o ast.o eval.o stack.o plheap.o \
     pli.o
	$(CC) mstring.o lexer.o parser.o clist.o ast.o eval.o stack.o plheap.o \
        pli.o -o pli

plast: mstring.o lexer.o parser.o clist.o ast.o tree_dump.o plast.o
	$(CC) mstring.o parser.o clist.o ast.o lexer.o tree_dump.o plast.o \
	      -o plast

tests: test_stack test_gc

test_gc: test_gc.o plheap.o clist.o mstring.o
	$(CC) test_gc.o plheap.o clist.o mstring.o -o test_gc

test_gc.o: test/test_gc.c
	$(CC) -c -I include test/test_gc.c -o test_gc.o -g

test_stack: stack.o test_stack.o clist.o mstring.o
	$(CC) stack.o test_stack.o clist.o mstring.o -o test_stack

test_stack.o: test/test_stack.c
	$(CC) -c -I include test/test_stack.c -o test_stack.o -g

plast.o: src/driver/plast.c y.tab.h
	$(CC) -c -I include -I ./ src/driver/plast.c -o plast.o -g

pli.o: src/driver/pli.c y.tab.h
	$(CC) -c -I include -I ./ src/driver/pli.c -o pli.o -g

eval.o: src/eval/eval.c y.tab.h
	$(CC) -c -I include -I ./ src/eval/eval.c -o eval.o -g

stack.o: src/eval/stack.c
	$(CC) -c -I include src/eval/stack.c -o stack.o -g

plheap.o: src/eval/plheap.c
	$(CC) -c -I include src/eval/plheap.c -o plheap.o -g

tree_dump.o: src/ast/tree_dump.c
	$(CC) -c -I include src/ast/tree_dump.c -o tree_dump.o -g

ast.o : src/ast/ast.c
	$(CC) -c -I include src/ast/ast.c -o ast.o -g

clist.o : src/support/clist.c
	$(CC) -c -I include src/support/clist.c -o clist.o -g

mstring.o : src/support/mstring.c
	$(CC) -c -I include src/support/mstring.c -o mstring.o -g

parser.o : y.tab.c
	$(CC) -c -I include y.tab.c -o parser.o -g

lexer.o : src/frontend/lexer.c y.tab.h
	$(CC) -c -I include src/frontend/lexer.c -I ./ -o lexer.o -g

y.tab.c y.tab.h : src/frontend/parser.y
	yacc -d -v src/frontend/parser.y

clean:
	rm -rf y.tab.*
	rm -rf y.output
	rm -rf *.o
	rm -rf pli plast
	rm -rf test_stack test_gc
	cd doc && make clean && cd ..
	rm -rf doc/docs

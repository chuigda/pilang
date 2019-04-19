CFLAGS = -fPIC -Wall -Wextra -Wno-unused-function -g

all: docs bins scripts libs stdlib tests

bins: plrepl.bin pli.bin plast.bin

scripts: pli plast plrepl

stdlib: libmstr.so

libs: libpilang.so libfront.so libsup.so

docs: doc/makefile
	cd doc && make && tree -P "*.html" > docs && cd ..

plrepl: plrepl.script.in
	cp plrepl.script.in plrepl
	chmod 755 plrepl

pli: pli.script.in
	cp pli.script.in pli
	chmod 755 pli
	
plast: plast.script.in
	cp plast.script.in plast
	chmod 755 plast

plrepl.bin: plrepl.o libpilang.so libfront.so libsup.so
	$(CC) builtins.o plrepl.o \
              dynload.o -L. -lpilang -lfront -lsup -ldl -o plrepl.bin

pli.bin: pli.o libpilang.so libfront.so libsup.so
	$(CC) pli.o builtins.o dynload.o \
              -L. -lpilang -lfront -lsup -ldl -o pli.bin

plast.bin: plast.o libfront.so libsup.so
	$(CC) plast.o -L. -lfront -lsup -ldl -o plast.bin

tests: test_stack test_gc test_calc test_assign test_eval_expr \
       libffiex.so libffiex2.so

libmstr.so: mutstr.o libpilang.so libsup.so
	$(CC) mutstr.o -L. -lpilang -lsup -shared -o libmstr.so

mutstr.o: stdlib/mutstr.c
	$(CC) stdlib/mutstr.c -I include -I ./ -c -o mutstr.o \
              $(CFLAGS)

libffiex2.so: ffiex2.o libpilang.so libsup.so
	$(CC) ffiex2.o -L. -lpilang -lsup -shared -o libffiex2.so

libffiex.so: ffiex.o libpilang.so libsup.so
	$(CC) ffiex.o -L. -lpilang -lsup -shared -o libffiex.so

libfront.so: lexer.o parser.o ast.o tree_dump.o
	$(CC) lexer.o parser.o ast.o tree_dump.o -shared -o libfront.so

libpilang.so: value.o stack.o plheap.o eval.o builtins.o
	$(CC) value.o stack.o plheap.o eval.o builtins.o \
              -shared -o libpilang.so

libsup.so: clist.o mstring.o dynload.o
	$(CC) clist.o mstring.o dynload.o -shared -o libsup.so

test_eval_expr: test_eval_expr.o libpilang.so libfront.so libsup.so
	$(CC) test_eval_expr.o \
              -L. -lpilang -lfront -lsup -ldl -o test_eval_expr

test_assign: test_assign.o libpilang.so libsup.so
	$(CC) test_assign.o -L. -lpilang -lsup -ldl -o test_assign

test_calc: test_calc.o libpilang.so libsup.so
	$(CC) test_calc.o -L. -lpilang -lsup -ldl -o test_calc

test_gc: test_gc.o plheap.o libsup.so
	$(CC) test_gc.o plheap.o -L. -lsup -ldl -o test_gc

test_stack: stack.o test_stack.o
	$(CC) stack.o test_stack.o -L. -lsup -ldl -o test_stack

test_eval_expr.o: test/test_eval_expr.c y.tab.h
	$(CC) -c -I include -I . test/test_eval_expr.c -o \
        test_eval_expr.o $(CFLAGS)

test_assign.o: test/test_assign.c
	$(CC) -c -I include test/test_assign.c \
              -o test_assign.o $(CFLAGS)

test_calc.o: test/test_calc.c
	$(CC) -c -I include test/test_calc.c -o test_calc.o $(CFLAGS)

test_gc.o: test/test_gc.c
	$(CC) -c -I include test/test_gc.c -o test_gc.o $(CFLAGS)

test_stack.o: test/test_stack.c
	$(CC) -c -I include test/test_stack.c -o test_stack.o $(CFLAGS)

ffiex2.o: sample/ffiex2.c
	$(CC) -c -I include -I ./ sample/ffiex2.c -o ffiex2.o \
              $(CFLAGS)

ffiex.o: sample/ffiex.c
	$(CC) -c -I include -I ./ sample/ffiex.c -o ffiex.o \
              $(CFLAGS)

plast.o: src/driver/plast.c y.tab.h
	$(CC) -c -I include -I ./ src/driver/plast.c \
              -o plast.o $(CFLAGS)

pli.o: src/driver/pli.c y.tab.h
	$(CC) -c -I include -I ./ src/driver/pli.c -o pli.o $(CFLAGS)

plrepl.o: src/driver/plrepl.c y.tab.h
	$(CC) -c -I include -I ./ src/driver/plrepl.c -o plrepl.o \
        $(CFLAGS)

dynload.o: src/support/dynload_posix.c y.tab.h
	$(CC) -c -I include -I ./ src/support/dynload_posix.c \
        -o dynload.o $(CFLAGS)

builtins.o: src/eval/builtins.c y.tab.h
	$(CC) -c -I include -I ./ src/eval/builtins.c \
        -o builtins.o $(CFLAGS)

value.o: src/eval/value.c y.tab.h
	$(CC) -c -I include -I ./ src/eval/value.c \
        -o value.o $(CFLAGS)

eval.o: src/eval/eval.c y.tab.h
	$(CC) -c -I include -I ./ src/eval/eval.c \
        -o eval.o $(CFLAGS)

stack.o: src/eval/stack.c
	$(CC) -c -I include src/eval/stack.c -o stack.o $(CFLAGS)

plheap.o: src/eval/heap.c
	$(CC) -c -I include src/eval/heap.c -o plheap.o $(CFLAGS)

tree_dump.o: src/ast/tree_dump.c y.tab.h
	$(CC) -c -I include -I ./ src/ast/tree_dump.c \
        -o tree_dump.o $(CFLAGS)

ast.o : src/ast/ast.c
	$(CC) -c -I include src/ast/ast.c -o ast.o $(CFLAGS)

clist.o : src/support/clist.c
	$(CC) -c -I include src/support/clist.c \
              -o clist.o $(CFLAGS)

mstring.o : src/support/mstring.c
	$(CC) -c -I include src/support/mstring.c \
              -o mstring.o $(CFLAGS)

parser.o : y.tab.c
	$(CC) -c -I include y.tab.c -o parser.o $(CFLAGS)

lexer.o : src/frontend/lexer.c y.tab.h
	$(CC) -c -I include src/frontend/lexer.c -I ./ \
              -o lexer.o $(CFLAGS)

y.tab.c y.tab.h : src/frontend/parser.y
	yacc -d -v src/frontend/parser.y

clean:
	rm -rf y.tab.*
	rm -rf y.output
	rm -rf *.o
	rm -rf pli plast plrepl
	rm -rf *.bin
	rm -rf test_*
	cd doc && make clean && cd ..
	rm -rf doc/docs
	rm -rf *.so
	rm -rf *.a

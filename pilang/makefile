plc : mstring.o parser.o clist.o ast.o
	$(CC) mstring.o parser.o clist.o ast.o -o plc

ast.o : ast.c jjvalue.h
	$(CC) -c ast.c -o ast.o -O2

clist.o : clist.c clist.h
	$(CC) -c clist.c -o clist.o -O2

mstring.o : mstring.c mstring.h
	$(CC) -c mstring.c -o mstring.o -O2

parser.o : y.tab.c mstring.h jjvalue.h
	$(CC) -c y.tab.c -o parser.o -O2

y.tab.c : parser.y
	yacc parser.y

clean:
	rm -rf y.tab.c
	rm -rf *.o
	rm -rf plc
plc : mstring.o parser.o
	$(CC) mstring.o parser.o -o plc

mstring.o : mstring.c mstring.h
	$(CC) -c mstring.c -o mstring.o -O2

parser.o : y.tab.c mstring.h
	$(CC) -c y.tab.c -o parser.o -O2

y.tab.c : parser.y
	yacc parser.y

clean: 
	rm -rf y.tab.c
	rm -rf *.o
	rm -rf plc
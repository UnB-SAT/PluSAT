
run: 
	./parser.out

build: 
	gcc -fsanitize=address -I. -I../formula/ test_parser.c parser.c ../formula/formula.c -o parser.out

clean:
	rm parser.out

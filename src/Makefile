CC=gcc -fsanitize=address

makebuilddir:
	@echo "Create build directory"
	mkdir -p build/

formula: makebuilddir
	@echo "Building formula module"
	$(CC) -c -o build/formula.o formula/formula.c -I formula/

parser: makebuilddir 
	@echo "Building parser module"
	$(CC) -c -o build/parser.o parser/parser.c -I formula/ -I parser/

build: formula parser
	@echo "Bulding main"
	$(CC) main.c build/parser.o build/formula.o -I formula/ -I parser -o build/main.o

clean: 
	rm -rf build/

run: build
	./build/main.o ../inputs/test2.cnf

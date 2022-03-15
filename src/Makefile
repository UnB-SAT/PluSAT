CC=gcc
CFLAGS = -fsanitize=address
INCLUDES = -I formula/ -I parser/ -I dpll/ -I level/

makebuilddir:
	@echo "Create build directory"
	mkdir -p build/

formula: makebuilddir
	@echo "Building formula module"
	$(CC) $(CFLAGS) -c -o build/formula.o formula/formula.c $(INCLUDES)

parser: makebuilddir
	@echo "Building parser module"
	$(CC) $(CFLAGS) -c -o build/parser.o parser/parser.c $(INCLUDES)

dpll: makebuilddir
	@echo "Bulding dpll module"
	$(CC) $(CFLAGS) -c -o build/dpll.o dpll/dpll.c $(INCLUDES)

build: formula parser dpll
	@echo "Bulding main"
	$(CC) $(CFLAGS) main.c build/parser.o build/formula.o  build/dpll.o $(INCLUDES) -o build/main.o

clean: 
	rm -rf build/

run: build
	./build/main.o ../inputs/test2.cnf

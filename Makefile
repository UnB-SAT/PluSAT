CC=gcc
CFLAGS = -fPIC -O2
INCLUDES = -I src/

makebuilddir:
	@echo "Create build directory"
	mkdir -p build/

build: makebuilddir
	@echo "Bulding main"
	$(CC) $(CFLAGS) -export-dynamic src/main.c src/parser.c src/dpll.c src/formula.c src/discovery.c $(INCLUDES) -o build/main.o

clean: 
	rm -rf build/

plugin: makebuilddir
	$(CC) $(CFLAGS) -shared -o build/libimplement.so src/clause_learning.c $(INCLUDES)

run: build
	./build/main.o ~/Downloads/cnf/uf20-0199.cnf

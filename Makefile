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
	$(CC) $(CFLAGS) -shared -o build/libimplement.so src/plugin/simple.c $(INCLUDES)

sat: build
	./build/main.o ./uf20-91/uf20-0999.cnf
	#./build/main.o ~/Downloads/cnf/cnf50-218/uuf50-063.cnf

unsat: build
	./build/main.o ./uuf50-218/uuf50-0999.cnf
	# ./build/main.o ~/Downloads/cnf/uf20-91/uf20-0999.cnf

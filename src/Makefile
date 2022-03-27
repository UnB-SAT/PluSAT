CC=gcc
CFLAGS = -fsanitize=address -fPIC -ldl
INCLUDES = -I formula/ -I parser/ -I dpll/ -I level/ -I discovery/

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

discovery: makebuilddir
	@echo "Bulding discovery module"
	$(CC) $(CFLAGS) -c -o build/discovery.o discovery/discovery.c $(INCLUDES)

build: formula parser dpll discovery
	@echo "Bulding main"
	$(CC) $(CFLAGS) -Wl,--export-dynamic main.c build/parser.o build/formula.o  build/dpll.o build/discovery.o $(INCLUDES) -o build/main.o

clean: 
	rm -rf build/

plugin: makebuilddir
	$(CC) $(CFLAGS) -shared -o build/libimplement.so plugin/implement.c $(INCLUDES)
run: build
	./build/main.o ../inputs/test2.cnf

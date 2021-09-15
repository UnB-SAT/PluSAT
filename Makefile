
all: build run

build: 
	gcc src/main.c src/formula.c src/parser.c -Isrc -o solver

run:
	./solver


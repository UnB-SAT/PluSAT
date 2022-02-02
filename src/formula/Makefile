TEST_TARGET=formula.out

build: 
	gcc test_formula.c -fsanitize=address -I. formula.c -o $(TEST_TARGET)


run: build
	./$(TEST_TARGET)

clean:
	@rm formula.out

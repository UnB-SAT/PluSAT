TEST_TARGET=formula.out

compiletest: 
	gcc test_formula.c -I. formula.c -o $(TEST_TARGET)

compiletests: 
	gcc test_formula.c -fsanitize=address -I. formula.c -o $(TEST_TARGET)


runtest: compiletest
	./$(TEST_TARGET) && rm $(TEST_TARGET)

runtests: compiletests
	./$(TEST_TARGET) && rm $(TEST_TARGET)


clean:
	@rm formula.out

TEST_TARGET=formula.out

compiletest: 
	@gcc test_formula.c -I. formula.c -o $(TEST_TARGET)

runtest: compiletest
	./$(TEST_TARGET) && rm $(TEST_TARGET)

clean:
	@rm formula.out

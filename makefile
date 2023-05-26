build:
	clang forth.c optable.c stack.c -o ./forth -Wall
run:
	./forth
run-rlwrap:
	rlwrap -r -f rlwrapcompletions.forth ./forth


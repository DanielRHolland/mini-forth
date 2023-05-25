build:
	gcc forth.c optable.c stack.c -o forth
run:
	./forth
run-rlwrap:
	rlwrap -r -f rlwrapcompletions.forth ./forth


build:
	clang forth.c optable.c stack.c -o ./forth -Wall
run:
	./forth
run-rlwrap:
	rlwrap -r -f rlwrapcompletions.forth ./forth
build-wasm:
	emcc forth.c optable.c stack.c -Wall -o forthlib.js -s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"

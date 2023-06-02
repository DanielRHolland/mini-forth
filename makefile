srcfiles := drhstrings.c forthmachine.c forthmachine_optable.c forth.c stack.c

nativeexe := ./forth

ifneq (, $(shell which clang))
	CC := clang
endif

build:
	${CC} ${srcfiles} -o ${nativeexe} -Wall
run:
	${nativeexe}
run-rlwrap:
	rlwrap -r -f rlwrapcompletions.forth ${nativeexe}
build-wasm:
	emcc ${srcfiles} -Wall -o forthlib.js -s NO_EXIT_RUNTIME=1 -s "EXPORTED_RUNTIME_METHODS=['ccall']"

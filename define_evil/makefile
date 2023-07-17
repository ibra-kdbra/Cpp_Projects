tests: tests.c define_EVIL.h generated
	gcc -std=c11 -Wall -Werror -o tests_bin tests.c

test: tests
	./tests_bin

dump: tests.c define_EVIL.h generated
	gcc -std=c11 -E tests.c

generated: generator.sh
	./generator.sh > generated_EVIL.h

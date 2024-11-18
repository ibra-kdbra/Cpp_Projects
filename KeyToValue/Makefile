all:
	$(CC) -fPIC -shared -fstack-protector-all -fstack-clash-protection   -D_FORTIFY_SOURCE=3 -Wno-unused-result -O2 -std=gnu99 -Wno-gnu-zero-variadic-macro-arguments -o libk2v.so src/k2v.c -z noexecstack -z now
	strip libk2v.so
	$(CC) -std=gnu99 -c -o libk2v.o src/k2v.c
	ar -r libk2v.a libk2v.o
	rm libk2v.o
format:
	clang-format -i src/*.c
	clang-format -i src/include/*.h
	clang-format -i test/*.c
dev:
	$(CC) -std=gnu99 -fPIC -shared -ggdb -O0 -fno-omit-frame-pointer -z norelro -z execstack -Wno-gnu-zero-variadic-macro-arguments -fno-stack-protector -Wall -Wextra -pedantic -Wconversion -Wno-newline-eof -o libk2v.so src/k2v.c
	$(CC) -std=gnu99 -ggdb -O0 -fno-omit-frame-pointer -Wno-gnu-zero-variadic-macro-arguments -fno-stack-protector -Wall -Wextra -pedantic -Wconversion -Wno-newline-eof -c -o libk2v.o src/k2v.c
	ar -r libk2v.a libk2v.o
	rm libk2v.o
test:dev
	$(CC) -ggdb -fsanitize=address -O0 -fno-omit-frame-pointer -z norelro -z execstack -fno-stack-protector -Wall -Wextra -pedantic -Wconversion -Wno-newline-eof -Wl,--gc-sections -o testk2v test/test.c src/k2v.c
check:
	clang-tidy --checks=*,-clang-analyzer-security.insecureAPI.strcpy,-altera-unroll-loops,-cert-err33-c,-concurrency-mt-unsafe,-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling,-readability-function-cognitive-complexity,-cppcoreguidelines-avoid-magic-numbers,-readability-magic-numbers,-bugprone-easily-swappable-parameters,-cert-err34-c,-misc-include-cleaner,-readability-identifier-length,-bugprone-signal-handler,-cert-msc54-cpp,-cert-sig30-c,-altera-id-dependent-backward-branch,-cppcoreguidelines-avoid-non-const-global-variables src/k2v.c --
clean:
	rm k2sh *.so *.a
	rm testk2v||true

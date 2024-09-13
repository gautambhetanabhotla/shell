objects := $(patsubst src/%.c,build/%.o,$(wildcard src/*.c))
test-objects := $(patsubst src/%.c,test-build/%.o,$(wildcard src/*.c))

default: build a.out

build:
	@mkdir build

a.out: $(objects) main.c
	@gcc -g $^

build/%.o: src/%.c
	@gcc -c -g -o $@ $<

test-strict: test.c src/*.c
	@gcc -g -o test-strict -DDEBUG -fsanitize=address test.c src/*.c

test: test.c test-build $(test-objects)
	@gcc -g -o test -DDEBUG test.c test-build/*.o

test-build:
	@mkdir test-build

test-build/%.o: src/%.c
	@gcc -DDEBUG -c -g -o $@ $<

clean:
	@rm -rf build a.out test test-strict test-build
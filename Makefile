objects := $(patsubst src/%.c,build/%.o,$(wildcard src/*.c))

default: build a.out

build:
	@mkdir build

a.out: $(objects) main.c
	@gcc -g $^

build/%.o: src/%.c
	@gcc -c -g -o $@ $<

test: test.c src/*.c
	@gcc -g -o test -DDEBUG test.c src/*.c

clean:
	@rm -rf build a.out
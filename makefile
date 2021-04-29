.PHONY: cmake build run docs test all clean
.DEFAULT_GOAL := all

cmake:
	@mkdir -p ./build && cd ./build && cmake -G Ninja ../

build:
	@cmake --build ./build/

run:
	@./build/src/parser-combinator

test:
	@./build/test/tests

docs:
	@doxygen ./Doxyfile

clean:
	@rm -rf build
	@rm -rf docs

all: clean docs cmake build run
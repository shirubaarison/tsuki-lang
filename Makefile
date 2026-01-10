.PHONY: all build clean run

all: build run

build:
	cmake -B build
	cmake --build ./build

clean:
	rm -rf build

run:
	./build/bin/TsukiLang --debug

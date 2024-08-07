.PHONY: all run build

all: build run

run:
	./build/wc -${FLAGS} ${INPUT_FILES}

build:
	g++ src/main.cpp -g -std=c++17 -fsanitize=address -fstack-protector -Wall -Wextra -Wpedantic -Werror -o build/wc

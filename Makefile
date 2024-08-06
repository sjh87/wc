.PHONY: all run build

all: build run

run:
	./build/wc -c ${INPUT_FILE}

build:
	g++ src/main.cpp -std=c++17 -fsanitize=address -fstack-protector -Wall -Wextra -Wpedantic -Werror -o build/wc

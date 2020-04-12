all: bin/main.exe
	gcc -o bin/main src/main.c -Iinclude -Llib -lmingw32 -lSDL2main -lSDL2

run:
	bin/main.exe
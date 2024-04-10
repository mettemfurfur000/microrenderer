# CFLAGS += -IC:/msys64/mingw64/include/SDL2 -Dmain=SDL_main -lmingw32 -lws2_32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm

all: clean vec
	gcc -c config_reader.c -o obj/config_reader.o -g -Wall
	gcc obj/vec.o obj/config_reader.o -o app -g -Wall

.PHONY: clean
clean:
	rm -f obj/*
	rm -f app

.PHONY: vec
vec:
	gcc -o obj/vec.o -c vec/src/vec.c -Ivec/src -g


#	gcc main.c -o main -lSDL2 -Wall -lSDL2_image -lSDL2_ttf
#	./main

CFLAGS += -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm

all: clean vec
	gcc -c main.c -o obj/main.o -g -Wall 
	gcc obj/vec.o obj/main.o -o app -g -Wall $(CFLAGS)

.PHONY: clean
clean:
	rm -f obj/*
	rm -f app

.PHONY: vec
vec:
	gcc -o obj/vec.o -c vec/src/vec.c -Ivec/src -g


#	gcc main.c -o main -lSDL2 -Wall -lSDL2_image -lSDL2_ttf
#	./main

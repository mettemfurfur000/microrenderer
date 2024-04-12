CFLAGS += -g -Wall
LDFLAGS += -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm

sources := $(shell cd src;echo *.c)
objects := $(patsubst %.c,obj/%.o,$(sources))
headers := $(shell cd src/headers;echo *.h)
executable := app

all: vec $(executable) 

# src/headers/%.h

obj/%.o : src/%.c
	gcc $(CFLAGS) -c $^ -o $@

.PHONY: vec
vec:
	gcc -o obj/vec.o -c vec/src/vec.c -Ivec/src -g

obj/main.o: src/main.c
	gcc $(CFLAGS) -c $^ -o $@

$(executable): $(objects)
	gcc $(CFLAGS) -o build/$@ $^ $(LDFLAGS)
	cp -r example_files/* build
	(cd build;./app test.cfg)

clean:
	rm -rf obj/*
	rm -rf build/*

#	gcc main.c -o main -lSDL2 -Wall -lSDL2_image -lSDL2_ttf
#	./main

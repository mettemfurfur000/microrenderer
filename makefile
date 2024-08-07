CFLAGS += -O0 -Wall -g
LDFLAGS += -lm

ifeq ($(OS),Windows_NT)
        CFLAGS += -IC:/msys64/mingw64/include/SDL2 -Dmain=SDL_main
        LDFLAGS += -LC:/msys64/mingw64/lib -lmingw32 -lws2_32      
endif

LDFLAGS += -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf

sources := $(shell cd src;echo *.c)
objects := $(patsubst %.c,obj/%.o,$(sources))
headers := $(shell cd src/headers;echo *.h)
executable := app

all: make_folders $(executable)

# src/headers/%.h

obj/%.o : src/%.c
	gcc $(CFLAGS) -c $^ -o $@

obj/main.o: src/main.c
	gcc $(CFLAGS) -c $^ -o $@

$(executable): $(objects)
	gcc $(CFLAGS) -o build/$@ $^ $(LDFLAGS)
	cp -r example_files/* build

.PHONY: clean
clean:
	rm -rf obj/*
	rm -rf build/*

.PHONY: test
test:
	(cd build;./app test.cfg)

.PHONY: make_folders
make_folders:
	mkdir -p obj
	mkdir -p build

#	gcc main.c -o main -lSDL2 -Wall -lSDL2_image -lSDL2_ttf
#	./main

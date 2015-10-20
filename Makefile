SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_ttf

# Here, I add the flag for the SDL2_net library.
SDL_LDFLAGS += -lSDL2_net

# These flags make sure the compiler warns me about all possible problems.
STRICT_FLAGS := -Wall -Wextra -pedantic


main: main.o
	g++  main.o -o main $(SDL_LDFLAGS)

main.o: main.cpp
	g++ -std=c++11 -c $(SDL_CFLAGS) main.cpp

clean:
	rm -f main main.o

CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic `sdl2-config --cflags` -g
LDFLAGS = `sdl2-config --libs`

ARGS =  200    # width
ARGS += 200    # height
ARGS += 0.2    # density
ARGS += 50     # iteration steps
ARGS += random # array initializing

DELAY = 20

.PHONY: all clean run gif
all: gol sdl_gol

clean:
	$(RM) -rf gol gol.dSYM *.pbm *.gif pbm sdl_gol sdl_gol.dSYM

run: gol
	./gol $(ARGS)

gif: clean run
	convert -filter point -resize 300%x300% -delay $(DELAY) ./pbm/gol_*.pbm gol.gif

gol: gol.c dir.c
	$(CC) $(CFLAGS) -o $@ $^

sdl_gol: sdl_gol.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)


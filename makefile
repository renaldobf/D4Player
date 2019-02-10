CC=gcc
CFLAGS=-Wall -O3
LDFLAGS=-s
LDLIBS=-lm -lalleg -lpng

all: build/common.o build/director/cast.o build/director/castlist.o build/director/clut.o build/director/imap.o build/director/keys.o build/director/mmap.o build/director/movie.o build/director/movieinf.o build/director/packbits.o build/director/palette.o build/director/rect.o build/director/rifxhead.o build/director/score.o build/director/sound.o build/director/text.o build/extract.o build/imgwrap.o build/main.o build/player/allegro.o build/player/effects.o build/player/pal_fx.o build/player/patterns.o build/player/player.o build/player/shapes.o build/player/trans_fx.o build/wave.o 
	$(CC) $(CFLAGS) -o d4player build/common.o build/director/cast.o build/director/castlist.o build/director/clut.o build/director/imap.o build/director/keys.o build/director/mmap.o build/director/movie.o build/director/movieinf.o build/director/packbits.o build/director/palette.o build/director/rect.o build/director/rifxhead.o build/director/score.o build/director/sound.o build/director/text.o build/extract.o build/imgwrap.o build/main.o build/player/allegro.o build/player/effects.o build/player/pal_fx.o build/player/patterns.o build/player/player.o build/player/shapes.o build/player/trans_fx.o build/wave.o  $(LDFLAGS) $(LDLIBS)

build/common.o: src/common.c src/common.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/common.c -o build/common.o

build/director/cast.o: src/director/cast.c src/director/cast.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/cast.c -o build/director/cast.o

build/director/castlist.o: src/director/castlist.c src/director/castlist.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/castlist.c -o build/director/castlist.o

build/director/clut.o: src/director/clut.c src/director/clut.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/clut.c -o build/director/clut.o

build/director/imap.o: src/director/imap.c src/director/imap.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/imap.c -o build/director/imap.o

build/director/keys.o: src/director/keys.c src/director/keys.h src/common.h src/director/mmap.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/keys.c -o build/director/keys.o

build/director/mmap.o: src/director/mmap.c src/director/mmap.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/mmap.c -o build/director/mmap.o

build/director/movie.o: src/director/movie.c src/director/movie.h src/common.h src/director/rifxhead.h src/director/imap.h src/director/mmap.h src/director/movieinf.h src/director/keys.h src/director/castlist.h src/director/score.h src/director/cast.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/movie.c -o build/director/movie.o

build/director/movieinf.o: src/director/movieinf.c src/director/movieinf.h src/common.h src/director/rect.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/movieinf.c -o build/director/movieinf.o

build/director/packbits.o: src/director/packbits.c src/director/packbits.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/packbits.c -o build/director/packbits.o

build/director/palette.o: src/director/palette.c src/director/palette.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/palette.c -o build/director/palette.o

build/director/rect.o: src/director/rect.c src/director/rect.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/rect.c -o build/director/rect.o

build/director/rifxhead.o: src/director/rifxhead.c src/director/rifxhead.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/rifxhead.c -o build/director/rifxhead.o

build/director/score.o: src/director/score.c src/director/score.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/score.c -o build/director/score.o

build/director/sound.o: src/director/sound.c src/director/sound.h src/common.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/sound.c -o build/director/sound.o

build/director/text.o: src/director/text.c src/director/text.h 
	@ mkdir -p build/director
	$(CC) $(CFLAGS) -c src/director/text.c -o build/director/text.o

build/extract.o: src/extract.c src/extract.h src/common.h src/director/movie.h src/director/sound.h src/director/cast.h src/director/palette.h src/director/clut.h src/director/packbits.h src/director/text.h src/imgwrap.h src/wave.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/extract.c -o build/extract.o

build/imgwrap.o: src/imgwrap.c src/imgwrap.h src/common.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/imgwrap.c -o build/imgwrap.o

build/main.o: src/main.c src/main.h src/common.h src/director/movie.h src/player/player.h src/extract.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/player/allegro.o: src/player/allegro.c src/player/allegro.h src/common.h 
	@ mkdir -p build/player
	$(CC) $(CFLAGS) -c src/player/allegro.c -o build/player/allegro.o

build/player/effects.o: src/player/effects.c src/player/effects.h src/player/allegro.h 
	@ mkdir -p build/player
	$(CC) $(CFLAGS) -c src/player/effects.c -o build/player/effects.o

build/player/pal_fx.o: src/player/pal_fx.c src/player/pal_fx.h src/player/allegro.h src/director/palette.h 
	@ mkdir -p build/player
	$(CC) $(CFLAGS) -c src/player/pal_fx.c -o build/player/pal_fx.o

build/player/patterns.o: src/player/patterns.c src/player/patterns.h src/player/allegro.h src/common.h 
	@ mkdir -p build/player
	$(CC) $(CFLAGS) -c src/player/patterns.c -o build/player/patterns.o

build/player/player.o: src/player/player.c src/player/player.h src/common.h src/extract.h src/director/movie.h src/director/palette.h src/director/cast.h src/director/clut.h src/director/packbits.h src/player/allegro.h src/player/effects.h src/player/patterns.h src/player/shapes.h src/player/trans_fx.h src/player/pal_fx.h 
	@ mkdir -p build/player
	$(CC) $(CFLAGS) -c src/player/player.c -o build/player/player.o

build/player/shapes.o: src/player/shapes.c src/player/shapes.h src/common.h src/player/allegro.h src/player/patterns.h 
	@ mkdir -p build/player
	$(CC) $(CFLAGS) -c src/player/shapes.c -o build/player/shapes.o

build/player/trans_fx.o: src/player/trans_fx.c src/player/trans_fx.h 
	@ mkdir -p build/player
	$(CC) $(CFLAGS) -c src/player/trans_fx.c -o build/player/trans_fx.o

build/wave.o: src/wave.c src/wave.h src/common.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/wave.c -o build/wave.o

clean:
	rm -rf -- build d4player

install: d4player
	./install

.PHONY: install


# D4Player

D4Player is a player for Macromedia Director 4 movie files.

It depends on [Allegro 4](http://liballeg.org/index.html) and [libpng](http://www.libpng.org/pub/png/libpng.html) libraries.

On Debian/Ubuntu, you can get both them from the apt repositories.

	apt-get install liballegro4-dev libpng12-dev
	
## Building

To build, just invoke `make`.

	make

If you modify the source code, then you need to reconfigure first.

	./configure
	make

It is possible to build without the libpng library, defining `ENABLE_PNG=0` on make. In this case, images will be written in [Netpbm](https://en.wikipedia.org/wiki/Netpbm_format) format as PPM files. Note that alpha channel will be discarded.

	make CFLAGS=-DENABLE_PNG=0
	
## Running

To play a movie, simply run:

	./d4player /path/to/file

To extract the files from the movie, use the `-c` option.

	./d4player -c all /path/to/file
	
For the full set of available options, run with `-h` option.

	./d4player -h

## Known issues

The player should able to work with Director 4 files, both little and big endian encodings. The file version is not checked prior to playing it.
It is possible to check warnings using the "no play" option `-np`.

On Linux, some users may experience lack of sound due to incompatibility with OSS and the way Allegro was compiled, specially on Debian/Ubuntu packages version 4.4.2 and up. Using `padsp` wrapper might be a workaround if your system uses PulseAudio:

	padsp ./d4player /path/to/file

The player is not capable of running Lingo scripts. If the movie has scripts, specially if it heavely depends on them, chances are that the frames are simply going to flash on screen before the player exits.

The reference to external files is not yet implemented, so only self-contained movies can be played for now.
Movie clips, buttons and custom palletes are not yet implemented.

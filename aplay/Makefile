default:
	gcc --fast-math -nostartfiles -Os -lm -o test main.c && strip --strip-unneeded test && rm test2.xz && cp test test2 && xz test2 && ./test | aplay -q -t raw -f FLOAT_LE -c2 -r44100

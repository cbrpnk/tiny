default:
	gcc -ffast-math -fno-stack-protector -fno-asynchronous-unwind-tables -nostartfiles -Os -lm -o test main.c && strip --strip-unneeded -R .eh_frame test && rm test2.lzma && cp test test2 && lzma -9 test2 && ./test | aplay -q -t raw -f FLOAT_LE -c2 -r44100

debug:
	gcc -g -ffast-math -fno-stack-protector -fno-asynchronous-unwind-tables -nostartfiles -Os -lm -o test main.c && rm test2.xz && cp test test2 && xz test2 && ./test | aplay -q -t raw -f FLOAT_LE -c2 -r44100

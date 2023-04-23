default:
	gcc -Wall -pedantic -lm -lportaudio -lasound -ljack -pthread -o test main.c && ./test
intro:
	gcc -lm -lportaudio -lasound -ljack -pthread -o out main.c && xz out && cat decompressor.sh out.xz > test && rm out.xz && ./test

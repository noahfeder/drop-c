drop: drop.c
	clang -ggdb3 -O0 -std=c11 -Wall -o drop drop.c -lm

clean:
	rm -f *.o a.out core drop log.txt

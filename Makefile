main : main.o functions.o
	gcc -O3 -g3 -o main main.o functions.o -lm

main.o : main.c
	gcc -O3 -g3 -c main.c

functions.o : functions.c
	gcc -O3 -g3 -c functions.c

clean:
	rm main ; rm *.o; rm Results.csv

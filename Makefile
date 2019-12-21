main : main.o functions.o Result.o Intermediate_Result.o
	gcc -O3 -g3 -o main main.o functions.o Result.o Intermediate_Result.o -lm

main.o : main.c
	gcc -O3 -g3 -c main.c

functions.o : functions.c
	gcc -O3 -g3 -c functions.c

Result.o : Result.c
	gcc -O3 -g3 -c Result.c

Intermediate_Result.o : Intermediate_Result.c
	gcc -O3 -g3 -c Intermediate_Result.c

clean:
	rm main ; rm *.o;

project: main.o func.o
	gcc main.o func.o -o project

main.o: main.c func.h
	gcc -c main.c

func.o: func.c func.h
	gcc -c func.c
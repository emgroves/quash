Quash: main.o
			gcc main.o -o Quash -lreadline
		
main.o: main.c
			gcc -c -g main.c -lreadline

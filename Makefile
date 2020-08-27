CC = gcc
EFLAGS = -Wall -pedantic -Wconversion -Werror			## error flags
OFLAGS = -g -lm -fopenmp -march=native				## other flags


compile: programa_paralelo_01.c
	cppcheck ./
	gcc $(EFLAGS) $(OFLAGS) programa_paralelo_01.c simple_bmp.c -o parallel

clean:
	rm -r Doxyfile parallel html/ latex/ 

doc:
	doxygen -g Doxyfile
	doxygen Doxyfile



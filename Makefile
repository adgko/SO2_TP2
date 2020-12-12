CC = gcc
EFLAGS = -Wall -pedantic -Wconversion -Werror			## flags de error
OFLAGS = -g -lm -fopenmp -march=native				## otras flags de error

all: check compile 

compile: 
	mkdir -p ./bin
	gcc $(EFLAGS) $(OFLAGS) src/parallel.c src/simple_bmp.c -o bin/parallel
	gcc $(EFLAGS) $(OFLAGS) src/serial.c src/simple_bmp.c -o bin/serial

clean:
	rm -r Doxyfile parallel html/ latex/ 

docs:
	doxygen Doxyfile
	
check:
	cppcheck ./


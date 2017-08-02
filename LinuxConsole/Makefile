all: proyecto

proyecto: proyecto.o 
	gcc -o proyecto proyecto.o 

proyecto.o: proyecto.c 
	gcc -c proyecto.c

clean:
	rm -rf proyecto proyecto.o Tiempo.txt

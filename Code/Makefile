#Práctica 1
#Código fuente: PracticaConcurrente.c
#GEI
#Marta Albets Mitjaneta
#Paula Gallucci Zurita

# Número de threads a realizar.
THREADS = 12

.SILENT:

all: compile test
compile: PracticaConcurrente.c ConvexHull.h
	gcc PracticaConcurrente.c -o PracticaConcurrente -I. -lm -lpthread

test: PracticaConcurrente
	echo "**** Ejemplo 1 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo1.dat $(THREADS) Ejemplo1.res 
	
	echo "**** Ejemplo 2 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo2.dat $(THREADS) Ejemplo2.res
	
	echo "**** Ejemplo 3 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo3.dat $(THREADS) Ejemplo3.res
	
	echo "**** Ejemplo 4 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo4.dat $(THREADS) Ejemplo4.res
	
	echo "**** Ejemplo 5 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo5.dat $(THREADS) Ejemplo5.res

	echo "**** Ejemplo 6 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo6.dat $(THREADS) Ejemplo6.res
	
	echo "**** Ejemplo 7 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo7.dat $(THREADS) Ejemplo7.res
	
	echo "**** Ejemplo 8 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo8.dat $(THREADS) Ejemplo8.res
	
	echo "**** Ejemplo 9 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo9.dat $(THREADS) Ejemplo9.res
	
	echo "**** Ejemplo 10 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo10.dat $(THREADS) Ejemplo10.res
	
	echo "**** Ejemplo 11 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo11.dat $(THREADS) Ejemplo11.res
	
	echo "**** Ejemplo 12 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo12.dat $(THREADS) Ejemplo12.res
	
	echo "**** Ejemplo 13 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo13.dat $(THREADS) Ejemplo13.res
	
	echo "**** Ejemplo 14 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo14.dat $(THREADS) Ejemplo14.res
	
	echo "**** Ejemplo 15 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo15.dat $(THREADS) Ejemplo15.res

	echo "**** Ejemplo 16 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo16.dat $(THREADS) Ejemplo16.res
	
	echo "**** Ejemplo 17 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo17.dat $(THREADS) Ejemplo17.res
	
	echo "**** Ejemplo 18 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo18.dat $(THREADS) Ejemplo18.res
	
	echo "**** Ejemplo 19 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo19.dat $(THREADS) Ejemplo19.res
	
	echo "**** Ejemplo 20 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo20.dat $(THREADS) Ejemplo20.res
	
	echo "**** Ejemplo 21 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo21.dat $(THREADS) Ejemplo21.res
	
	echo "**** Ejemplo 22 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo22.dat $(THREADS) Ejemplo22.res
	
	echo "**** Ejemplo 23 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo23.dat $(THREADS) Ejemplo23.res
	
	echo "**** Ejemplo 24 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo24.dat $(THREADS) Ejemplo24.res
	
	echo "**** Ejemplo 25 ****"
	time ./PracticaConcurrente Ejemplos/Ejemplo25.dat $(THREADS) Ejemplo25.res
	
	echo "**** Ejemplo Gordo ****"
	time ./PracticaConcurrente Ejemplos/EjemploGordo.dat $(THREADS) EjemploGordo.res

	echo "*** Utiliza Make clean para eliminar los resultados ***"
clean:
	echo "**** Eliminando archivos *****"
	rm -f PracticaConcurrente *.res
	ls

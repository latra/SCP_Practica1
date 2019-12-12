/* --------------------------------------------------------------------
Práctica 1
Código fuente: PracticaConcurrente.c
GEI
Marta Albets Mitjaneta
Paula Gallucci Zurita
----------------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ConvexHull.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#define DMaxArboles 	25
#define DMaximoCoste 999999
#define S 10000
#define DDebug 0


  //////////////////////////
 // Estructuras de datos //
//////////////////////////


// Definicin estructura arbol entrada (Conjunto �boles).
struct  Arbol
{
	int	  IdArbol;
	Point Coord;			// Posicin �bol
	int Valor;				// Valor / Coste �bol.
	int Longitud;			// Cantidad madera �bol
};
typedef struct Arbol TArbol, *PtrArbol;



// Definicin estructura Bosque entrada (Conjunto �boles).
struct Bosque
{
	int 		NumArboles;
	TArbol 	Arboles[DMaxArboles];
};
typedef struct Bosque TBosque, *PtrBosque;
int numeroThreads;


// Combinacin .
struct ListaArboles
{
	int 		NumArboles;	//arboles 
 	float		Coste; // coste total del mapa
	float		CosteArbolesCortados;	//coste actual
	float		CosteArbolesRestantes; // coste restante para talar todo
	float		LongitudCerca; 	// logitud conseguida
	float		MaderaSobrante; // madera restante
	int 		Arboles[DMaxArboles]; // arboles talados
};
typedef struct ListaArboles TListaArboles, *PtrListaArboles,**PartialLista;



struct ParamsThread
{
	int inicio;
	int final;
	PtrListaArboles optimo;
};
typedef struct ParamsThread TParamsThread, *PtrParamsThread;

// Vector est�ico Coordenadas.
typedef Point TVectorCoordenadas[DMaxArboles], *PtrVectorCoordenadas;


typedef enum {false, true} bool;



bool CalcularCombinacionOptimaSequencial(int PrimeraCombinacion, int UltimaCombinacion, PtrListaArboles Optimo);
bool LeerFicheroEntrada(char *PathFicIn);
bool GenerarFicheroSalida(TListaArboles optimo, char *PathFicOut);
bool CalcularCercaOptima(PtrListaArboles Optimo);
void OrdenarArboles();
bool CalcularCombinacionOptima(PtrParamsThread parametros);
int EvaluarCombinacionListaArboles(int Combinacion);
int ConvertirCombinacionToArboles(int Combinacion, PtrListaArboles CombinacionArboles);
int ConvertirCombinacionToArbolesTalados(int Combinacion, PtrListaArboles CombinacionArbolesTalados);
void ObtenerListaCoordenadasArboles(TListaArboles CombinacionArboles, TVectorCoordenadas Coordenadas);
float CalcularLongitudCerca(TVectorCoordenadas CoordenadasCerca, int SizeCerca);
float CalcularDistancia(int x1, int y1, int x2, int y2);
int CalcularMaderaArbolesTalados(TListaArboles CombinacionArboles);
int CalcularCosteCombinacion(TListaArboles CombinacionArboles);
void MostrarArboles(TListaArboles CombinacionArboles);
void CalculoCargaDeTrabajo(PtrParamsThread paramsThread, int MaxCombinaciones);
bool CalcularCercaOptimaSequencial(PtrListaArboles Optimo);
  ////////////////////////
 // Variables Globales //
////////////////////////

TBosque ArbolesEntrada;

int main(int argc, char *argv[])
{
	TListaArboles Optimo;
	
	if (argc<3 || argc>4)
	{
		printf("Error Argumentos. Usage: CalcArboles <Fichero_Entrada> <Threads> [<Fichero_Salida>]\n");
		exit(1);
	}
	if (!LeerFicheroEntrada(argv[1]))
	{
		printf("Error lectura fichero entrada.\n");
		exit(1);
	}
	numeroThreads = atoi(argv[2]);
	if (numeroThreads == 0)
	{
		printf("No se ha introducido un valor numérico válido para indicar el número de Threads.\n");
		exit(1);
	}
	if (numeroThreads == 1)
	{
		printf("Solo se ha indicado un thread. La ejecución será secuencial.\n");
		if (!CalcularCercaOptimaSequencial(&Optimo))
		{
			printf("Error CalcularCercaOptima.\n");
			exit(1);
		}
	}
	else
	{
		printf("Se han introducio %i threads\n", numeroThreads);
		if (!CalcularCercaOptima(&Optimo))
		{
			printf("Error CalcularCercaOptima.\n");
			exit(1);
		}
	}

	if (argc==3)
	{
		if (!GenerarFicheroSalida(Optimo, "./Valla.res"))
		{

			printf("Error GenerarFicheroSalida.\n");
			exit(1);
		}
	}
	else
	{
		if (!GenerarFicheroSalida(Optimo, argv[3]))
		{
			printf("Error GenerarFicheroSalida.\n");
			exit(1);
		}
	}
	exit(0);
}
#pragma region Funciones Secuenciales Originales
bool CalcularCercaOptimaSequencial(PtrListaArboles Optimo)
{
	int MaxCombinaciones;

	/* C�culo Máximo Combinaciones */
	MaxCombinaciones = (int) pow(2.0,ArbolesEntrada.NumArboles);

	// Ordenar Arboles por segun coordenadas crecientes de x,y
	OrdenarArboles();

	/* C�culo �timo */
	Optimo->NumArboles = 0;
	Optimo->Coste = DMaximoCoste;
	CalcularCombinacionOptimaSequencial(1, MaxCombinaciones, Optimo);

	return true;
}

bool CalcularCombinacionOptimaSequencial(int PrimeraCombinacion, int UltimaCombinacion, PtrListaArboles Optimo)
{
	int Combinacion, MejorCombinacion=0, CosteMejorCombinacion;
	int Coste;
	TListaArboles OptimoParcial;

	TListaArboles CombinacionArboles;
	TVectorCoordenadas CoordArboles, CercaArboles;
	int NumArboles, PuntosCerca;
	float MaderaArbolesTalados;

  	printf("Evaluacin Combinaciones posibles: \n");
	CosteMejorCombinacion = Optimo->Coste;
	for (Combinacion=PrimeraCombinacion; Combinacion<UltimaCombinacion; Combinacion++)
	{
		Coste = EvaluarCombinacionListaArboles(Combinacion);
		if ( Coste < CosteMejorCombinacion )
		{
			CosteMejorCombinacion = Coste;
			MejorCombinacion = Combinacion;
		}
		if ((Combinacion%S)==0)
		{
			 ConvertirCombinacionToArbolesTalados(MejorCombinacion, &OptimoParcial);
			 printf("\r[%d] OptimoParcial %d-> Coste %d, %d Arboles talados:", Combinacion, MejorCombinacion, CosteMejorCombinacion, OptimoParcial.NumArboles);
			 MostrarArboles(OptimoParcial);
		}
			
	}

	printf("\n");
	
	ConvertirCombinacionToArbolesTalados(MejorCombinacion, &OptimoParcial);
	printf("\rOptimo %d-> Coste %d, %d Arboles talados:", MejorCombinacion, CosteMejorCombinacion, OptimoParcial.NumArboles);
	MostrarArboles(OptimoParcial);
	printf("\n");

	if (CosteMejorCombinacion == Optimo->Coste)
		return false;  // No se ha encontrado una combinacin mejor.

	ConvertirCombinacionToArbolesTalados(MejorCombinacion, Optimo);
	Optimo->Coste = CosteMejorCombinacion;
	NumArboles = ConvertirCombinacionToArboles(MejorCombinacion, &CombinacionArboles);
	ObtenerListaCoordenadasArboles(CombinacionArboles, CoordArboles);
	PuntosCerca = chainHull_2D( CoordArboles, NumArboles, CercaArboles );

	Optimo->LongitudCerca = CalcularLongitudCerca(CercaArboles, PuntosCerca);
	MaderaArbolesTalados = CalcularMaderaArbolesTalados(*Optimo);
	Optimo->MaderaSobrante = MaderaArbolesTalados - Optimo->LongitudCerca;
	Optimo->CosteArbolesCortados = CosteMejorCombinacion;
	Optimo->CosteArbolesRestantes = CalcularCosteCombinacion(CombinacionArboles);

	return true;
}
#pragma endregion

#pragma region Tratamiento de ficheros
bool LeerFicheroEntrada(char *PathFicIn)
{
	FILE *FicIn;
	int a;

	FicIn=fopen(PathFicIn,"r");
	if (FicIn==NULL)
	{
		perror("Lectura Fichero entrada.");
		return false;
	}
	printf("Datos Entrada:\n");

	// Leemos el nmero de arboles del bosque de entrada.
	if (fscanf(FicIn, "%d", &(ArbolesEntrada.NumArboles))<1)
	{
		perror("Lectura arboles entrada");
		return false;
	}
	printf("\tÁrboles: %d.\n",ArbolesEntrada.NumArboles);

	// Leer atributos arboles.
	for(a=0;a<ArbolesEntrada.NumArboles;a++)
	{
		ArbolesEntrada.Arboles[a].IdArbol=a+1;
		// Leer x, y, Coste, Longitud.
		if (fscanf(FicIn, "%d %d %d %d",&(ArbolesEntrada.Arboles[a].Coord.x), &(ArbolesEntrada.Arboles[a].Coord.y), &(ArbolesEntrada.Arboles[a].Valor), &(ArbolesEntrada.Arboles[a].Longitud))<4)
		{
			perror("Lectura datos arbol.");
			return false;
		}	
		printf("\tÁrbol %d-> (%d,%d) Coste:%d, Long:%d.\n",a+1,ArbolesEntrada.Arboles[a].Coord.x, ArbolesEntrada.Arboles[a].Coord.y, ArbolesEntrada.Arboles[a].Valor, ArbolesEntrada.Arboles[a].Longitud);
	}

	return true;
}


//Guarda el resultado
bool GenerarFicheroSalida(TListaArboles Optimo, char *PathFicOut)
{
	FILE *FicOut;
	int a;
	FicOut=fopen(PathFicOut,"w+");
	if (FicOut==NULL)
	{
		perror("Escritura fichero salida.");
		return false;
	}

	// Escribir arboles a talartalado.
		// Escribimos nmero de arboles a talar.
	if (fprintf(FicOut, "Cortar %d árbol/es: ", Optimo.NumArboles)<1)
	{
		perror("Escribir nmero de arboles a talar");
		return false;
	}

	for(a=0;a<Optimo.NumArboles;a++)
	{
		// Escribir nmero arbol.
		if (fprintf(FicOut, "%d ",ArbolesEntrada.Arboles[Optimo.Arboles[a]].IdArbol)<1)
		{
			perror("Escritura nmero �bol.");
			return false;
		}
	}

	// Escribimos coste arboles a talar.
	if (fprintf(FicOut, "\nMadera Sobrante: \t%4.2f (%4.2f)", Optimo.MaderaSobrante,  Optimo.LongitudCerca)<1)
	{
		perror("Escribir coste arboles a talar.");
		return false;
	}

	// Escribimos coste arboles a talar.
	if (fprintf(FicOut, "\nValor árboles cortados: \t%4.2f.", Optimo.CosteArbolesCortados)<1)
	{
		perror("Escribir coste arboles a talar.");
		return false;
	}

		// Escribimos coste arboles a talar.
	if (fprintf(FicOut, "\nValor árboles restantes: \t%4.2f\n", Optimo.CosteArbolesRestantes)<1)
	{
		perror("Escribir coste arboles a talar.");
		return false;
	}

	return true;
}
#pragma endregion


bool CalcularCercaOptima(PtrListaArboles Optimo)
{
	int i;
	int MaxCombinaciones;
	bool result = false;
	pthread_t tids[numeroThreads];
	Optimo->Coste=DMaximoCoste;
	TParamsThread paramsThread[numeroThreads];
	TListaArboles listaParcial[numeroThreads];
	for (i = 0 ; i<numeroThreads;i++)
	{
		typedef struct ListaArboles OptimoFake;
		OptimoFake optimoFake;
		//Inicializamos los valores de inicio y de final a 0 para evitar problemas a la hora de calcular el peso de cada thread
		paramsThread[i].inicio=0;
		paramsThread[i].final=0;
		
		paramsThread[i].optimo = &listaParcial[i];

	}

	/* C�culo Máximo Combinaciones */
	MaxCombinaciones = (int) pow(2.0,ArbolesEntrada.NumArboles);
	
	// Calculamos la carga de trabajo de cada thread
	CalculoCargaDeTrabajo(paramsThread, MaxCombinaciones);
	
	// Ordenar Arboles por segun coordenadas crecientes de x,y
	OrdenarArboles();


	//for number of theados : thread_create(rango/number * number actual)
	// CalcularCombinacionOptima(1, 30, Optimo);
	for (i = 0 ; i < numeroThreads ; i++ )
	{
		// Creamos los threads correspondientes
		if(pthread_create(&tids[i], NULL, (void *(*) (void *)) CalcularCombinacionOptima, &(paramsThread[i]))!=0)
		{
			perror("Error al crear el thread\n");
			exit(1);
		}
	}
	for (i=0; i<numeroThreads; i++)
	{
		//Esperamos la finalización del thread actual y, si no han habido errores, comparamos respecto el optimo actual.
		// Si este optimo es menor, actualizamos el optimo
		if(pthread_join(tids[i], (void**) NULL))
		{
			perror("Error al hacer el join del thread\n");
			pthread_cancel(tids[i]);
			exit(-1);	
		}
		else
		{
			if (paramsThread[i].optimo->Coste < Optimo->Coste)
			{
				*Optimo = *paramsThread[i].optimo;
			}
		}
		
	}
	printf("\nEl coste optimo es %f\n", Optimo->Coste);
	return true;
}

void CalculoCargaDeTrabajo(PtrParamsThread paramsThread, int MaxCombinaciones)
{
	int i = 0;
	while (i < numeroThreads)
	{
		// Calculamos el óptimo para cada thread
		paramsThread[i].final = (int) (MaxCombinaciones/(numeroThreads-i) + 1);
		MaxCombinaciones -= paramsThread[i].final;
		i++;
	}
	int total = 0;
	// modificamos el vector indicando la ultima combinacion (incluida) que realizara el thread 
	// correspondiendo al indice
	for (i = 0; i < numeroThreads ; i++)
	{
		if (i == 0)
		{
			paramsThread[i].inicio = 1;
		}
		else
		{
			paramsThread[i].inicio = paramsThread[i-1].final;
		}
		
		paramsThread[i].final += total;
		total = paramsThread[i].final; 
	}

}
// ESTA FUNCION ORDENA LOS ARBOLES PARA QUE TENGAN UNA ESTRUCTURA LOGICA SEGUIBLE
void OrdenarArboles()
{
  int a,b;
  
	for(a=0; a<(ArbolesEntrada.NumArboles-1); a++)
	{
		for(b=1; b<ArbolesEntrada.NumArboles; b++)
		{
			if ( ArbolesEntrada.Arboles[b].Coord.x < ArbolesEntrada.Arboles[a].Coord.x ||
				 (ArbolesEntrada.Arboles[b].Coord.x == ArbolesEntrada.Arboles[a].Coord.x && ArbolesEntrada.Arboles[b].Coord.y < ArbolesEntrada.Arboles[a].Coord.y) )
			{
				TArbol aux;

				// aux=a
				aux.Coord.x = ArbolesEntrada.Arboles[a].Coord.x;
				aux.Coord.y = ArbolesEntrada.Arboles[a].Coord.y;
				aux.IdArbol = ArbolesEntrada.Arboles[a].IdArbol;
				aux.Valor = ArbolesEntrada.Arboles[a].Valor;
				aux.Longitud = ArbolesEntrada.Arboles[a].Longitud;

				// a=b
				ArbolesEntrada.Arboles[a].Coord.x = ArbolesEntrada.Arboles[b].Coord.x;
				ArbolesEntrada.Arboles[a].Coord.y = ArbolesEntrada.Arboles[b].Coord.y;
				ArbolesEntrada.Arboles[a].IdArbol = ArbolesEntrada.Arboles[b].IdArbol;
				ArbolesEntrada.Arboles[a].Valor = ArbolesEntrada.Arboles[b].Valor;
				ArbolesEntrada.Arboles[a].Longitud = ArbolesEntrada.Arboles[b].Longitud;

				// b=aux
				ArbolesEntrada.Arboles[b].Coord.x = aux.Coord.x;
				ArbolesEntrada.Arboles[b].Coord.y = aux.Coord.y;
				ArbolesEntrada.Arboles[b].IdArbol = aux.IdArbol;
				ArbolesEntrada.Arboles[b].Valor = aux.Valor;
				ArbolesEntrada.Arboles[b].Longitud = aux.Longitud;
			}
		}
	}
}


// Calcula la combinacin ptima entre el rango de combinaciones PrimeraCombinacion-UltimaCombinacion.
// ENtre un rango devuelve cual de ellos es el mejor
bool CalcularCombinacionOptima(PtrParamsThread parametros)
{
	
	int PrimeraCombinacion = parametros->inicio; 
	int UltimaCombinacion = parametros->final;
	int Combinacion, MejorCombinacion=0, CosteMejorCombinacion;
	int Coste;
	
	TListaArboles OptimoParcial;

	TListaArboles CombinacionArboles;
	TVectorCoordenadas CoordArboles, CercaArboles;
	int NumArboles, PuntosCerca;
	float MaderaArbolesTalados;

	CosteMejorCombinacion = DMaximoCoste;
	for (Combinacion=PrimeraCombinacion; Combinacion<UltimaCombinacion; Combinacion++)
	{
		Coste = EvaluarCombinacionListaArboles(Combinacion);
		if ( Coste < CosteMejorCombinacion )
		{
			CosteMejorCombinacion = Coste;
			MejorCombinacion = Combinacion;
		}
		if ((Combinacion%S)==0)
		{
			 ConvertirCombinacionToArbolesTalados(MejorCombinacion, &OptimoParcial);
			 MostrarArboles(OptimoParcial);
		}
	}
	
	ConvertirCombinacionToArbolesTalados(MejorCombinacion, &OptimoParcial);
	MostrarArboles(OptimoParcial);

	if (CosteMejorCombinacion == parametros->optimo->Coste)
	{
		
			return false;  // No se ha encontrado una combinacin mejor.
	}
	// Asignar combinacin encontrada.
	ConvertirCombinacionToArbolesTalados(MejorCombinacion, parametros->optimo);
	parametros->optimo->Coste = CosteMejorCombinacion;
	// Calcular estadisticas óptimo.
	NumArboles = ConvertirCombinacionToArboles(MejorCombinacion, &CombinacionArboles);

	ObtenerListaCoordenadasArboles(CombinacionArboles, CoordArboles);
	PuntosCerca = chainHull_2D( CoordArboles, NumArboles, CercaArboles );

	parametros->optimo->LongitudCerca = CalcularLongitudCerca(CercaArboles, PuntosCerca);
	MaderaArbolesTalados = CalcularMaderaArbolesTalados(*parametros->optimo);
	parametros->optimo->MaderaSobrante = MaderaArbolesTalados - parametros->optimo->LongitudCerca;
	parametros->optimo->CosteArbolesCortados = CosteMejorCombinacion;
	parametros->optimo->CosteArbolesRestantes = CalcularCosteCombinacion(CombinacionArboles);

	return true;
}


// CAlcula el coste a partir de la ID de la combinacion
int EvaluarCombinacionListaArboles(int Combinacion)
{
	TVectorCoordenadas CoordArboles, CercaArboles;
	TListaArboles CombinacionArboles, CombinacionArbolesTalados;
	int NumArboles, NumArbolesTalados, PuntosCerca, CosteCombinacion;
	float LongitudCerca, MaderaArbolesTalados;

	// Convertimos la combinacin al vector de arboles no talados.
	NumArboles = ConvertirCombinacionToArboles(Combinacion, &CombinacionArboles);

	// Obtener el vector de coordenadas de arboles no talados.
	ObtenerListaCoordenadasArboles(CombinacionArboles, CoordArboles);

	// Calcular la cerca
	// PuntosCerca es el tamaño que tiene que tener la valla
	// cercaarboles tiene pinta de ser "el dibujo" de la valla
	PuntosCerca = chainHull_2D( CoordArboles, NumArboles, CercaArboles );

	/* Evaluar si obtenemos suficientes �boles para construir la cerca */

	// Utiliza los puntos de "CErca Arboles" para obtener el tamaño total de la valla. 
	// PuntosCerca es el numero de puntos que hay en cerca arboles ( se pasa para poder iterar sin que pete)
	LongitudCerca = CalcularLongitudCerca(CercaArboles, PuntosCerca);
	// Evaluar la madera obtenida mediante los arboles talados.
	// Calcula/Enc(uentra los arboles cortados en esta combinacion y los cuenta. Los arboles talados los
	// guardamos en combinacionarbolestalados
	NumArbolesTalados = ConvertirCombinacionToArbolesTalados(Combinacion, &CombinacionArbolesTalados);
	if (DDebug) 
		printf(" %d arboles cortados: ",NumArbolesTalados);
	if (DDebug) 
		MostrarArboles(CombinacionArbolesTalados);
	
	// A partir de los arboles talados calculamos la madera obtenida
  	MaderaArbolesTalados = CalcularMaderaArbolesTalados(CombinacionArbolesTalados);
	if (DDebug) 
		printf("  Madera:%4.2f  \tCerca:%4.2f ",MaderaArbolesTalados, LongitudCerca);
	//miramos si hemos podido construir la valla con la madera
	if (LongitudCerca > MaderaArbolesTalados)
	{
		// Los arboles cortados no tienen suficiente madera para construir la cerca.
		if (DDebug) 
			printf("\tCoste:%d",DMaximoCoste);
    	return DMaximoCoste;
	}

	// Evaluar el coste de los arboles talados ( cuanto nos ha costado talarlos ).
	CosteCombinacion = CalcularCosteCombinacion(CombinacionArbolesTalados);
if (DDebug) printf("\tCoste:%d",CosteCombinacion);
	// SOlo devovemos coste combinacion
	return CosteCombinacion;
}

// "Recupera el mapa de arboles" y devuelve el numero total de arboles
int ConvertirCombinacionToArboles(int Combinacion, PtrListaArboles CombinacionArboles)
{
	//printf("LA mejor combi de este thread es %i\n", Combinacion);
	int arbol=0;

	CombinacionArboles->NumArboles=0;
	CombinacionArboles->Coste=0;

	while (arbol<ArbolesEntrada.NumArboles)
	{
		//printf("Hola bucle\n");
		if ((Combinacion%2)==0)
		{
			CombinacionArboles->Arboles[CombinacionArboles->NumArboles]=arbol;
			CombinacionArboles->NumArboles++;
			CombinacionArboles->Coste+= ArbolesEntrada.Arboles[arbol].Valor;
		}
		arbol++;
		Combinacion = Combinacion>>1;
	}
//	printf("FIN BUCLE\n");
//	printf("hay %i arboles\n", CombinacionArboles->NumArboles);
	return CombinacionArboles->NumArboles;
}

// COnvierte a partir de una ID de combinacion que arboles se talan y devuelve el numero de arboles talados 
// (ademas de modificar COmbinacionArbloesTAlados)
int ConvertirCombinacionToArbolesTalados(int Combinacion, PtrListaArboles CombinacionArbolesTalados)
{
	int arbol=0;

	CombinacionArbolesTalados->NumArboles=0;
	CombinacionArbolesTalados->Coste=0;

	while (arbol<ArbolesEntrada.NumArboles)
	{
		if ((Combinacion%2)==1)
		{
			CombinacionArbolesTalados->Arboles[CombinacionArbolesTalados->NumArboles]=arbol;
			CombinacionArbolesTalados->NumArboles++;
			CombinacionArbolesTalados->Coste+= ArbolesEntrada.Arboles[arbol].Valor;
		}
		arbol++;
		Combinacion = Combinacion>>1;
	}

	return CombinacionArbolesTalados->NumArboles;
}


// DEtermina donde se encuentran los arboles (asi luego puedes saber el tamaño necesario de la cerca)
void ObtenerListaCoordenadasArboles(TListaArboles CombinacionArboles, TVectorCoordenadas Coordenadas)
{
	int c, arbol;

	for (c=0;c<CombinacionArboles.NumArboles;c++)
	{
    arbol=CombinacionArboles.Arboles[c];
		Coordenadas[c].x = ArbolesEntrada.Arboles[arbol].Coord.x;
		Coordenadas[c].y = ArbolesEntrada.Arboles[arbol].Coord.y;
	}
}


// Calcula que tamaño puede tener la valla 
float CalcularLongitudCerca(TVectorCoordenadas CoordenadasCerca, int SizeCerca)
{
	int x;
	float coste;
	
	for (x=0;x<(SizeCerca-1);x++)
	{
		coste+= CalcularDistancia(CoordenadasCerca[x].x, CoordenadasCerca[x].y, CoordenadasCerca[x+1].x, CoordenadasCerca[x+1].y);
	}
	
	return coste;
}



float CalcularDistancia(int x1, int y1, int x2, int y2)
{
	return(sqrt(pow((double)abs(x2-x1),2.0)+pow((double)abs(y2-y1),2.0)));
}



int 
CalcularMaderaArbolesTalados(TListaArboles CombinacionArboles)
{
	int a;
	int LongitudTotal=0;
	
	for (a=0;a<CombinacionArboles.NumArboles;a++)
	{
		LongitudTotal += ArbolesEntrada.Arboles[CombinacionArboles.Arboles[a]].Longitud;
	}
	
	return(LongitudTotal);
}



int 
CalcularCosteCombinacion(TListaArboles CombinacionArboles)
{
	int a;
	int CosteTotal=0;
	
	for (a=0;a<CombinacionArboles.NumArboles;a++)
	{
		CosteTotal += ArbolesEntrada.Arboles[CombinacionArboles.Arboles[a]].Valor;
	}
	
	return(CosteTotal);
}






void
MostrarArboles(TListaArboles CombinacionArboles)
{
	int a;
	//printf("aha\n");
	//for (a=0;a<CombinacionArboles.NumArboles;a++)
		//printf("%d ",ArbolesEntrada.Arboles[CombinacionArboles.Arboles[a]].IdArbol);

//  for (;a<ArbolesEntrada.NumArboles;a++)
   // printf("  ");
	//printf("AHA\n");  
}

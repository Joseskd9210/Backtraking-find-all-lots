#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMNOM			16	 //maximo tamaño de nombres y marcas

//estructura para guardar la informacion de cada palabra
typedef struct _productos {
	char	nombre[TAMNOM];			//nombre del producto
	int		precio;					//precio
	char	marca[TAMNOM];			// marca
	char	categoria[TAMNOM];		//categoria
} productos;

#define TAMANOLINEA	2048	// un tamaño máximo de línea escogido arbitrariamente
#define SEPARADOR	",\n"		//separador de palabras
#define ARCHIVO	"amadeus.txt"
#define MAXPRODUCTOS	20	//numero maximo de productos
#define MAXCATEGORIAS	5	//numero maximo de categoris
#define	TAMLOTE			5	//tamaño del lote para los productos de los empleados
#define	CRECIMIENTO		1000	//crecimiento si se llena el array

int	visitados[5000];			// se coloca global para afectar a la memoria 
int	numero_visitados;			// numero de nodos visitados
productos		tabla_prods[MAXPRODUCTOS];	// tabla de productos global para no afectar la memoria 
int				cant_productos;
char*			categorias[MAXCATEGORIAS];		//array para categorias
int				cant_categorias;
int				lotenum;						// indica el número de lote al presentar los resulatados 
int				maxprecio;						// máximo precio de un lote, esta es la restricción 1  
/* prototipos */
void recorrer_arbol (int v[]);
void insertion_sort (int array[], int cantidad);  // ordenar el array de nodos visitados 
int busqueda_binaria (int codigo);					// para buscar el cuál nodo ha sido visitado 
int visitado (int v[]);					// devuelve 1 si el nodo ha sido visitado, 0 en caso  que no haya sido visitado
										// aceptable: devuelve 1 el array cumple con las restricciones, 0 en caso que no las cumpla
int aceptable (int v[]);
int repetido (int v[]);					//devuelve 1 si el array tiene indices duplicados, 0 en caso que no 
void presentamos (int v[]); // despliega la información del lote aceptado osea que sea valido 
int	buscar_categoria (char* categoria); // devuelve 1 categoria está en el array de categorias, 0 en caso qe no este
char *strsave (char *nombre);			// guarda la cadena de caracteres en un espacio de memoria nuevo 

int main (void)
{
	FILE*			fp;						// puntero al documento 
	char			linea[TAMANOLINEA];		// array para almacenar las líneas leídas del archivo
	int				i;						// variables temporales de uso múltiple 
	char*			p;
	int		v[TAMLOTE];				// este es el array para guardar los indices a los candidatos 

	fp = fopen (ARCHIVO, "r");	//abrimos el documento
	
	if (fp == NULL) {
		fprintf (stderr, "%s: no se puede abrir el archivo\n", ARCHIVO);
		return (1);	//fallo
	}

	numero_visitados = 0;	//numero de nodos visitados
	//leemos productos
	cant_productos = 0;
	cant_categorias = 0;
	lotenum = 1;	//idicamos el numero lote

	while (fgets(linea, TAMANOLINEA, fp) != NULL) {
		linea[TAMANOLINEA-1] = '\0';	// nos aseguramos que el archivo tiene NULL al final
		if (strlen (linea) < 2) {	//saltamos lineas en blanco 
			continue;
		}
		// extraccion de informacion del producto
		p = strtok (linea, SEPARADOR);	//nombre del producto 
		if (p == NULL) {
			continue;	// si el nombre no es valido
		}
		strcpy (tabla_prods[cant_productos].nombre, strsave (p));
		p = strtok (NULL, SEPARADOR);	// extraemos el precio del producto 
		tabla_prods[cant_productos].precio = atoi (p);
		p = strtok (NULL, SEPARADOR);	// extraemos la marca del producto 
		strcpy (tabla_prods[cant_productos].marca, strsave (p));
		p = strtok (NULL, SEPARADOR);	// extraemos la categoría del producto
		strcpy (tabla_prods[cant_productos].categoria, strsave (p)); // guardamos en la tabla de categorias si no existe 
		if (buscar_categoria (tabla_prods[cant_productos].categoria) == -1) {
			categorias[cant_categorias] = tabla_prods[cant_productos].categoria; // no esta la guardamos
			cant_categorias++;
		}
		cant_productos++;
	}
	fclose (fp); // cerramos el puntero
	// pedimos al usuario el maximo lote
	while (1) {
		printf("Indique el máximo precio del lote ");
		gets(linea);
		maxprecio = atoi(linea);
		if (maxprecio < 1 || maxprecio > 999) {
			printf("Precio invalido debe ser mayor que cero\n");
			continue;
		}
		else {
			break;
		}
	}
	
	// imprimimos
	printf ("%-12.12s", "Nombre");
	printf ("%-12.12s", "Precio");
	printf ("%-20.20s", "Marca");
	printf ("%-12.12s", "Categoria");
	printf ("\n");
	for (i = 0; i < 48; i++) {
		printf ("=");
	}
	printf ("\n");
	// inicializamos el array de candidatos
	for (i = 0; i < TAMLOTE; i++) {
		v[i] = i;	
	}
	recorrer_arbol (v);
		
	return 0;	
}

//recorremos el arbol de candidatos
void recorrer_arbol (int v[])
{
	int i;
	int		v1[TAMLOTE];				//nuevo array construido a partir del array de candidatos

	if (visitado (v))
		return;
	if (aceptable (v)) {
		presentamos (v);
		return;
	}
	for (i = 0; i < TAMLOTE; i++) {
		v1[i] = v[i];	
		
	}
	
	for (i = 0; i < TAMLOTE; i++) {
		if (v[i] > 0) {
			v1[i] = v[i] - 1;
			if (repetido (v1))	//descartamos los elementos repetidos
				continue;
			recorrer_arbol (v1);
			v1[i] = v[i]; // restauramos v1 
		}
		if (v[i] < cant_productos-1) {
			v1[i] = v[i] + 1;
			if (repetido (v1))	//descartamos los elementos repetidos
				continue;
			recorrer_arbol (v1);
			v1[i] = v[i]; // restauramos v1 
		}
	}
}

/* Verificamos que el array no tenga valores repetidos 
 devuelve 1 si el array tiene indices duplicados, 0 en caso contrario*/

int repetido (int v[])
{
	int i;

	for (i = 1; i < TAMLOTE; i++) {
		if (v[i] == v[i-1])
			return 1;	//hay dos indices iguales en el array
	}
	return 0; //tiene indices diferentes 
}

//conseguimos un lote valido
void presentamos (int v[])
{
	int i;

	printf ("Lote %d\n", lotenum); lotenum++;

	for (i = 0; i < TAMLOTE; i++) {
		printf ("%-12.12s", tabla_prods[v[i]].nombre);
		printf ("%-12d",	tabla_prods[v[i]].precio);
		printf ("%-20.20s", tabla_prods[v[i]].marca);
		printf ("%-12.12s", tabla_prods[v[i]].categoria);
		printf ("\n");
	}
	fflush (stdout);
}

// Devuelve 1 si el lote cumple con las condiciones, 0 en caso contrario 
int aceptable (int v[])
{
	int precio_total;
	int	i, j;
	int	cat_existe[MAXCATEGORIAS];	//aqui marcaremos cuando una categoria esta en el lote

	// inicializamos cat_existe
	for (i = 0; i < MAXCATEGORIAS; i++) {
		cat_existe[i] = 0; //comienza como categoria no encontrada
	}

	//revisamos cada restriccion
	// 1: El precio de cada lote no puede superar una cantidad indicada por el usuario 
	precio_total = 0;
	for (i = 0; i < TAMLOTE; i++) {
		precio_total += tabla_prods[v[i]].precio;
	}
	if (precio_total > maxprecio)
		return 0;	//lote se pasa de precio
	// 2: No puede haber más de dos productos de la misma marca en el lote
	for (i = 0; i < TAMLOTE; i++) {
		for (j = 0; j < TAMLOTE; j++) {
			if (i == j)
				continue;	// es el mismo ignoramos este caso 
			if (strcmp (tabla_prods[v[i]].marca, tabla_prods[v[j]].marca) == 0) {
				return 0; // dos productos con la misma marca: rechazado
			}
		}
	}

	// 3: Debe haber al menos un producto de cada categoría en el lote 
	// procedemos a guardar los indices
	for (i = 0; i < TAMLOTE; i++) {
		j = buscar_categoria (tabla_prods[v[i]].categoria); //esto es lo que devuelve el indice
		if (j == -1) {
			printf ("Error fatal:buscar_categoria () retornó -1 sobre categoria existente\n");
			exit (1);
		}
		cat_existe[j] = 1; // la categoria esta en el lote
	}
	//ya tenemos cuales estan
	for (i = 0; i < cant_categorias; i++) {
		if (!cat_existe[i]) {
			return 0;	//la categoria n oesta en el lote rechazada
		}
	}

	return 1; // lote aceptado
}

/* Verificamos y Guardamos si hemos visitado este nodo 
 * Como esta es una rutina critica tratamos de hacerla eficiente
 * guardando en un array ordenado los arrays que vamos visitando.
 * Hacemos una búsqueda binaria para que sea lo más rápido posible.
 */
int visitado (int v[])
{
	int		i;
	int		dec;
	int		codigo;

	/* la idea es detectar y guardar si hemos visitado o no este array 
	 * construimos un número código que representa a este array
	 * para ello ordenamos el array de forma que no importa como vengan los indices al ordenarlos
	 * siempre va a dar el mismo ordenamiento y por consiguiente el mismo código
	 */

	insertion_sort (v, TAMLOTE);	/* v queda ordenado */

	/* construimos el código 
	 * dec nos permite ubicar cada indice separado sin afecta al de al lado 
	 * cada indice va a quedar así
	 *  dd  dd  dd  dd  dd
	 *  [0] [1] [2] [3] [4]
	 * asi que el array 1,2,3,4,5 se va a convertir en 102030405
	 */
	codigo = 0;
	dec = 100000000;
	for (i = 0; i < TAMLOTE;i++) {
		codigo += v[i] * dec;
		dec /= 100;	// convertimos dec en el siguiente desplazamiento 
	}
	if (busqueda_binaria (codigo))
		return 1;	//este nodo ya ha sido visitado
	// no ha sido visitado asi que lo marcamos
	visitados[numero_visitados] = codigo;	// almacenamos como ultimo 
	numero_visitados++;	// + 1 
	//como la busqueda es binaria el array tiene que estar ordenado 
	insertion_sort (visitados, numero_visitados);	//ordenacion 
	return 0; // devolvemos 0 indicando que el nodo no ha sido visitado
}

int busqueda_binaria (int codigo)
{
	int primero = 0;
	int ultimo = numero_visitados - 1;
	int medio  = (primero + ultimo) / 2;
 
	while (primero <= ultimo) {
		if (visitados[medio] < codigo)
			primero = medio + 1;    
		else if (visitados[medio] == codigo) {
			return 1;
		} else {
			ultimo = medio - 1;
		}
		medio = (primero + ultimo) / 2;
	}
	return 0;
}


 /* Insertion sort es el mejor algoritmo para data casi ordenada
 * La data en este array es casi ordenada porque estamos ordenando luego de cada inserción
 * Parametros: array[] array de numeros a ser ordenado
 *				cantidad		cuantos numeros hay en el array
 */
void insertion_sort (int array[], int cantidad)
{
	int temp;
	int i, j;

	for (i = 1; i < cantidad; i++) {
        for (j = i; j >= 1; j--) {
			// ordenamos en forma descendiente  así 2 5 1 va a quedar 5 2 1 
			if (array[j] == array[j-1]) 
				continue;
			if (array[j] < array[j-1]) {
                temp = array[j];
                array[j] = array[j-1];
                array[j-1] = temp;
            }  else {
                break;
			}
        }
    }
}

// devuelve el indice de la  categoria se está en el array de categorias, -1 en caso contrario 
int	buscar_categoria (char* categoria)
{
	int i;

	for (i = 0; i < cant_categorias;i++) {
		if (strcmp (categorias[i], categoria) == 0) {
			return i; //ya esta guardada
		}
	}
	return -1;	// la categoria no se encontro en el array
}

// guarda la cadena de caracteres en un espacio de memoria nuevo 
char *strsave (char *nombre)
{
    char *str;

    if ((str = (char *) malloc (strlen (nombre) + 1)) == NULL) {
		printf ("Se acabo el espacio de memoria\n");
		return NULL;
    }
    strcpy (str, nombre);
    return str;
}


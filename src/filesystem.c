/*	Autores							NIA				Grupo
	JAVIER PRIETO CEPEDA:			100 307 011		82
	SERGIO RUIZ JIMÉNEZ:			100 303 582		81
	MARIN LUCIAN PRIALA:			100 303 625		83
 */

	/* This file contains the definition of the functions that must be implemented
	 * to allow the user access to the file system and the files.
	 */

	#define ANSI_COLOR_RESET   "\x1b[0m"
	#define ANSI_COLOR_RED     "\x1b[31m"
	#define ANSI_COLOR_GREEN   "\x1b[32m"

	#include "include/filesystem.h"
	#include <strings.h>
	#include <string.h>

	/***************************/
	/* File system management. */
	/***************************/

	/*----------DEFINE----------*/
	#define MAX_FILES 50					/*Maximo numero de ficheros en el sistema de ficheros*/
	#define MAX_NAME_FILE 64				/*Longitud maxima del nombre de cada fichero*/
	#define MAX_TAGS 30						/*Maximo numero de etiquetas para el sistema de ficheros*/
	#define MAX_NAME_TAG 32					/*Longitud maxima del nombre de la etiqueta*/
	#define MAX_TAGS_FILE 3					/*Numero maximo de etiquetas que puede tener un fichero*/
	#define MIN_SIZE_DEVICE 327680			/*Capacidad minima en Bytes que puede tener el disco*/
	#define MAX_SIZE_DEVICE 512000			/*Capacidad maxima en Bytes que puede tener el disco*/
	#define NIA 303625						/*Numero magico, ultimos 6 digitos del nia de uno de los integrantes del grupo*/
	#define NUM_BLOCK_SUPERBLOQUE 0			/*Numero de bloque del superbloque*/
	#define NUM_BLOCK_INODOS 1				/*Numero de bloque del bloque de inodos*/
	#define DESPLAZAMIENTO_BLOQUE_DIRECTO 2	/*El despalazamiento donde comienza los bloques directos*/


	typedef struct {
		unsigned int 	nMagico;									/*Los ultimos 6 digitos de nuestro NIA*/
		unsigned int	sizeDispositivo;							/*El tamaño del dispositivo en bytes*/
		char			nombreEtiquetas[MAX_TAGS][MAX_NAME_TAG];	/*Nombre de las etiquetas que se han creado*/
		unsigned char	mapaEtiquetas[MAX_TAGS][MAX_FILES];			/*Mapa que por cada etiqueta tendra 1 si el fichero tiene esa etiqueta y 0 si dicha posicion (inodo) no la tiene*/
		unsigned char	mapaEtiquetasLibres[MAX_TAGS];				/*Las etiquetas libres 1 --> en uso, 0--> libre*/
		unsigned char	nInodosBloques;								/*Numero de i-nodos (por tanto bloques) que tiene el sistema. Como maximo 50*/
		unsigned char	iMap;										/*Primer inodo libre*/
	} superbloque;													/*+++++2500 bytes ocupados.+++++*/

	typedef struct {
		unsigned short	sizeFichero;								/*Size del fichero, maximo 4096 bytes*/
		char 			nombreFichero[MAX_NAME_FILE];				/*Nombre del fichero, debe ser unico*/
		unsigned char 	bloqueDirecto;								/*Bloque de datos de este fichero*/
		unsigned char	etiquetasAsignadas;							/*Numero de etiquetas asignadas a este fichero, maximo 3*/
	} inodo;														/*+++++68 bytes ocupados. 4096/68=60 inodos caben en un bloque+++++*/

	typedef struct {
		unsigned short	posicion;									/*Posicion del puntero dle fichero*/
		unsigned char	abierto;									/*Si el fichero esta abierto o no. Abierto --> 1, Cerrado --> 0*/
	} inodo_mem;

	superbloque mySuperbloque;
	inodo myInodo[MAX_FILES];
	inodo_mem myInodo_mem[MAX_FILES];

	int esta_montado; // 1--> si esta montado, 0--> si no lo esta

	/*
	 * Formats a device.
	 * Returns 0 if the operation was correct or -1 in case of error.
	 */
	int mkFS(int maxNumFiles, long deviceSize) {
		// La capacidad del dispositivo debe estar entre el MIN y MAX establecidos
		if(deviceSize < MIN_SIZE_DEVICE || deviceSize > MAX_SIZE_DEVICE){
			return -1;
		}

		// El numero de ficeros debe estar entre 0 y MAX
		if(maxNumFiles < 0 || maxNumFiles > MAX_FILES){
			return -1;
		}

		char buffer[BLOCK_SIZE];	// Buffer que contendra la estructura a escribir
		bzero(buffer,BLOCK_SIZE);	// Rellenamos de ceros el buffer

		/* Escribimos 0 en el bloque del indodo por si queda basura
		 * y no Escribimos nada en los datos porque si los metadatos dicen
		 * que no hay datos pues no los hay
		 */
		int result = bwrite(DEVICE_IMAGE, NUM_BLOCK_INODOS, buffer);

		// Si el numero de bytes escritos es negativo o mayor del bloque error
		if(result < 0 || result > sizeof(buffer)){
			return -1;
		}

		// Rellenamos el superbloque con los parametros del sistema de ficheros
		mySuperbloque.nMagico = NIA;
		mySuperbloque.sizeDispositivo = deviceSize;

		// Inicialmente no tiene etiquetas
		bzero(&mySuperbloque.nombreEtiquetas, sizeof(mySuperbloque.nombreEtiquetas));
		bzero(&mySuperbloque.mapaEtiquetas, sizeof(mySuperbloque.mapaEtiquetas));

		// Asignamos el numero maximo de ficheros que tendra el sistema segun la peticion del usuario
		mySuperbloque.nInodosBloques = maxNumFiles;

		// Rellenamos que todas las etiquetas estna libres para poder usarse
		int i;
		for(i = 0; i < MAX_TAGS; i++){
			mySuperbloque.mapaEtiquetasLibres[i] = 0;
		}
		mySuperbloque.iMap = 0; // Indicamos que el primer inodo/bloque libre es el 0

		// Transformamos de superbloque a char * para poder escribir en el disco
		memcpy(buffer, &mySuperbloque, sizeof(mySuperbloque));

		// Escribimos en el disco los metadatos del superbloque
		result = bwrite(DEVICE_IMAGE, NUM_BLOCK_SUPERBLOQUE, buffer);

		// Si el numero de bytes escritos es negativo o mayor del bloque error
		if(result < 0 || result > sizeof(buffer)){
			return -1;
		}

		esta_montado = 0; // No montamos el sistema de ficheros, sino que lo creamos

		// En el resto de casos significa que todo ha ido bien
		return 0;
	}

	/*
	 * Mounts a file system from the device deviceName.
	 * Returns 0 if the operation was correct or -1 in case of error.
	 */
	int mountFS() {

		char buffer[BLOCK_SIZE];	// Buffer del superbloque
		char bufferInodo[BLOCK_SIZE];

		// Si esta montado el sistema de ficheros devolvemos -1, error.
		if(esta_montado == 1){
			return -1;
		}

		// Limpiamos las estructuras de superbloque y la de inodo por si hay basura de ejecuciones anteriores
		bzero(&mySuperbloque, sizeof(mySuperbloque));
		bzero(&myInodo, sizeof(mySuperbloque));

		// Leemos el superbloque de disco
		int result = bread(DEVICE_IMAGE, NUM_BLOCK_SUPERBLOQUE, buffer);

		// En caso de lo leer un bloque entonces error devolvemos -1
		if(result < 0 || result > BLOCK_SIZE) {
			return -1;
		}

		// Leemos el bloque de inodos
		result = bread(DEVICE_IMAGE, NUM_BLOCK_INODOS, bufferInodo);

		// En caso de error devolvemos -1
		if(result < 0 || result > BLOCK_SIZE) {
			return -1;
		}

		// Convertimos de char * a superbloque
		memcpy(&mySuperbloque, buffer, sizeof(mySuperbloque));

		// Copiamos todos los inodos en el array de inodos de memoria
		char auxBuffer[sizeof(inodo)];
		int i;	// Para iterar sobre los inodos
		int sizeInodoBytes = sizeof(inodo);
		int j = 0;	// Para iterar sobre el buffer auxiliar creado
		int k = 0;	// Para iterar sobre los inodos de memoria

		// Iteramos byte a byte hasta el primer inodo libre, es decir, hasta (incluido) el ultimo inodo ocupado
		for(i = 0; i < (mySuperbloque.iMap)*sizeInodoBytes; i++, j++){
			auxBuffer[j] = bufferInodo[i];

			/* Cuando llega al byte donde acaba el primer inodo copiamos todo el buffer a la estructura inodo
			 * es sizeInodoBytes-1 porque j comienza en 0 y el size en 1
			 */
			if(j == (sizeInodoBytes-1)){
				memcpy(&myInodo[k], auxBuffer, sizeInodoBytes);

				// Reseteamos j porque queremos reescribir el buffer reaprovechandolo
				j = 0;
				k++;
			}
		}

		// Consideramos que si el nMagio esta bien entonces los demas campos tambien
		if(mySuperbloque.nMagico != NIA){
				return -1;
		}
		esta_montado = 1; // Actualizamos la variable indicando que el sistema de ficheros se ha montado
		return 0;
	}

	/*
	 * Unmount file system.
	 * Returns 0 if the operation was correct or -1 in case of error.
	 */
	int umountFS() {
		char buffer[BLOCK_SIZE];
		char bufferInodo[BLOCK_SIZE];
		bzero(buffer,BLOCK_SIZE);	// Rellenamos de ceros el buffer
		bzero(bufferInodo,BLOCK_SIZE);	// Rellenamos de ceros el bufferInodo

		// Si esta desmontado error.
		if(esta_montado == 0){
			return -1;
		}

		// Comprobamos si hay ficheros abiertos que se han creado en el sistema de ficheros
		int i;
		for(i = 0; i < mySuperbloque.iMap; i++){
			// Si esta abierto el fichero entonces -1, error
			if(myInodo_mem[i].abierto == 1){
				return -1;
			}
		}

		// Convertimos de superbloque a char para escribir en el disco
		memcpy(buffer, &mySuperbloque, sizeof(mySuperbloque));
		memcpy(bufferInodo, &myInodo, sizeof(myInodo));

		// Si no escribe correctamente entonces devolvemos -1, error.
		int result = bwrite(DEVICE_IMAGE, NUM_BLOCK_SUPERBLOQUE, buffer);
		if(result < 0 || result > BLOCK_SIZE){
			return -1;
		}

		result = bwrite(DEVICE_IMAGE, NUM_BLOCK_INODOS, bufferInodo);
		if(result < 0 || result > BLOCK_SIZE){
			return -1;
		}

		esta_montado = 0;	// Indicamos que el sistema de ficheros no esta montado
		// Borramos los metadatos de memoria
		bzero(&myInodo,sizeof(myInodo));
		bzero(&mySuperbloque,sizeof(mySuperbloque));

		// Retornamos 0 en caso de exito
		return 0;
	}

	/*******************/
	/* File read/write */
	/*******************/

	/*
	 * Creates a new file, if it doesn't exist.
	 * Returns 0 if a new file is created, 1 if the file already exists or -1 in
	 * case of error.
	 */
	int creatFS(char *fileName) {

		// Si esta desmontado error.
		if(esta_montado == 0){
			return -1;
		}

		// Si la longitud del nombre del fichero excede el maximo entonces -1, error
		if(strlen(fileName) <= 0 || (strlen(fileName)+1) > MAX_NAME_FILE){
			return -1;
		}

		/* Si no se puede crear mas ficheros en el sistema de ficheros entonces error
		 * Si el primer inodo libre es el MAX_FILES (50 en nuestro caso) entonces
		 * significa que todos los inodos estan ocupados
		 *
		 */
		if(mySuperbloque.iMap >= mySuperbloque.nInodosBloques){
			return -1;
		}

		// Si el primero es 0 entonces esta vacio porque no se puede borrar
		if(mySuperbloque.iMap == 0){
			myInodo[0].sizeFichero = 0;
			memcpy(myInodo[0].nombreFichero, fileName, strlen(fileName));
			myInodo[0].bloqueDirecto = DESPLAZAMIENTO_BLOQUE_DIRECTO;
			myInodo[0].etiquetasAsignadas = 0;
			mySuperbloque.iMap = 1;	// El inodo 0 ya esta ocupado por tanto el primero libre es el 1

			/* En caso de no estar libre la primera posicion, miramos todas las creadas
			 * comprobamos que el fichero por su nombre no existe, y si no existe, le asignamos
			 * un nuevo inodo, el primero libre
			 * actualizamos todos los metadatos, y los guardamos en memoria
			 */
		} else {
			int i;
			for(i = 0; i < mySuperbloque.iMap; i++){
				// Si el nombre del fichero existe devolvemos 1
				if(strcmp(myInodo[i].nombreFichero, fileName) == 0){

					return 1;
				}
			}
			// Como el iMap nos devuelve el primer indodo LIBRE, en ese inodo creamos el fichero
			myInodo[mySuperbloque.iMap].sizeFichero = 0;
			memcpy(myInodo[mySuperbloque.iMap].nombreFichero, fileName, strlen(fileName));
			myInodo[mySuperbloque.iMap].bloqueDirecto = DESPLAZAMIENTO_BLOQUE_DIRECTO + mySuperbloque.iMap;
			myInodo[mySuperbloque.iMap].etiquetasAsignadas = 0;
			mySuperbloque.iMap++; // Ocupamos el inodo ya creado, por tanto el primero libre es el siguiente
		}

		// En caso de exito devolvemos 0
		return 0;
	}

	/*
	 * Opens an existing file.
	 * Returns file descriptor if possible, -1 if file does not exist or -1 in case
	 * of error.
	 */
	int openFS(char *fileName) {
		/* Buscamos en los inodos de memoria, el que se llame igual
		 * en caso de no existir, devolvemos error -1
		 * si existe, actualizamos y decimos que esta abierto
		 * si esta abierto -2
		 */

		// Si esta desmontado error.
	 	if(esta_montado == 0){
	 		return -2;
	 	}

		int i;
		for(i=0;i < mySuperbloque.iMap; i++){
				if(strcmp(myInodo[i].nombreFichero, fileName) == 0){	// Son iguales
					if(myInodo_mem[i].abierto == 1){
						return -2;	// Si el fichero ya esta abierto entonces -2
					}
					myInodo_mem[i].abierto = 1;	//Indicamos que el fichero esta abierto
					myInodo_mem[i].posicion = 0;	// Reseteamos la posicion del puntero de posicion
					return myInodo[i].bloqueDirecto;	// Devolvemos el descriptor del fichero que es el bloque directo de datos
				}
		}
		return -1;	// Si el fichero no existe entonces -1
	}

	/*
	 * Closes a file.
	 * Returns 0 if the operation was correct or -1 in case of error.
	 */
	int closeFS(int fileDescriptor) {
		// Si esta desmontado error.
	 	if(esta_montado == 0){
	 		return -1;
	 	}

		// Si esta abierto entonces lo cerramos y devolvemos 0, exito. De lo contrario -1, error
		if(myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].abierto == 1){
			myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].abierto = 0;
			return 0;
		} else{
			return -1;
		}
	}

	/*
	 * Reads a number of bytes from a file and stores them in a buffer.
	 * Returns the number of bytes read or -1 in case of error.
	 */
	int readFS(int fileDescriptor, void *buffer, int numBytes) {

		// Si esta desmontado error.
	 	if(esta_montado == 0){
	 		return -1;
	 	}

		char bufferRead[BLOCK_SIZE];
		int offset;
		int bytesALeer;

		// Si el fichero no esta abierto entonces error
		if(myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].abierto == 0){
			return -1;
		}

		// Si el puntero de posicion esta al final del fichero entonces no llemos nada
		if(myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion == BLOCK_SIZE){
			return 0;
		}

		/* Como el descriptor del fichero es el numero del bloque que corresponde
		 * al dato de dicho fichero leemos directamente el fichero. Si la lectura es
		 * erronea entonces -1, error
		 */
		int result = bread(DEVICE_IMAGE, fileDescriptor, bufferRead);
		if(result < 0){
			return -1;
		}

		offset = myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion;

		/* Si el numero de bytes a leer es mayor que el numero de bytes restantes
		 * entonces solo leemos has que se pueda leer. De lo contrario leemos el
		 * el numero de bytes pedidos
		 */

		if(numBytes > myInodo[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].sizeFichero-offset) {
			bytesALeer = myInodo[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].sizeFichero - offset;
		} else {
			bytesALeer = numBytes;
		}
		memcpy(buffer, bufferRead+offset, bytesALeer);

		// Actualizamos el puntero de posicion
		myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion = myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion + bytesALeer;

		// Devolvemos el numero de bytes que se nos pide leer
		return bytesALeer;
	}

	/*
	 * Reads number of bytes from a buffer and writes them in a file.
	 * Returns the number of bytes written, 0 in case of end of file or -1 in case
	 * of error.
	 */
	int writeFS(int fileDescriptor, void *buffer, int numBytes) {

		// Si esta desmontado error o Si el fichero esta cerrado, error
	 	if(esta_montado == 0 || myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].abierto == 0){
	 		return -1;
	 	}

		char bufferDato[BLOCK_SIZE];
		char aux[numBytes];

		unsigned short pos = myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion;

		// Si el puntero esta al final del fichero no escribimos nada y devolvemos 0 bytes escritos
		if(pos == (BLOCK_SIZE-1)){
			return 0;
		}

		// Si el size a escribir excede el size del fichero (bloque en nuestro caso) -1, error
		if(numBytes > BLOCK_SIZE || numBytes < 0){
			return -1;
		}

		int restante = BLOCK_SIZE - myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion;

		// Si el size a escribir excede el size del fichero des de la posicion actual, error
		if(numBytes > restante){
			return -1;
		}

		// Copiamos los datos que debemos escribir a un char * auxiliar
		memcpy(aux, buffer, numBytes);

		// Primero leemos el bloque de datos para escribir solamente los bytes deseados
		int result;
		result = bread(DEVICE_IMAGE, fileDescriptor, bufferDato);

		if(result < 0){
		 return -1;
		}

		// Escribimos en el bloque leido de disco solo los datos que queremos escribir
		memcpy(bufferDato+pos,aux, numBytes);

/*
		int k;
		for(k = 0; k < numBytes; k++){
			bufferDato[pos] = aux[k];
			pos++;
		}
*/
		result = bwrite(DEVICE_IMAGE, fileDescriptor, bufferDato);
		if(result < 0){
			return -1;
		}

		// Actualizamos la posicion del puntero
		myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion = myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion + numBytes -1;

		/* Actualizamos el size del fichero, como cada posicion puede
		 * representar un byte, nos sirve. Porque el fichero puede crecer
		 * hasta el maximo del size del bloque
		 */
		if(myInodo[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].sizeFichero < myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion){
			myInodo[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].sizeFichero = myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion+1;
		}
		// Devolvemos el numero de bytes escritos
	 	return numBytes;
	}


	/*
	 * Repositions the pointer of a file. A greater offset than the current size, or
	 * an offset lower than 0 are considered errors.
	 * Returns new position or -1 in case of error.
	 */
	 int lseekFS(int fileDescriptor, long offset, int whence) {

		// Si esta desmontado error.
		if(esta_montado == 0){
			return -1;
		}

		if(myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].abierto == 0){
			return -1;
		}
		unsigned short aux;
		switch(whence){
			case FS_SEEK_SET:
				// Si el ofset no esta en el rango establecido o pos actual + offset entonces -1, error
				if(offset < 0 ||
					offset >= myInodo[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].sizeFichero ||
						myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion + offset >= BLOCK_SIZE){
					return -1;
				}
				aux = (unsigned short) offset;
				myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion = aux + myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion;
			break;
			case FS_SEEK_BEGIN:
				// Posicionamos el puntero al inicio del fichero
				aux = 0;
				myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion = aux;
			break;
			case FS_SEEK_END:
				// Posicionamos el puntero al final del fichero
				aux = (myInodo[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].sizeFichero-1);
				myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion = aux;
			break;

			default:
				// Si el tipo de desplazamiento es distinto a los tres posibles casos entonces -1, error
				return -1;
		}
		return myInodo_mem[fileDescriptor-DESPLAZAMIENTO_BLOQUE_DIRECTO].posicion;
	}

	/**********************/
	/* Version management */
	/**********************/

	/*
	 * Tags a file with the given tag name. Returns 0 if the operation is
	 * successful, 1 if the file already had that tag or -1 in case of error.
	 */
	int tagFS(char *fileName, char *tagName) {

		// Si esta desmontado error.
		if(esta_montado == 0){
			return -1;
		}

		int i,k;
		int existeFichero = 0; // 0-->False, no existe el fichero
		int posicionFichero;

		// Si la longitud del nombre de la etiqueta sobrepasa el maximo permitido  entonces -1, error
		if(strlen(tagName) > MAX_NAME_TAG){
			return -1;
		}

		// Buscamos el fichero, en los indodos ocupados, cuando lo encontramos entonces nos salimos
		for(k = 0; k < mySuperbloque.iMap && existeFichero == 0; k++){
			if(strcmp(myInodo[k].nombreFichero, fileName) == 0){
				posicionFichero = k; // Guardamos el inodo del fichero, su posicion
				existeFichero = 1; // 1--> True, el fichero existe
				// Si el fichero esta abierto entonces error, tambien si tiene 3 o mas etiquetas asociadas
				if(myInodo_mem[k].abierto == 1){
					return -1;
				}
			}
		}

		// Si el fichero no existe entonces error
		if(existeFichero == 0){
			return -1;
		}

		// Buscamos la etiqueta
		for(i = 0; i < MAX_TAGS; i++){
			// La etiqueta existe
			if(strcmp(tagName, mySuperbloque.nombreEtiquetas[i]) == 0){

				// Si el fichero tiene esa etiqueta asociada entonces devolvemos 1
				if(mySuperbloque.mapaEtiquetas[i][posicionFichero] == 1){
					return 1;
				}

				// Como la etiqueta existe entonces la asociamos a dicho fichero
				if(myInodo[posicionFichero].etiquetasAsignadas < MAX_TAGS_FILE){
					mySuperbloque.mapaEtiquetas[i][posicionFichero] = 1;
					myInodo[posicionFichero].etiquetasAsignadas++;

					// Devolvemos 0, exito
					return 0;
				} else {
					// El fichero ya no puede tener mas etiquetas asignadas, devolvemos -1, error
					return -1;
				}
			}
		}

		// Si llega aqui significa que no existe la etiqueta
		for(i = 0; i < MAX_TAGS; i++){
			// Si la encontramos un hueco, creamos la etiqueta
			if(myInodo[posicionFichero].etiquetasAsignadas < MAX_TAGS_FILE && mySuperbloque.mapaEtiquetasLibres[i] == 0){
				memcpy(mySuperbloque.nombreEtiquetas[i], tagName, strlen(tagName));	//Creamos la etiqueta
				myInodo[posicionFichero].etiquetasAsignadas++;	// Aumentamos el numero de etiquetas que tiene asignadas el fichero
				mySuperbloque.mapaEtiquetasLibres[i] = 1;	// Ocupamos la etiqueta en el mapa de etiquetas
				mySuperbloque.mapaEtiquetas[i][posicionFichero] = 1; // Asociamos la etiqueta a dicho fichero

				// Devolvemos 0, exito
				return 0;
			}
		}

		// Devolvemos -1, error
		return -1;
	}

	/*
	 * Removes a tag from a file. Returns 0 if the operation is successful, 1 if
	 * the tag wasn't associated to the file or -1 in case of error.
	 */
	int untagFS(char *fileName, char *tagName) {

		// Si esta desmontado error.
		if(esta_montado == 0){
			return -1;
		}

		int i,k;
		int posEtiqueta = -1;
		int encontrado = 0;
		int existeFichero = 0;
		int countLibre = 0; // Contamos el numero de ceros en el mapa de etiquetas

		// Buscamos la etiqueta
		for(i = 0; i < MAX_TAGS && posEtiqueta == -1; i++){
			// Comparamos el nombre solo si la etiqueta esta en uso.
			if(mySuperbloque.mapaEtiquetasLibres[i] == 1){
				if(strcmp(tagName, mySuperbloque.nombreEtiquetas[i]) == 0){
					posEtiqueta = i; // Guardamos la posicion de la etiqueta
				}
			}
		}
		// La etiqueta no existe
		if(posEtiqueta == -1){
			return -1;
		}


		/* Recorremos los ficheros que tengan dicha etiqueta
		 * Recorremos hasta el primero libre, en caso de haber max ficheros, entonces iMap valdra MAX_FILES
		 * porque va desde 0 hasta MAX_FILES-1, pero al ocuparse el ultimo, se sumara iMap y valdra MAX_FILES
		 * por tanto countLibre en el por de los casos llegara a valer MAX_FILES
		 */
		for(k = 0; k < mySuperbloque.iMap; k++){
			/* Buscamos el fichero en el sistema de ficheros hasta el primer inodo libre
			 * si esta, entonces buscamos si esta asociado a la etiqueta
			 */
			if(strcmp(myInodo[k].nombreFichero,fileName) != 0 && encontrado == 0){
				existeFichero = -1;

			} else if(strcmp(myInodo[k].nombreFichero,fileName) == 0){
				existeFichero = 1;
				encontrado = 2;
			}
			// Contamos el numero de ficheros no asociados a la etiqueta
			if(mySuperbloque.mapaEtiquetas[posEtiqueta][k] == 1){
				// Si el fichero esta asociado a la etiqueta lo quitamos
				if(strcmp(myInodo[k].nombreFichero,fileName) == 0){

					// Si esta abierto entonces error
					if(myInodo_mem[k].abierto == 1){
						return -1;
					}
					myInodo[k].etiquetasAsignadas--;
					mySuperbloque.mapaEtiquetas[posEtiqueta][k] = 0;
					countLibre++; // Hemos puesto una pos a 0 del mapa
					encontrado = 1;
					existeFichero = encontrado;
				}
			} else{
				countLibre++;
			}
		}

		// SI el fichero no existe entonces -1, si existe pero no esta asociado entonces 1
		if(existeFichero == -1){
			return -1;
		} else if(encontrado == 2){
			return 1;
		}

		/* Si countLibre vale el numero de ficheros que puede tener el sistema
		 * de ficheros, o el el numero de ficheros creados, aunque no sea el
		 * maximo, entonces eliminamos la etiqueta, porque sabemos que lo demas
		 * estara a 0
		 */
		if(countLibre == mySuperbloque.nInodosBloques || countLibre == mySuperbloque.iMap){
			// Borrar la etiqueta
			bzero(&mySuperbloque.nombreEtiquetas[posEtiqueta], MAX_NAME_TAG);
			mySuperbloque.mapaEtiquetasLibres[posEtiqueta] = 0;

			return 0;
		}

		// Si la etiqueta estaba asociada y eliminada, entonces 0, exito
		return 0;
	}

	/*
	 * Looks for all files tagged with the tag tagName, and stores them in the list
	 * passed as a parameter. Returns the number of tagged files found or -1 in
	 * case of error.
	 */
	int listFS(char *tagName, char **files) {

		// Si esta desmontado error.
		if(esta_montado == 0){
			return -1;
		}

		int i,k,j;
		j = 0;
		for(i = 0; i < MAX_TAGS; i++){
			// Solo miramos el nombre de la etiqueta si esta en uso
			if(mySuperbloque.mapaEtiquetasLibres[i] == 1){
				// Buscamos por etiqueta, hasta encontrarla
				if(strcmp(tagName, mySuperbloque.nombreEtiquetas[i]) == 0){
					// Recorremos todos los inodos hasta el primero libre
					for(k = 0; k < mySuperbloque.iMap; k++){
						// Si esta asociado el fichero a la etiqueta entonces guardamos el nombre de dicho fichero
						if(mySuperbloque.mapaEtiquetas[i][k] == 1){
							files[j] = myInodo[k].nombreFichero;
							j++; // Incrementamos el numero de ficheros encontrados para esta etiqueta
						}
					}
					// Devolvemos el numero de ficheros encontrados para dicha etiqueta
					return j;
				}
			}
		}
		// Si no hay etiquetas en el FS o no encuentra la etiqueta buscada devolvemos -1
		return -1;
	}

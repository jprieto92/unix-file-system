/*	Autores							NIA				Grupo
	JAVIER PRIETO CEPEDA:			100 307 011		82
	SERGIO RUIZ JIMÉNEZ:			100 303 582		81
	MARIN LUCIAN PRIALA:			100 303 625		83
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/filesystem.h"

#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"

/*----------DEFINE----------*/
#define MAX_FILES 50					/*Maximo numero de ficheros en el sistema de ficheros*/
#define MIN_SIZE_DEVICE 327680			/*Capacidad minima en Bytes que puede tener el disco*/
#define MAX_SIZE_DEVICE 512000			/*Capacidad maxima en Bytes que puede tener el disco*/


int main() {
	int ret, i, fd;
	char buffer1[BLOCK_SIZE];
	char buffer2[BLOCK_SIZE];
	char buffer3[BLOCK_SIZE];

	/*
	 * LAS PRUEBAS PROPORCIONADAS ESTAN COMENTADAS, EN CASO DE QUERER PROBARLAS HAY QUE DESCOMENTARLAS,
	 * CABE DESTACAR QUE NO SE HAN MODIFICADO LAS PRUEBAS PROPORCIONADAS
	 */

/*
	fprintf(stdout, "%s", "TEST mkFS\n");

	ret = mkFS(50, 204800);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST mountFS\n");

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST creatFS\n");

	ret = creatFS("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST creatFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST openFS + closeFS\n");

	fd = openFS("test.txt");
	if(fd < 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;
	}
	ret = closeFS(fd);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST openFS + closeFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS\n");

	fd = openFS("test.txt");
	if(fd < 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;
	}
	ret = readFS(fd, buffer1, BLOCK_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;
	}
	memset(buffer2, 't', BLOCK_SIZE);
	ret = writeFS(fd, buffer2, BLOCK_SIZE);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS\n");
		return -1;
	}
	ret = lseekFS(fd, 0, FS_SEEK_SET);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;
	}
	ret = readFS(fd, buffer3, BLOCK_SIZE);
	if(ret != BLOCK_SIZE) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #2\n");
		return -1;
	}
	for(i = 0; i < BLOCK_SIZE; ++i) {
		if(buffer3[i] != 't') {
			fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #2\n");
			return -1;
		}
	}
	ret = closeFS(fd);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST openFS + readFS + writeFS + lseekFS + closeFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST tagFS + listFS + untagFS\n");

	ret = tagFS("test.txt", "sample");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS\n");
		return -1;
	}
	char **files = (char**) malloc(50*sizeof(char*));
	for(i = 0; i < 50; ++i) {
		files[i] = (char*) malloc(64*sizeof(char));
	}
	ret = listFS("sample", files);
	if(ret != 1 || strcmp(files[0], "test.txt") != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS\n");
		return -1;
	}
	ret = untagFS("test.txt", "sample");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS\n");
		return -1;
	}
	ret = listFS("sample", files);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS #2\n");
		return -1;
	}
	fprintf(stdout, "%s%s%s%s", "TEST tagFS + listFS + untagFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	fprintf(stdout, "%s", "TEST umountFS\n");


	ret = umountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "TEST umountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "TEST umountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

*/
	/************************************************* Prueba 3: el mountFS *************************************************/
	// No hay sistema de ficheros creado, el disco esta a 0 todo. En este paso es necesario esta condicion
	ret = mountFS();
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "Prueba 3 mountFS + mkFS + mountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS\n");
		return -1;
	}

	ret = mkFS(MAX_FILES, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "Prueba 3 mountFS + mkFS + mountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mkFS\n");
		return -1;
	}

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "Prueba 3 mountFS + mkFS + mountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS\n");
		return -1;
	}

	ret = mountFS();
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "Prueba 3 mkFS + mountFS + mountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS #2\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 3 mkFS + mountFS + mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);



	/************************************************* Prueba 1: el mkFS los casos limites disco *************************************************/
	ret = mkFS(MAX_FILES, MIN_SIZE_DEVICE-1);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 1 mkFS  MIN_SIZE_DEVICE-1 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(MAX_FILES, MIN_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 1 mkFS  MIN_SIZE_DEVICE ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(MAX_FILES, 400000);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 1 mkFS  MIN_SIZE_DEVICE ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(MAX_FILES, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 1 mkFS  MAX_SIZE_DEVICE ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(MAX_FILES, MAX_SIZE_DEVICE+1);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 1 mkFS  MAX_SIZE_DEVICE+1 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 1 mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);



	/************************************************* Prueba 2: el mkFS los casos limites ficheros *************************************************/
	ret = mkFS(-1, MAX_SIZE_DEVICE);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 2 mkFS  -1 ficheros ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(0, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 2 mkFS  0 ficheros ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(25, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 2 mkFS  25 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(MAX_FILES, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 2 mkFS  MAX_FILES ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = mkFS(MAX_FILES+1, MAX_SIZE_DEVICE);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 2 mkFS  MAX_FILES+1 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 2 mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);



	/************************************************* Prueba 4: el umountFS *************************************************/

	ret = umountFS();
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 4 umountFS + mountFS + umountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at umountFS\n");
		return -1;
	}

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 4 umountFS + mountFS + umountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS\n");
		return -1;
	}

	ret = umountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 4 umountFS + mountFS + umountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at umountFS #2\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 4 umountFS + mountFS + umountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);



	/************************************************* Prueba 5: mkFS + mountFS + umountFS *************************************************/

	ret = mkFS(25, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 5 mkFS + mountFS + umountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mkFS\n");
		return -1;
	}

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 5 mkFS + mountFS + umountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS\n");
		return -1;
	}

	ret = umountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 5 mkFS + mountFS + umountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at umountFS\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 5 mkFS + mountFS + umountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);



	/************************************************* Prueba 6: creacion de ficheros *************************************************/
	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 6 mountFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS\n");
		return -1;
	}

	ret = creatFS("");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 6 creatFS length 0 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	ret = creatFS("12345678901234567890123456789012345678901234567890123456789.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 6 creatFS  64 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = creatFS("1234567890123456789012345678901234567890123456789012345678901.txt");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 6 creatFS  65 ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	ret = creatFS("12345678901234567890123456789012345678901234567890123456789.txt");
	if(ret != 1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 6 creatFS repeat ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	int ff = 0;
	char nombre[25-1][20];
	char *uuu = "abcdefghijklmnopqrstuvwyz123456789123456789123456798132465798";
	for (ff = 0; ff < 25-1; ff++) {
		memcpy(nombre[ff],uuu,ff+1);
		ret = creatFS(nombre[ff]);
		if(ret != 0) {
			fprintf(stdout, "%s%d%s%s%s", "TEST creatFS ",ff, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
	}

	ret = creatFS("extra.txt");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 6 creatFS extra file ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 6 mountFS + createFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);



	/************************************************* Prueba 7: Cierre de ficheros *************************************************/
	int fd3 = openFS("no_existe.txt");
	if(fd3 != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 7 openFS inexistente ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS inexistente\n");
		return -1;
	}

	int fd1 = openFS("12345678901234567890123456789012345678901234567890123456789.txt");
	if(fd1 == -1 || fd1 == -2) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 7 openFS existente cerrado ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS existente cerrado\n");
		return -1;
	}

	fd = openFS("12345678901234567890123456789012345678901234567890123456789.txt");
	if(fd != -2) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 7 openFS existente abierto ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS existente abierto\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 7 openFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);



	/************************************************* Prueba 8: Cierre de ficheros *************************************************/
	ret = closeFS(fd1);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 8 closeFS fichero existente abierto ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	ret = umountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 8 umountFS + mkFS + mountFS + openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at umountFS\n");
		return -1;
	}

	ret = mkFS(25, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 8 umountFS + mkFS + mountFS + openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mkFS\n");
		return -1;
	}

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 8 umountFS + mkFS + mountFS + openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS\n");
		return -1;
	}

	ret = creatFS("extra.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s", "PRUEBA 8 creatFS extra file ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = closeFS(5);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 8 closeFS fichero inexistente ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	ret = closeFS(0);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 8 closeFS fichero existente cerrado ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 8 closeFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);


	/************************************************* Prueba 9: Wrote, Read y Lseek *************************************************/

	int myfd1 = openFS("extra.txt");
	if(myfd1 == -1 || fd1 == -2) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 openFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS\n");
		return -1;
	}

	memset(buffer2, 't', BLOCK_SIZE);
	ret = writeFS(myfd1, buffer2, BLOCK_SIZE);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 writeFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS\n");
	}

	ret = closeFS(myfd1);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 closeFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS\n");
		return -1;
	}

	myfd1 = openFS("extra.txt");
	if(myfd1 == -1 || fd1 == -2) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 openFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS #2\n");
		return -1;
	}

	ret = readFS(myfd1, buffer3, BLOCK_SIZE);
	if(ret < 0 && strncmp(buffer2,buffer3, BLOCK_SIZE) != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 writeFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS\n");
		return -1;
	}

	ret = lseekFS(myfd1, 0, FS_SEEK_BEGIN);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 lseekFS begin extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS\n");
		return -1;
	}

	memset(buffer2, 'z', BLOCK_SIZE);
	ret = writeFS(myfd1, buffer2, BLOCK_SIZE/2);
	if(ret < 0 && ret != BLOCK_SIZE/2) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 writeFS extras z", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS #2\n");
		return -1;
	}

	ret = lseekFS(myfd1, 0, FS_SEEK_BEGIN);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 lseekFS begin extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS #2\n");
		return -1;
	}

	ret = lseekFS(myfd1, BLOCK_SIZE/4, FS_SEEK_SET);
	if(ret != BLOCK_SIZE/4) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 lseekFS 1/4 extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS #3\n");
		return -1;
	}

	ret = readFS(myfd1, buffer1, BLOCK_SIZE);
	int aux =0;
	int aux2 = 0;
	for (i = 0; i < BLOCK_SIZE - BLOCK_SIZE/4; i++) {
		if(buffer1[i] == 'z'){
			aux++;
		} else if(buffer1[i] == 't'){
			aux2++;
		}
	}

	if(ret < 0 || ret != (BLOCK_SIZE - BLOCK_SIZE/4) || aux != BLOCK_SIZE/4 || aux2 != (BLOCK_SIZE/4)*2) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 readFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #2\n");
		return -1;
	}

	ret = closeFS(myfd1);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 closeFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS #2\n");
		return -1;
	}

	ret = readFS(myfd1, buffer1, BLOCK_SIZE);
	if(ret != -1 ) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 readFS extras despues del close ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at readFS #3\n");
		return -1;
	}

	ret = writeFS(myfd1, buffer1, BLOCK_SIZE);
	if(ret != -1 ) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 writeFS extras despues del close ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS #4\n");
		return -1;
	}

	myfd1 = openFS("extra.txt");
	if(myfd1 == -1 || fd1 == -2) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 openFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at openFS #2\n");
		return -1;
	}

	ret = lseekFS(myfd1, BLOCK_SIZE/4, FS_SEEK_END);
	if(ret != BLOCK_SIZE-1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 lseekFS 1/4 extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS #4\n");
		return -1;
	}

	ret = lseekFS(myfd1, BLOCK_SIZE/4, FS_SEEK_BEGIN);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 lseekFS 1/4 extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS #4\n");
		return -1;
	}

	ret = lseekFS(myfd1, BLOCK_SIZE/4, FS_SEEK_SET);
	if(ret != BLOCK_SIZE/4) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 lseekFS 1/4 extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at lseekFS #4\n");
		return -1;
	}

	ret = writeFS(myfd1, buffer1, (BLOCK_SIZE/4)*3+100);
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 9 writeFS extras puntero + offset  > BLOCK_SIZE ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at writeFS #3\n");
		return -1;
	}

	fprintf(stdout, "%s%s%s%s", "PRUEBA 9 writeFS + readFS + lseekFS begin + writeFS/2 lseekFS begin + lseekFS/4 + readFS 3/4 + readFS +closeFS + readFS + writeFS + writeFS offset ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);


	/************************************************* Prueba 10: tagFS *************************************************/
	ret = closeFS(myfd1);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 closeFS extras ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at closeFS #2\n");
		return -1;
	}

	ret = umountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 umountFS + mkFS + mountFS + openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at umountFS\n");
		return -1;
	}

	ret = mkFS(35, MAX_SIZE_DEVICE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 umountFS + mkFS + mountFS + openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mkFS\n");
		return -1;
	}

	ret = mountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 umountFS + mkFS + mountFS + openFS + closeFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at mountFS\n");
		return -1;
	}

	ret = tagFS("test.txt", "sample");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS\n");
		return -1;
	}

	ret = creatFS("myTEST.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%d%s%s%s", "PRUEBA 10 creatFS ",ff, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = creatFS("myTEST2.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%d%s%s%s", "PRUEBA 10 creatFS ",ff, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = creatFS("myTEST3.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%d%s%s%s", "PRUEBA 10 creatFS ",ff, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}

	ret = tagFS("myTEST.txt", "sample");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #2\n");
		return -1;
	}

	ret = tagFS("myTEST.txt", "sample");
	if(ret != 1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #3\n");
		return -1;
	}

	{
		char **files = (char**) malloc(50*sizeof(char*));
		for(i = 0; i < 50; ++i) {
			files[i] = (char*) malloc(64*sizeof(char));
		}
		ret = listFS("sample", files);
		if(ret != 1 || strcmp(files[0],"myTEST.txt") != 0) {
			fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS primer list\n");
			return -1;
		}
	}

	ret = tagFS("myTEST2.txt", "sample");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #2\n");
		return -1;
	}

	ret = tagFS("myTEST.txt", "sample2");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #2\n");
		return -1;
	}

	ret = tagFS("myTEST.txt", "sample3");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #2\n");
		return -1;
	}

	ret = tagFS("myTEST.txt", "sample4");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #2\n");
		return -1;
	}

	{
		char **files = (char**) malloc(50*sizeof(char*));
		for(i = 0; i < 50; ++i) {
			files[i] = (char*) malloc(64*sizeof(char));
		}
		ret = listFS("sample2", files);
		if(ret != 1 || strcmp(files[0],"myTEST.txt") != 0) {
			fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS primer list #2\n");
			return -1;
		}
	}
	{
		char **files = (char**) malloc(50*sizeof(char*));
		for(i = 0; i < 50; ++i) {
			files[i] = (char*) malloc(64*sizeof(char));
		}
		ret = listFS("sample3", files);
		if(ret != 1 || strcmp(files[0],"myTEST.txt") != 0) {
			fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS primer list #4\n");
			return -1;
		}
	}

	ret = untagFS("myTEST.txt", "sample");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS as dasd asd asd \n");
		return -1;
	}
	ret = untagFS("myTEST.txt", "sample2");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS as dasd asd asd \n");
		return -1;
	}
	ret = untagFS("myTEST.txt", "sample3");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS as dasd asd asd \n");
		return -1;
	}

	{
		char **files = (char**) malloc(50*sizeof(char*));
		for(i = 0; i < 50; ++i) {
			files[i] = (char*) malloc(64*sizeof(char));
		}
		ret = listFS("sample2", files);
		if(ret != -1) {
			fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS primer list #5\n");
			return -1;
		}
	}
	{
		char **files = (char**) malloc(50*sizeof(char*));
		for(i = 0; i < 50; ++i) {
			files[i] = (char*) malloc(64*sizeof(char));
		}
		ret = listFS("sample3", files);
		if(ret != -1) {
			fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS primer list #6\n");
			return -1;
		}
	}

	{
		char **files = (char**) malloc(50*sizeof(char*));
		for(i = 0; i < 50; ++i) {
			files[i] = (char*) malloc(64*sizeof(char));
		}
		ret = listFS("sample", files);
		if(ret != 1 || strcmp(files[0],"myTEST2.txt") != 0) {
			fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS primer list #7\n");
			return -1;
		}
	}

	ret = untagFS("myTEST3.txt", "sample");
	if(ret != 1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS as dasd asd asd \n");
		return -1;
	}

	ret = untagFS("myTEST.txt", "sample3d");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS as dasd asd asd \n");
		return -1;
	}


	ff = 0;
	char nombre3[35-3][20];
	char nombre2[32];

	char *uuu3 = "abcdefghijklmnopqrstuvwyz123456789123456789123456798132465798";
	for (ff = 0; ff < 35-3; ff++) {
		memcpy(nombre3[ff],uuu3,ff+1);
		memcpy(nombre2,uuu3,ff+1);
		ret = creatFS(nombre3[ff]);
		if(ret != 0) {
			fprintf(stdout, "%s%d%s%s%s", "PRUEBA 10 creatFS files ",ff, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
		ret = tagFS(nombre3[ff],nombre2);
		if(ret != 0 && (ff < 29 && ret != -1)) {
			fprintf(stdout, "%s%d%s%s%s", "PRUEBA 10 tagFS ",ff, ANSI_COLOR_RED, " FAILED\n", ANSI_COLOR_RESET);
			return -1;
		}
		{
			char **files = (char**) malloc(50*sizeof(char*));
			for(i = 0; i < 50; ++i) {
				files[i] = (char*) malloc(64*sizeof(char));
			}
			ret = listFS(nombre2, files);
			if(ret != 1 && (ff < 29 && ret != -1)) {
				fprintf(stdout, "%s%d%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ",ff, ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at listFS primer list #7\n");
				return -1;
			}
		}
	}

	ret = tagFS("a.txt", "sample43");
	if(ret != -1) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #2\n");
		return -1;
	}

	ret = untagFS("a", "a");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS + listFS + untagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at untagFS as dasd asd asd \n");
		return -1;
	}

	ret = tagFS("myTEST.txt", "sample43");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", "PRUEBA 10 tagFS ", ANSI_COLOR_RED, "FAILED ", ANSI_COLOR_RESET, "at tagFS #9\n");
		return -1;
	}


	fprintf(stdout, "%s%s%s%s", "PRUEBA 10 tagFS + untagFS + listFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	return 0;
}

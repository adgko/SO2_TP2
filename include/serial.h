#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
//#include <libnet.h>
#include <time.h>
#include <omp.h>
#include "./simple_bmp.h"			// path relativo porque está en la misma carpeta

#define ARCHIVO_ORIGINAL "../img/base.bmp"
#define ARCHIVO_EDITADO "../img/edited.bmp"
#define SIZE_K 41

/*
	Variables empleadas para imprimir en colores
*/
#define KNRM  					"\x1B[0m"	//normal
#define KRED  					"\x1B[31m"	//rojo
#define KGRN  					"\x1B[32m"	//verde
#define KYEL  					"\x1B[33m"	//amarillo
#define KBLU  					"\x1B[34m"	//azul
#define KMAG  					"\x1B[35m"	//magenta
#define KCYN  					"\x1B[36m"	//cyan
#define KWHT  					"\x1B[37m"	//blanco

// declaración de funciones
void escalar(sbmp_image *una_imagen,sbmp_image *otra_imagen, int32_t x, int32_t y,int contraste, int brillo);
void validar_colores(int32_t *x, int32_t *y, int32_t *z);
void validar_color(int32_t *color);
void convolucion(sbmp_image *una_imagen,sbmp_image *otra_imagen, int32_t x, int32_t y, int32_t altura, int32_t anchura, uint16_t **matriz_kernel,int sum);
void trabajar_pixel(sbmp_image *una_imagen,sbmp_image *otra_imagen, int32_t x, int32_t y, uint16_t **matriz_kernel,int suma);
void crear_matriz(uint16_t **kernel);
int kernel_setup (uint16_t **kern, int16_t ksize);
#include "../include/serial.h"

int main(int32_t arg, char *argv[]){ // entra Radio, Contraste K y Brillo L

	// imagen archivo a trabajar
	sbmp_image imagen;
	sbmp_image imagen_final;
	sbmp_load_bmp(ARCHIVO_ORIGINAL, &imagen);
	sbmp_load_bmp(ARCHIVO_ORIGINAL, &imagen_final);
	// declaración de variables a usar
	int32_t alto = imagen.info.image_height;
	int32_t ancho = imagen.info.image_width;
	int32_t radio = atoi(argv[1]);
	int K = atoi(argv[2]);
	int L = atoi(argv[3]);
	int32_t centro[] = {alto/2, ancho/2};
	uint16_t **kernel;
	int sumatoria = 0;

	// para saber cuanto dura la función
	double start, end;
    double cpu_time_used;
    start = omp_get_wtime();

    printf("%sComienza el procesamiento de %s %s\n",KBLU,ARCHIVO_ORIGINAL,KNRM );
    printf("%sRadio: %d %s\n",KBLU,radio,KNRM );
    printf("%scontraste: %d %s\n",KBLU,K,KNRM );
    printf("%sbrillo: %d %s\n",KBLU,L,KNRM );

	// alocando memoria para la matriz
	kernel = calloc (SIZE_K, sizeof (int *));
	for (int k = 0; k < SIZE_K; k++){
		kernel[k] = calloc (SIZE_K, sizeof (uint16_t));
	}
	sumatoria = kernel_setup (kernel, SIZE_K);


	//recorrido de la matriz
	for(int32_t i=0; i<alto; i++){
		for(int32_t j=0; j<ancho; j++){
			// cálculo del centro
			if(((i-centro[0])*(i-centro[0])+(j-centro[1])*(j-centro[1])) <= radio*radio){
				// si es la zona A, hace p(x,y)*K+L
				escalar(&imagen,&imagen_final,i,j,K,L);						
			}
			else{
				// si es la zona B, realiza la convolución
				convolucion(&imagen,&imagen_final,i,j,alto,ancho,kernel,sumatoria);
			}
		}
	}
	// almacena la nueva imagen
	sbmp_save_bmp(ARCHIVO_EDITADO, &imagen_final);

	// para calcular cuanto dura
	end = omp_get_wtime();
    cpu_time_used = ((double) (end - start));
	int h,m,s;
	h = (int)(cpu_time_used/3600);
	m = (int)(cpu_time_used -(3600*h))/60;
	s = (int)(cpu_time_used -(3600*h)-(m*60));
	printf("%sProcesamiento Completado%s\n",KGRN,KNRM );
	printf("%sLa operación duró %d horas, %d minutos y %d segundos %s\n", KGRN, h, m, s, KNRM);

	return 0;


}

/* multiplica el pixel por el contraste y le agrega brillo
   parámetros: *una_imagen es la dirección de la imagen que trabajo
   				x e y son las coordenadas del pixel
*/
void escalar(sbmp_image *una_imagen,sbmp_image *otra_imagen, int32_t x, int32_t y,int contraste,int brillo){				//le estoy pasando la imagen por referencia
	int32_t red_aux = una_imagen->data[x][y].red*contraste+brillo;
	int32_t blue_aux = una_imagen->data[x][y].blue*contraste+brillo;
	int32_t green_aux = una_imagen->data[x][y].green*contraste+brillo;

	validar_colores(&red_aux, &blue_aux, &green_aux);

	otra_imagen->data[x][y].red = (uint8_t)red_aux;
	otra_imagen->data[x][y].blue = (uint8_t)blue_aux;
	otra_imagen->data[x][y].green = (uint8_t)green_aux;
}


/* llama 3 veces a validar color, con los distintos colores
	rojo, verde y azul
	parámetros: *a,*b y *c son la dirección de los colores
*/
void validar_colores(int32_t *a, int32_t *b, int32_t *c){
	validar_color(a);
	validar_color(b);
	validar_color(c);
}

/* validar color se encarga de que los colores no se saturen
	parámetro: *color es la dirección del color a validar
*/
void validar_color(int32_t *color){
	if(*color > 255){
		*color = 255;
	}
	else if(*color < 0){
		*color = 0;
	}
}



/* se encarga de hacer la convolución de cada pixel
	parámetros: *una_imagen es la dirección de la imagen que trabajo
   				x e y son las coordenadas del pixel
				matriz_kernel es la matriz con la que se hace la convolución
				y suma es el valor sumado de los valores de la matriz para normalizar
*/
void trabajar_pixel(sbmp_image *una_imagen,sbmp_image *otra_imagen, int32_t x, int32_t y, uint16_t **matriz_kernel,int suma){

	int32_t red_tmp = 0;
	int32_t blue_tmp = 0;
	int32_t green_tmp = 0;
    int m = 0;
    int n = 0;
	

	// recorro la matriz al rededor del pixel y la conovluciono con kernel
    for(int i=x-(SIZE_K-1)/2;i<x+(SIZE_K-1)/2;i++){
        for(int j=y-(SIZE_K-1)/2;j<y+(SIZE_K-1)/2;j++){
            int aux = matriz_kernel[n][m];
            red_tmp += una_imagen->data[i][j].red*aux;
			blue_tmp += una_imagen->data[i][j].blue*aux;
			green_tmp += una_imagen->data[i][j].green*aux;
            m++;
			//printf("%d %d %d %d\n",n,m, i, j);
        }
        n++;
    	m = 0;
	} 
	// normalizo con los valores de sumatoria sacados de la matriz
	int32_t com_red = red_tmp/suma;
	int32_t com_blue = blue_tmp/suma;
	int32_t com_green = green_tmp/suma;

	// se valida el color para que no sature
	validar_colores(&com_red, &com_blue, &com_green);

	// modifica la imagen trabajada
	otra_imagen->data[x][y].red = (uint8_t)com_red;
	otra_imagen->data[x][y].blue = (uint8_t)com_blue;
	otra_imagen->data[x][y].green = (uint8_t)com_green;

}

/* Recorre la imagen y si no está en los bordes, modifica el pixel
	parámetros: la imagen a trabajar, als coordenadas del pixel, 
	y el alto y ancho de la imagen.
*/
void convolucion(sbmp_image *una_imagen,sbmp_image *otra_imagen, int32_t x, 
     int32_t y, int32_t altura, int32_t anchura, uint16_t **matriz_kernel, int sum){

	// revisa que no esté en los pixeles de los bordes
	if(x-(SIZE_K-1)/2>0 && y-(SIZE_K-1)/2>0 && x+1+(SIZE_K-1)/2<altura && y+1+(SIZE_K-1)/2<anchura){
		trabajar_pixel(una_imagen,otra_imagen,x,y,matriz_kernel,sum);
	}
} 
/* La función que hace la matriz de kernel para la convolución
	**kern es una dirección donde se guardará la matriz armada
	ksize es el tamaño de las filas y las columnas del kernel
*/
int kernel_setup (uint16_t **kern, int16_t ksize)
{
  uint16_t st_val = 1;
  int sum = 0;
  for (int j = 0; j < ksize; j++)
          kern[0][j] = st_val;
		  sum += st_val;

  for (int i = 1; i < ksize / 2 + 1; i++)
    {
      for (int j = 0; j < ksize; j++)
        {
          if (j >= i && j < (ksize - i)){
            kern[i][j] = (uint16_t) (kern[i - 1][j] + (uint16_t) 1);
			sum += (uint16_t) (kern[i - 1][j] + (uint16_t) 1);
		  }
          else{
            kern[i][j] = kern[i - 1][j];
			sum += kern[i - 1][j];
		  }
        }

    }
  for (int i = 1; i < ksize / 2; i++)
    {
      for (int j = 0; j < ksize; j++)
        {
          kern[i + ksize / 2][j] = kern[ksize / 2 - i][j];
		  sum += kern[ksize / 2 - i][j];
        }

    }
  for (int j = 0; j < ksize; j++){
          kern[ksize-1][j] = st_val;
		  sum += st_val;
  }

	return sum;
}


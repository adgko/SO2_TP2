# TP2_SO2
Trabajo práctico realizaba para entender los conceptos de paralelismo en lenguaje C

Emplea librería OpenMP y procesamiento de imagen BMP

## Compilación

Todo lo necesario es el programa programa_paralelo_01.c, simple_bmp.c y simple_bmp.h.Al hacer make compile se consigue el bin y con make docs se obtiene la documentación de Doxygen.

## Ejecución

./parallel Radio Contraste Brillo
  
  A la imagen procesada, se la separa en dos areas, un area delimitada por el radio en la que se aumenta su brillo, y un area fuera del círculo en la que se le aplica un filtro blur, logrado mediante una convolución

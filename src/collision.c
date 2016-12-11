#include <gb/gb.h>
#include "collision.h"

/**
Contiene los metodos necesarios para, dados un vector de una unica dimension, compobrar si se colisiona
a partir de una posicion y un tama�o.

unsigned char *levelCollision Es el array de una dimension que contiene el mapa, cada valor representa un
tile de 8 pixeles.

De esta manera un nivel de 8x8 tiles donde 0x00 es vacio y 0x01 es objeto deberia de representarse de la siguiente
manera:
const unsigned char LEVEL1[] =
{
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
}
*/

UINT16 isCollisonDown(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision) {
    UINT16 indX; UINT16 indY; UINT16 indXY; UINT16 offsetX; UINT8 i;
    /*
    Si el personaje mide mas de un tile en ancho, se va recorriendo toda su anchura
    (de 8 en 8-> desplazar 3 bits hacia la izquierda equivale a dividir entre 8)
    y comprobando si colisiona.
    */
    i = 0;
    //for(i = 0; i < (w>>3); i++)
    while(i < (w>>3)+1)
    {
        offsetX = i * 8;
        if(i != 0){
            offsetX--;
        }
        i++;
        /*
        Convierte la posicion del player en index x/y
        (Matriz de dos dimensiones donde cada posicion equivale a la informacion en 8 pixeles)
        */
        indX = (x + offsetX) / 8;
        indY = (y + h -1) / 8;
        //Convierte los index x/y en xy
        indXY = (indY * mapSizeX) + indX;

        //Recupero el tile
        if(levelCollision[indXY] != 0x00) {
            return indXY;
        }
    }
    return 0;
}

UINT16 isCollisionUp(UINT16 x, UINT16 y, UINT16 w, UINT16 mapSizeX, unsigned char *levelCollision) {
    UINT16 indX;UINT16 indY;UINT16 indXY;UINT16 offsetX;UINT8 i;
    i = 0;
    //for(i = 0; i < (w>>3); i++)
    while(i < (w>>3)+1)
    {
        offsetX = i * 8;
        if(i != 0){
            offsetX--;
        }
        i++;

        //Convierte la posicion del player en index x/y
        indX = (x + offsetX) / 8;
        indY = y / 8;
        //Convierte los index x/y en xy
        indXY = (indY * mapSizeX) + indX;

        //Recupero el tile
        if(levelCollision[indXY] != 0x00) {
            return indXY;
        }

    }
    return 0;
}

UINT16 isCollisionRight(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision) {
    UINT16 indX;UINT16 indY;UINT16 indXY;UINT16 offsetY;UINT8 i;

    /*
    Si el personaje mide mas de un tile en alto, se va recorriendo toda su altura
    (de 8 en 8-> desplazar 3 bits hacia la izquierda equivale a dividir entre 8)
    y comprobando si colisiona.
    */
    i = 0;
    //for(i = 0; i < (h>>3); i++)
    while(i < (h>>3)+1)
    {
        offsetY = i * 8;
        if(i != 0){
            offsetY--;
        }
        i++;
        //Convierte la posicion del player en index x/y
        indX = (x + w-1) / 8;
        indY = (y + offsetY) / 8;
        //Convierte los index x/y en xy
        indXY = (indY * mapSizeX) + indX;

        //Recupero el tile
        if(levelCollision[indXY] != 0x00) {
            return indXY;
        }
    }
    return 0;
}

UINT16 isCollisionLeft(UINT16 x, UINT16 y, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision) {
    UINT16 indX; UINT16 indY; UINT16 indXY; UINT16 offsetY; UINT8 i;

    i = 0;
    //for(i = 0; i < (h>>3); i++)
    while(i < (h>>3)+1)
    {
        offsetY = i * 8;
        if(i != 0){
            offsetY--;
        }
        i++;
        //Convierte la posicion del player en index x/y
        indX = x /8;
         indY = (y + offsetY) / 8;
        //Convierte los index x/y en xy
        indXY = (indY * mapSizeX) + indX;

        //Recupero el tile
        if(levelCollision[indXY] != 0x00) {
            return indXY;
        }
    }
    return 0;
}

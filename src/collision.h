#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

//Tiles que no colisionan
#define FREE 0x00
#define NO_COL 0x66

//Tiles especiales
#define PLATFORM_TARGET 0x67

/**
Retorna el tile index(X,Y) del target que colisona por abajo
*/
UINT16 isCollisionDownT(
    UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision);

/**
Retorna el tile index(X,Y) del target que colisona por arriba
*/
UINT16 isCollisionUpT(
    UINT16 x, UINT16 y, UINT16 w, UINT16 mapSizeX, unsigned char *levelCollision);

UINT16 isCollisionDown(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision);

UINT16 isCollisionUp(UINT16 x, UINT16 y, UINT16 w, UINT16 mapSizeX, unsigned char *levelCollision);

UINT16 isCollisionRight(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision);

UINT16 isCollisionLeft(UINT16 x, UINT16 y, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision);

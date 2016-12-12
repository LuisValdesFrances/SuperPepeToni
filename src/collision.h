#ifndef COLLISION_H_INCLUDED
#define COLLISION_H_INCLUDED

UINT16 isCollisionDown(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision);

UINT16 isCollisionUp(UINT16 x, UINT16 y, UINT16 w, UINT16 mapSizeX, unsigned char *levelCollision);

UINT16 isCollisionRight(UINT16 x, UINT16 y, UINT16 w, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision);

UINT16 isCollisionLeft(UINT16 x, UINT16 y, UINT16 h, UINT16 mapSizeX, unsigned char *levelCollision);

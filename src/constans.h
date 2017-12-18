#ifndef CONSTANS_H_INCLUDED
#define CONSTANS_H_INCLUDED

#define TRUE 1
#define FALSE 0

#define PRECISION_BITS 4
#define INC_BITS <<PRECISION_BITS
#define DEC_BITS >>PRECISION_BITS

#define GRAVITY 6

//GAME STATES
#define GAME_STATE_MAIN_MENU 0
#define GAME_STATE_PLAY 1

//CHARACTERS
//Por motivos de optimizacion, nunca puede ser 0
#define MAX_GOCHI 1
#define MAX_POPO 1
#define MAX_BABIT 1
#define MAX_BULLET 1
//Payer
#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 32
#define PLAYER_WEIGHT 5
#define PLAYER_JUMP 3
#define PLAYER_SPEED 4 INC_BITS
//Enemies
#define POPO 0
#define GOCHI 1

#define PLATFORM_WIDTH 16
#define PLATFORM_HEIGHT 16
#define PLATFORM_SPEED 1//No se desplazan bits

#define POPO_WIDTH 8
#define POPO_HEIGHT 8
#define POPO_SPEED 18

#define GOCHI_WIDTH 16
#define GOCHI_HEIGHT 24
#define GOCHI_SPEED 12

#define BABIT_WIDTH 24
#define BABIT_HEIGHT 32

#define BULLET_WIDTH 8
#define BULLET_HEIGHT 8
#define BULLET_SPEED 40



#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_WIDTH2 (SCREEN_WIDTH >> 1)
#define SCREEN_HEIGHT2 (SCREEN_HEIGHT >> 1)
#define SCREEN_WIDTH4 (SCREEN_WIDTH >> 2)
#define SCREEN_HEIGHT4 (SCREEN_HEIGHT >> 2)
#define SCREEN_WIDTH8 (SCREEN_WIDTH >> 3)
#define SCREEN_HEIGHT8 (SCREEN_HEIGHT >> 3)

//Player states
#define STATE_UNACTIVE 0
#define STATE_IDLE 1
#define STATE_DOWN 2
#define STATE_RUN 3
#define STATE_ATACK 4

#define SUFF_COUNT 50

/**
Datos del mapa maps.c Los arrays estan implementados en maps.c
*/
#define MAP_SIZE_X 128
#define MAP_SIZE_Y 32
#define LEVEL_WIDTH (MAP_SIZE_X * 8)
#define LEVEL_HEIGHT (MAP_SIZE_Y * 8)

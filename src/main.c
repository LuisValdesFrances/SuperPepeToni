#include <stdio.h>
#include <gb/gb.h>
#include <gb/drawing.h>
#include "font.h"
#include "spriteTiles.h"
#include "mapTiles.h"
#include "collision.h"

#define TRUE 1
#define FALSE 0

#define PRECISION_BITS 4
#define INC_BITS <<PRECISION_BITS
#define DEC_BITS >>PRECISION_BITS

#define GRAVITY 6

//CHARACTERS
//Por motivos de optimizacion, nunca puede ser 0
#define MAX_GOCHI 2
#define MAX_POPO 1
//Payer
#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 32
#define PLAYER_WEIGHT 3
#define PLAYER_JUMP 2
#define PLAYER_SPEED 1 INC_BITS
//Enemies
#define POPO 0
#define GOCHI 1

#define POPO_WIDTH 8
#define POPO_HEIGHT 8
#define POPO_SPEED 18

#define GOCHI_WIDTH 16
#define GOCHI_HEIGHT 24
#define GOCHI_SPEED 12



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
#define STATE_RUN 2
#define STATE_ATACK 3

#define SUFF_COUNT 100

/**
Datos del mapa maps.c Los arrays estan implementados en maps.c
*/
#define MAP_SIZE_X 128
#define MAP_SIZE_Y 18
#define LEVEL_WIDTH (MAP_SIZE_X * 8)
#define LEVEL_HEIGHT (MAP_SIZE_Y * 8)

extern const unsigned char FONT_TILES[];//Informacion de los tiles (imagenes)
extern const unsigned char MAP_TILES[];//Informacion de los tiles (imagenes)
extern const unsigned char SPRITE_TILES[];//Informacion de los tiles (imagenes)
extern const unsigned char LEVEL1[];//Colisiones y distribucion


UBYTE keyPressUP(UBYTE key) {
    if( key & (J_UP)) {
        return 1;
    } else {
        return 0;
    }
}
UBYTE keyPressDOWN(UBYTE key) {
    if( key & (J_DOWN)) {
        return 1;
    } else {
        return 0;
    }
}
UBYTE keyPressLEFT(UBYTE key) {
    if( key & (J_LEFT)) {
        return 1;
    } else {
        return 0;
    }
}
UBYTE keyPressRIGHT(UBYTE key) {
    if( key & (J_RIGHT)) {
        return 1;
    } else {
        return 0;
    }
}
UBYTE keyPressA(UBYTE key) {
    if( key & (J_A)) {
        return 1;
    } else {
        return 0;
    }
}
UBYTE keyPressB(UBYTE key) {
    if( key & (J_B)) {
        return 1;
    } else {
        return 0;
    }
}
UBYTE keyPressSTART(UBYTE key) {
    if( key & (J_START)) {
        return 1;
    } else {
        return 0;
    }
}
UBYTE keyPressSELECT(UBYTE key) {

    if( key & (J_SELECT)) {
        return 1;
    } else {
        return 0;
    }
}


UINT8 getNumberDigits(UINT16 number, UINT8 c) {
    c++;
    if(number / 10 == 0) {
        return c;
    } else {
        number /= 10;
        return getNumberDigits(number, c);
    }
}
UINT8 getDigit(UINT16 number, UINT8 digit) {

    if(digit >= getNumberDigits(number, 0)) {
        return 0;
    } else {
        int d = number%10;
        if(digit == 0) {
            return d;
        } else {
            int n = number/10;
            digit--;
            getDigit(n, digit);
        }
    }
}
void drawPoints(UINT16 points, UINT8 digits, UINT8 positionX, UINT8 positionY) {

    UINT8 i;
    UINT8 digit;
    for(i = 0; i != digits; i++) {
        //UINT8 tile = points[i] - 48;//Cast chat to int
        digit = getDigit(points, (digits - (i+1)));
        set_sprite_tile(i, digit);
        move_sprite(i, positionX + (i << 3), positionY);
    }
}
void drawNumbers(UINT8 digits, UINT16 number, UINT8 positionX, UINT8 positionY) {

    UINT8 digit = (number % 10);

    set_sprite_tile(digits-1, digit);
    move_sprite(digits-1, positionX + ((digits-1) << 3), positionY);

    if(number / 10 == 0) {
        return;
    } else {
        digits--;
        drawNumbers(digits, number / 10, positionX, positionY);
    }
}


struct Player {
    UINT16 x;
    UBYTE y;
    UBYTE flip;
    UBYTE velocityDesc;
    UBYTE velocityAsc;
    UBYTE state;
    UBYTE frame;
    UBYTE suffCount;
};

struct Enemy {
    UINT16 x;
    UBYTE y;
    UBYTE flip;
    UBYTE frame;
    UBYTE expCount;
};

struct Camera {
    UINT16 scroll;
    UINT16 lastX;
};

UBYTE blink01(UBYTE value){
    if(value == FALSE){
        return TRUE;
    }else{
        return FALSE;
    }
}

UINT16 getScroll(UINT16 objX, UINT16 scrollX) {

    if(objX > SCREEN_WIDTH2 && objX > scrollX + SCREEN_WIDTH2){
        if(objX < LEVEL_WIDTH - SCREEN_WIDTH2){
            return objX - SCREEN_WIDTH2;
        }
        else{
            return LEVEL_WIDTH - SCREEN_WIDTH;
        }
    }else{
        return scrollX;
    }
}
UBYTE isInScreen(UINT16 scrollX, UINT16 x, UINT16 width){
    if(x < scrollX + SCREEN_WIDTH && x + width > scrollX){
        return TRUE;
    }else{
        return FALSE;
    }
}

UBYTE getFrameIdle(UBYTE gameFrame, UBYTE characterFrame) {
    UBYTE f =characterFrame;
    if(gameFrame%15 == 0) {
        if(characterFrame == 1){
            f = 0;
        }else{
            f = 1;
        }
    }
    return f;
}

UBYTE getFrameIdleFast(UBYTE gameFrame, UBYTE characterFrame) {
    UBYTE f =characterFrame;
    if(gameFrame%10 == 0) {
        if(characterFrame == 1){
            f = 0;
        }else{
            f = 1;
        }
    }
    return f;
}

UBYTE getPlayerFrameRun(UBYTE gameFrame, UBYTE playerFrame) {
    if(gameFrame%4 == 0) {
        playerFrame = (playerFrame +1)%3;
    }
    return playerFrame;
}

UBYTE checkCollision(UINT16 x1, UBYTE y1, UBYTE w1, UBYTE h1, UINT16 x2, UBYTE y2, UBYTE w2, UBYTE h2){
    if(x1 + w1 > x2 && x1 < x2 + w2){
        if(y1 + h1 > y2 && y1 < y2 + h2){
            return TRUE;
        }
    }
    return FALSE;
}

void flipPlayer(UBYTE flip){
    //Solo flipean cabeza y piernas
    set_sprite_prop(SPRITE_PLAYER_1, flip);
    set_sprite_prop(SPRITE_PLAYER_2, flip);
    set_sprite_prop(SPRITE_PLAYER_3, flip);
    set_sprite_prop(SPRITE_PLAYER_4, flip);
    set_sprite_prop(SPRITE_PLAYER_5, flip);
    set_sprite_prop(SPRITE_PLAYER_6, flip);
    set_sprite_prop(SPRITE_PLAYER_7, flip);
    set_sprite_prop(SPRITE_PLAYER_8, flip);

    set_sprite_prop(SPRITE_PLAYER_9, flip);
    set_sprite_prop(SPRITE_PLAYER_10, flip);
}

UINT8 getGravitySpeed(UINT16 gravityForce, UINT16 weight){
    UINT16 newGravityForce;
    UINT16 newWeight;
    /*
    Regla de tres
    (Las pruebas se hicieron con desplazamientos de 8 (3 bits), de esta manera, si se cambia esto, la fisica se adapta)
    8               = gravityForce
    PRECISION_BITS  = newGravityForce
    8               = weight
    PRECISION_BITS  = newWeight
    */
    newGravityForce = (PRECISION_BITS * gravityForce) >> 3;
    newWeight = (PRECISION_BITS * weight) >> 3;
    return newGravityForce * newWeight;
}

UINT8 getStrongJump(UINT16 strongJump){
    /*
    Regla de tres INVERSA
    (Las pruebas se hicieron con desplazamientos de 8 (3 bits), de esta manera, si se cambia esto, la fisica se adapta)
    8               = strongJump
    PRECISION_BITS  = newStrongJump
    */
    return (8 * strongJump) / PRECISION_BITS;
}

void checkPlayerDamage(struct Camera *camera, struct Player *player, struct Enemy *enemy, UBYTE enemyW, UBYTE enemyH){
    if(isInScreen((*camera).scroll, ((*enemy).x DEC_BITS), enemyW)){
        if((*enemy).expCount == 0){
            if((*player).suffCount == 0
            &&
            checkCollision(
                ((*player).x DEC_BITS), (*player).y, PLAYER_WIDTH, PLAYER_HEIGHT,
                ((*enemy).x DEC_BITS), (*enemy).y, enemyW, enemyH)){

                (*player).suffCount = SUFF_COUNT;
            }
         }
    }
}

void checkEnemyDamage(struct Camera *camera, struct Player *player, struct Enemy *enemy, UBYTE enemyW, UBYTE enemyH){
    UBYTE temp;
    if(isInScreen((*camera).scroll, ((*enemy).x DEC_BITS), enemyW)){
        if((*enemy).expCount == 0){
            if((*player).frame > 3 && (*player).frame < 9){

                temp = ((*player).x DEC_BITS) + PLAYER_WIDTH;
                if((*player).flip){
                    temp = ((*player).x DEC_BITS) - (PLAYER_WIDTH>>1);
                }
                if(
                    checkCollision(
                    temp, (*player).y, (PLAYER_WIDTH>>1), PLAYER_HEIGHT,
                    ((*enemy).x DEC_BITS), (*enemy).y, enemyW, enemyH)){

                        (*enemy).expCount = 1;
                }
            }
        }
    }
}

void moveEnemy(struct Camera *camera, struct Enemy *enemy, UBYTE enemyW, UBYTE enemyH, UBYTE speed, unsigned char *level){
    UINT16 newX;
    if(isInScreen((*camera).scroll, ((*enemy).x DEC_BITS), enemyW)){
        if((*enemy).expCount == 0){
            if((*enemy).flip){
                newX = (*enemy).x + speed;
            }else{
                newX = (*enemy).x - speed;
            }
            if(isCollisionLeft(newX DEC_BITS, (*enemy).y, enemyH, MAP_SIZE_X, level)){
                newX = (((isCollisionLeft(newX DEC_BITS, (*enemy).y, enemyH, MAP_SIZE_X, level) % MAP_SIZE_X) * 8)+8) INC_BITS;
                (*enemy).flip = blink01((*enemy).flip);
            }
            else if(isCollisionRight(newX DEC_BITS, (*enemy).y, enemyW, enemyH, MAP_SIZE_X, level)){
                newX = (((isCollisionRight(newX DEC_BITS, (*enemy).y, enemyW, enemyH, MAP_SIZE_X, level) % MAP_SIZE_X) * 8)-enemyW) INC_BITS;
                (*enemy).flip = blink01((*enemy).flip);
            }
            //No hay suelo
            if(isCollisionDown(newX DEC_BITS, (*enemy).y, enemyW, enemyH, MAP_SIZE_X, level)){
                (*enemy).flip = blink01((*enemy).flip);
            }
            (*enemy).x = newX;
        }
    }
}

void drawGochi(struct Camera *camera, struct Enemy *gochi, UBYTE count, UBYTE frame){

    UBYTE temp;
    UBYTE count2;
    temp = count*6;//6 es el numero de sprites
    if(isInScreen((*camera).scroll, ((*gochi).x DEC_BITS), GOCHI_WIDTH) && (*gochi).expCount != 4){
        if((*gochi).expCount == 0){
            set_sprite_tile(SPRITE_ENEMY_16X24_1+temp, TILE_GOCHI_1_F1);
            set_sprite_tile(SPRITE_ENEMY_16X24_2+temp, TILE_GOCHI_2_F1);
            set_sprite_tile(SPRITE_ENEMY_16X24_3+temp, TILE_GOCHI_3_F1 + ((*gochi).frame*4));
            set_sprite_tile(SPRITE_ENEMY_16X24_4+temp, TILE_GOCHI_4_F1 + ((*gochi).frame*4));
            set_sprite_tile(SPRITE_ENEMY_16X24_5+temp, TILE_GOCHI_5_F1 + ((*gochi).frame*4));
            set_sprite_tile(SPRITE_ENEMY_16X24_6+temp, TILE_GOCHI_6_F1 + ((*gochi).frame*4));

            (*gochi).frame = getFrameIdleFast(frame, (*gochi).frame);//OJO

            count2= 6;
            do{
                if((*gochi).flip){
                    set_sprite_prop(SPRITE_ENEMY_16X24_1 + temp + 6-count2, S_FLIPX);
                }else{
                    set_sprite_prop(SPRITE_ENEMY_16X24_1 + temp + 6-count2, 0);
                }
            }
            while(count2--);
        }else{
            set_sprite_tile(SPRITE_ENEMY_16X24_1+temp, TILE_EXP_1_F1 + (4*((*gochi).expCount-1)));
            set_sprite_tile(SPRITE_ENEMY_16X24_2+temp, TILE_EXP_2_F1 + (4*((*gochi).expCount-1)));
            set_sprite_tile(SPRITE_ENEMY_16X24_3+temp, TILE_EXP_3_F1 + (4*((*gochi).expCount-1)));
            set_sprite_tile(SPRITE_ENEMY_16X24_4+temp, TILE_EXP_4_F1 + (4*((*gochi).expCount-1)));
            set_sprite_tile(SPRITE_ENEMY_16X24_5+temp, TILE_BLANK);
            set_sprite_tile(SPRITE_ENEMY_16X24_6+temp, TILE_BLANK);
            if(frame%6==0){
                (*gochi).expCount++;
            }
        }
    }else{
        count2 = 5;
        do
        {
            set_sprite_tile(SPRITE_ENEMY_16X24_1 + count2 + temp, TILE_BLANK);
        }
        while(count2--);
    }
}

void moveSpriteGochi(struct Camera *camera, struct Enemy *gochi, UBYTE count){
    UBYTE temp;
    UBYTE temp2;
    temp = count*6;//6 es el numero de sprites
    if(isInScreen((*camera).scroll, ((*gochi).x DEC_BITS), GOCHI_WIDTH)){

        if((*gochi).flip){
            temp2 = 8;
        }else{
            temp2 = 0;
        }
        move_sprite(SPRITE_ENEMY_16X24_1+temp,((*gochi).x DEC_BITS) - (*camera).scroll +8 + temp2, ((*gochi).y) +16);
        move_sprite(SPRITE_ENEMY_16X24_2+temp,((*gochi).x DEC_BITS) - (*camera).scroll +16 - temp2, ((*gochi).y) +16);
        move_sprite(SPRITE_ENEMY_16X24_3+temp,((*gochi).x DEC_BITS) - (*camera).scroll +8 + temp2, ((*gochi).y) +24);
        move_sprite(SPRITE_ENEMY_16X24_4+temp,((*gochi).x DEC_BITS) - (*camera).scroll +16 - temp2, ((*gochi).y) +24);
        move_sprite(SPRITE_ENEMY_16X24_5+temp,((*gochi).x DEC_BITS) - (*camera).scroll +8 + temp2, ((*gochi).y) +32);
        move_sprite(SPRITE_ENEMY_16X24_6+temp,((*gochi).x DEC_BITS) - (*camera).scroll +16 - temp2, ((*gochi).y) +32);
    }
}

void drawPopo(struct Camera *camera, struct Enemy *popo, UBYTE count, UBYTE frame){
    UBYTE temp;
    temp = count*1;//1 es el numero de tiles
    if(isInScreen((*camera).scroll, ((*popo).x DEC_BITS), POPO_WIDTH)){
        set_sprite_tile(SPRITE_ENEMY_8X8_1 + temp, TILE_POPO_F1 + ((*popo).frame));
        (*popo).frame = getFrameIdle(frame, (*popo).frame);
        if((*popo).flip){
            set_sprite_prop(SPRITE_ENEMY_8X8_1 + temp, S_FLIPX);
        }else{
            set_sprite_prop(SPRITE_ENEMY_8X8_1 + temp, 0);
        }
    }else{
        set_sprite_tile(SPRITE_ENEMY_8X8_1 + temp, TILE_BLANK);
    }
}

void moveSpritePopo(struct Camera *camera, struct Enemy *popo, UBYTE count){
    UBYTE temp;
    temp = count*1;//1 es el numero de tiles
    if(isInScreen((*camera).scroll, ((*popo).x DEC_BITS), POPO_WIDTH) && (*popo).expCount == 0){
        move_sprite(SPRITE_ENEMY_8X8_1 + temp,((*popo).x DEC_BITS) - (*camera).scroll +8, ((*popo).y) +16);
    }
}

void main() {

    //Mierda temporal
    //printf("%d",3);
    //printf("Inicio del programa");

    //Declaracion
    UBYTE frame;
    UINT16 temp;
    UBYTE temp2;
    UBYTE count;
    UBYTE count2;

    UBYTE screenCountX;

    UBYTE isInGround;



    UBYTE keys;
    UBYTE keyB_Up;
    UBYTE keyB_Down;
    UBYTE keyA_Up;
    UBYTE keyA_Down;

    UINT16 newX;
    UBYTE newY;

    char digitPoints[5];
    struct Enemy gochiList[MAX_GOCHI];
    struct Enemy popoList[MAX_POPO];
    struct Player player;
    struct Camera camera;

    keyB_Up = TRUE;
    keyB_Down = FALSE;
    keyA_Up = TRUE;
    keyA_Down = FALSE;
    temp = 0;
    frame = 0;
    screenCountX = 0;

    digitPoints[0] = 0;
    digitPoints[1] = 0;
    digitPoints[2] = 0;
    digitPoints[3] = 0;
    digitPoints[4] = 0;

    DISPLAY_OFF;//Desactiva la pantalla
    disable_interrupts();
    HIDE_SPRITES;
    HIDE_BKG;
    HIDE_WIN;

    //Tiles del fondo/fuente
    SWITCH_ROM_MBC1(2);//Banco de memoria 2
    //posicion inicial, numero y tiles
    set_bkg_data(FONT_OFFSET, 64,(unsigned char *)FONT_TILES);
    set_bkg_data(0, TOTAL_MAP_TILES, (unsigned char *)MAP_TILES);


    //Sprites
    //Carga en la VRAM los tiles para los sprites
    SPRITES_8x8;
    SWITCH_ROM_MBC1(2);//Salto al banco de memoria 2
    //posicion de memoria, cantidad de tiles, tiles
    set_sprite_data(0, TOTAL_TILES, (unsigned char *)SPRITE_TILES);
    //Asigna a un sprite un tile
    //numero del sprite (0-39), posicion del tile
    set_sprite_tile(SPRITE_DIGIT_1, TILE_0);
    set_sprite_tile(SPRITE_DIGIT_2, TILE_0);
    set_sprite_tile(SPRITE_DIGIT_3, TILE_0);
    set_sprite_tile(SPRITE_DIGIT_4, TILE_0);

    //set_bkg_tiles(indexX(8 pixelex), indexY(8 pixelex), ancho, alto y mapa)
    //Viejo sistema, hasta 32 tiles me vale...
    //set_bkg_tiles(0, 0, MAP_SIZE_X, MAP_SIZE_Y, LEVEL1);


    /**
    El bucle rellena la pantalla de tiles de arriba a abajo (Se recorre en Y),
    haciendo una fila completa de 22 tiles (los que entran en la pantalla)
    Al final de cada iteracion mueve el puntero 64 tiles (El tamaño en X del mapa)
    para que en la siguiente iteracion apunte a la nueva fila de la matriz
    */

    SWITCH_ROM_MBC1(2);//Banco de memoria 4
    //Seria 32 pero solo quiero llenar la parte visible de la pantalla (144 pixels = 18 tiles)
    for(count = 0; count != 18; count++ ){//18 son los tiles que entran en la pantalla en alto

        //El mapa maximo en memoria es 32 x 32
        //22 son los tiles que entran en la pantalla en ancho (160 pixels = 20 tiles)
        set_bkg_tiles(0, count, 22, 1, &(LEVEL1+temp));

         //Incrementa temp con el tamaño X del mapa para que se cargue la proxima fila
		temp = temp + MAP_SIZE_X;

    }

    SHOW_BKG;//Muestra el fondo
    SHOW_SPRITES;//Muestra los sprites
    DISPLAY_ON;
    enable_interrupts();

    //Inicializacion del player y los enemigos
    player.x = (PLAYER_WIDTH>>1) INC_BITS;
    player.y = 0;
    player.flip = FALSE;
    newX= player.x;
    newY = player.y;
    player.state = STATE_IDLE;
    player.suffCount = 0;

    count = MAX_GOCHI-1;
    do{
        gochiList[count].y = (SCREEN_HEIGHT-GOCHI_HEIGHT-8);
        gochiList[count].frame = 0;
        gochiList[count].flip = FALSE;
        gochiList[count].x = 20 + (count * 40);
        gochiList[count].x = gochiList[count].x INC_BITS;
        gochiList[count].expCount = 0;
    }while(count--);
    count = MAX_POPO-1;
    do{
        popoList[count].y = (SCREEN_HEIGHT-POPO_HEIGHT-8);
        popoList[count].frame = 0;
        popoList[count].flip = FALSE;
        popoList[count].x = 30 + (count * 20);
        popoList[count].x = popoList[count].x INC_BITS;
        popoList[count].expCount = 0;
    }while(count--);


    while(TRUE) {

        /**
        Gestion del game pad
        */
        /**
        Sincronizacion con el blanqueo de la pantalla.

        Espera a que la pantalla esta lista para para pintarse (Se detiene la lógica de los calculos)
        En la GameBoy mientras se hace el blanqueo de la pantalla no se puede acceder a la memoria.
        De esta manera, una vez acaba el blanqueo de la memoria, se estable el pequeño tiempo en el que
        se puede acceder a la memoria de video
        */
        keys = joypad();//Lee el pad


        if(keyB_Up){//No se esta pulsando ninguna tecla
            if(keyPressB(keys)){
                keyB_Down = TRUE;//Entra evento down
                keyB_Up = FALSE;
            }
        }else{
            keyB_Down = FALSE;
            if(keyPressB(keys)){//En el frame anterior ya se pulsaba
                keyB_Up = FALSE;
            }else{
                keyB_Up = TRUE;
            }
        }
        if(keyA_Up){//No se esta pulsando ninguna tecla
            if(keyPressA(keys)){
                keyA_Down = TRUE;//Entra evento down
                keyA_Up = FALSE;
            }
        }else{
            keyA_Down = FALSE;
            if(keyPressA(keys)){//En el frame anterior ya se pulsaba
                keyA_Up = FALSE;
            }else{
                keyA_Up = TRUE;
            }
        }

        //Player controls
        newX = player.x;
        newY = player.y;

        /**
        Fisica del player
        */
        if(player.velocityAsc > 1 INC_BITS){
            player.velocityDesc = 0;
            newY -= (player.velocityAsc DEC_BITS);
            player.velocityAsc -= getGravitySpeed(GRAVITY, PLAYER_WEIGHT);
        }else{
            player.velocityAsc = 0;
            player.velocityDesc += getGravitySpeed(GRAVITY, PLAYER_WEIGHT);
            newY += (player.velocityDesc DEC_BITS);
        }
        isInGround = isCollisionDown(player.x DEC_BITS, player.y + 1, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1);


        /**
        Inputs
        */
        if(keyPressSTART(keys)){}
        if(keyPressSELECT(keys)){}
        if(keyPressUP(keys)) {
        }
        else if(keyPressDOWN(keys)) {
        }
        if(keyPressLEFT(keys)) {
            if(player.state == STATE_IDLE || player.state == STATE_RUN || !isInGround){
                player.flip = TRUE;
                player.state = STATE_RUN;
                newX -=  PLAYER_SPEED;
            }
        }
        else if(keyPressRIGHT(keys)) {
            if(player.state == STATE_IDLE || player.state == STATE_RUN || !isInGround){
                player.flip = FALSE;
                player.state = STATE_RUN;
                newX +=  PLAYER_SPEED;
            }
        }
        else{
            if(player.state != STATE_ATACK){
                player.state = STATE_IDLE;
            }
        }
        if(keyA_Down) {
            if(isInGround && player.state != STATE_ATACK){
                player.state = STATE_ATACK;
                player.frame = 0;
            }
        }
        if(keyB_Down) {
            if(isInGround && player.state != STATE_ATACK){
                player.velocityAsc = getStrongJump(PLAYER_JUMP INC_BITS);
            }

        }


        /**
        Colisiones player
        */
        if(isCollisionDown(player.x DEC_BITS, newY, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1)){
            newY = (((isCollisionDown(player.x DEC_BITS, newY, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1) / MAP_SIZE_X) << 3) - PLAYER_HEIGHT);
            player.velocityDesc = 0;
        }
        if(isCollisionUp(player.x DEC_BITS, newY, PLAYER_WIDTH, MAP_SIZE_X, LEVEL1)){
            player.velocityAsc = 0;//En el FK2 iba de lujo
            newY = (((isCollisionUp(player.x DEC_BITS, newY, PLAYER_WIDTH, MAP_SIZE_X, LEVEL1) / MAP_SIZE_X) << 3) + 8);
            player.velocityAsc = 0;
            player.velocityDesc = 0;
        }
        player.y = newY;
        if(isCollisionRight(newX DEC_BITS, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1)){
            newX = (((isCollisionRight(newX DEC_BITS, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1) % MAP_SIZE_X) << 3)-PLAYER_WIDTH) INC_BITS;
        }
        if(isCollisionLeft(newX DEC_BITS, player.y, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1)){
            newX = (((isCollisionLeft(newX DEC_BITS, player.y, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1) % MAP_SIZE_X) << 3)+8) INC_BITS;
        }
        player.x = newX;


        //Set limits screen
        if(player.x < (camera.scroll+1) INC_BITS){
            player.x = ((camera.scroll+1) INC_BITS);
        }
        else if((player.x DEC_BITS) + PLAYER_WIDTH >= LEVEL_WIDTH){
            player.x = (LEVEL_WIDTH - PLAYER_WIDTH) INC_BITS;
        }



        /**
        Colisiones enemigos
        */
        //Gochi
        count = MAX_GOCHI-1;
        do{
            moveEnemy(&camera, &gochiList[count], GOCHI_WIDTH, GOCHI_HEIGHT, GOCHI_SPEED, LEVEL1);
            checkPlayerDamage(&camera, &player, &gochiList[count], GOCHI_WIDTH, GOCHI_HEIGHT);
            checkEnemyDamage(&camera, &player, &gochiList[count], GOCHI_WIDTH, GOCHI_HEIGHT);
        }while(count--);

        //Popo
        count = MAX_POPO-1;
        do{
            moveEnemy(&camera, &popoList[count], POPO_WIDTH, POPO_HEIGHT, POPO_SPEED, LEVEL1);
            checkPlayerDamage(&camera, &player, &popoList[count], POPO_WIDTH, POPO_HEIGHT);
        }while(count--);


        wait_vbl_done();

        /**
        Pintado enemigos
        */
        //Gochi
        for(count = 0; count < MAX_GOCHI; count++){
            drawGochi(&camera, &gochiList[count], count, frame);
            moveSpriteGochi(&camera, &gochiList[count], count);
        }

        //Popo
        for(count = 0; count < MAX_POPO; count++){
            drawPopo(&camera, &popoList[count], count, frame);
            moveSpritePopo(&camera, &popoList[count], count);
        }

        /**
        Pintado Player
        */
        if(player.suffCount != 0){
            player.suffCount--;

            for(count = 0; count < 10; count++){
                set_sprite_tile(SPRITE_PLAYER_1+count, TILE_BLANK);
            }

        }
        if(player.suffCount%2 == 0){
        //Movimiento de los sprites //ANCHOR-> abajo derecha
        if(player.flip){
            flipPlayer(S_FLIPX);
            temp = 8;
        }else{
            flipPlayer(0);
            temp = 0;
        }
        temp2 = 0;
        //Ñapa para la animacion idle (Muevo algunos sprites un pixel)
        if(player.state == STATE_IDLE){
            player.frame = getFrameIdle(frame, player.frame);
            temp2 = player.frame;
        }
        player.flip == FALSE;

        //Cabeza
        set_sprite_tile(SPRITE_PLAYER_1, TILE_PLAYER_HEAD_1);
        set_sprite_tile(SPRITE_PLAYER_2, TILE_PLAYER_HEAD_2);
        set_sprite_tile(SPRITE_PLAYER_3, TILE_PLAYER_HEAD_3);
        set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_HEAD_4);
        //Cuerpo
        set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
        set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_BODY_2);
        //Tiles especiales reseteados a blanco
        set_sprite_tile(SPRITE_PLAYER_9, TILE_BLANK);//Especial(Sobreesale)
        set_sprite_tile(SPRITE_PLAYER_10, TILE_BLANK);//Especial(Sobreesale)

        //Coloco los sprites en su posicion
        move_sprite(SPRITE_PLAYER_1, (player.x DEC_BITS) - camera.scroll +8 +temp, player.y +16 +temp2);
        move_sprite(SPRITE_PLAYER_2, (player.x DEC_BITS) - camera.scroll +16 -temp, player.y +16 +temp2);
        move_sprite(SPRITE_PLAYER_3, (player.x DEC_BITS) - camera.scroll +8 +temp, player.y +24 +temp2);
        move_sprite(SPRITE_PLAYER_4, (player.x DEC_BITS) - camera.scroll +16 -temp, player.y +24 +temp2);
        move_sprite(SPRITE_PLAYER_5, (player.x DEC_BITS) - camera.scroll +8 +temp, player.y +32 +temp2);
        move_sprite(SPRITE_PLAYER_6, (player.x DEC_BITS) - camera.scroll +16 -temp, player.y +32 +temp2);
        move_sprite(SPRITE_PLAYER_7, (player.x DEC_BITS) - camera.scroll +8 +temp, player.y +40);
        move_sprite(SPRITE_PLAYER_8, (player.x DEC_BITS) - camera.scroll +16 -temp, player.y +40);

        if(!isInGround){
            //Piernas
            set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_JUMP_1);
            set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_JUMP_2);
        }else{
            if(player.state == STATE_RUN){
                //Piernas
                player.frame = getPlayerFrameRun(frame, player.frame);
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_RUN_F1_1 + (player.frame<<1));
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_RUN_F1_2 + (player.frame<<1));
            }
            else if(player.state == STATE_IDLE){
               //Piernas
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_FOOTS_2);

            }
            else if(player.state == STATE_ATACK){
                if(player.frame < 3){
                    //Cuerpo
                    set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_F1_1);//Especial
                    //Piernas
                    set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                    set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_F1_2);//Especial

                    move_sprite(SPRITE_PLAYER_6, (player.x DEC_BITS) - camera.scroll +16 -temp, player.y +32);//ESPECIAL
                    move_sprite(SPRITE_PLAYER_7, (player.x DEC_BITS) - camera.scroll +8 +temp, player.y +40);//ESPECIAL
                }else if(player.frame < 9){
                //Atack frame 2
                    //Cabeza
                    set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_ATACK_F2_1);//Especial
                    set_sprite_tile(SPRITE_PLAYER_9, TILE_PLAYER_ATACK_F2_2);//Especial(Sobreesale)
                    //Cuerpo
                    set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_F2_3);//Especial
                    set_sprite_tile(SPRITE_PLAYER_10, TILE_PLAYER_ATACK_F2_4);//Especial(Sobreesale)
                    //Piernas
                    set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_F1_2);//Especial

                    move_sprite(SPRITE_PLAYER_9, (player.x DEC_BITS) - camera.scroll+24-(temp*3), player.y +24);//ESPECIAL
                    move_sprite(SPRITE_PLAYER_6, (player.x DEC_BITS) - camera.scroll+16-temp, player.y +32);//ESPECIAL
                    move_sprite(SPRITE_PLAYER_10, (player.x DEC_BITS) - camera.scroll+24-(temp*3), player.y +32);//ESPECIAL
                }else if(player.frame < 12){
                    //Cuerpo
                    set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_F1_1);//Especial
                    //Piernas
                    set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_F1_2);//Especial

                    move_sprite(SPRITE_PLAYER_6, (player.x DEC_BITS) - camera.scroll+16-temp, player.y +32);//ESPECIAL
                    move_sprite(SPRITE_PLAYER_7, (player.x DEC_BITS) - camera.scroll+8+temp, player.y +40);//ESPECIAL
                }else{
                    player.state = STATE_IDLE;
                    player.frame = 0;
                }
            }
        }
        }
        if(player.state == STATE_ATACK){
            player.frame++;
        }

        //Actualizo los valores de la camara
        camera.lastX = camera.scroll;
        camera.scroll = getScroll(player.x DEC_BITS, camera.scroll);
        screenCountX += (camera.scroll-camera.lastX);

        //Movimiento del fondo
        move_bkg(camera.scroll, 0);
        //scroll_bkg(camera.scroll-camera.lastX, 0);
        //Al avanzar un tile se actualiza el mapa
        if(screenCountX == 8){
        //if(screenCountX & 8){//Ocupa menos
            screenCountX = 0;

            /**
            Posiciona el indice X en la posicion del vector a rellenar
            camera.scroll>>3 es la suma de tiles de 8x8 avanzados: camera.scroll/8
            Ejemplo: el primer tile que se avance sera 1 + 21 = 22
            Como el tile que queremos rellenar de mas a la derecha se encuentra fuera de la pantalla (20 tiles)
            sumo 21
            */
            temp = (camera.scroll>>3)+21;
            /**
            El modulo se debe a que una vez superados los 32 tiles (32*8=256) la cuenta se reseta a 0
            (Le sigue el 0, no el 33)
            Es elmotivo de porque si el personaje esta enla posicion 257, se tilea a partir de la posición 0
            */
            temp2 = temp % 32;
            /*
            //18 son los tiles que entran en la pantalla en alto
            for(count = 0; count != 18; count++ ){

                //Solo relleno el ultimo tile por la derecha
                set_bkg_tiles(temp2, count, 1, 1, &(LEVEL1+temp));

                 //incrementa Cnt con el tamaño de X para que se cargue la proxima fila
                temp = temp + MAP_SIZE_X;
            }
            */
            //Mierda para ahorrar memoria
            count = 18;
            do{
                set_bkg_tiles(temp2, 18-count, 1, 1, &(LEVEL1+temp));
                temp = temp + MAP_SIZE_X;
            }while(count--);
        }
        /*
        else if(screenCountX == -8){
            screenCountX = 0;

            //Como el tile que queremos rellenar de mas a la izquierda se encuentra fuera de la pantalla resto -1

            temp = (camera.scroll>>3)-1;

            //El modulo se debe a que una vez superados los 32 tiles (32*8=256) la cuenta se reseta a 0
            //(Le sigue el 0, no el 33)
            //Es elmotivo de porque si el personaje esta enla posicion 257, se tilea a partir de la posición 0

            temp2 = temp % 32;
            //18 son los tiles que entran en la pantalla en alto
            for(count = 0; count != 18; count++ ){

                //Solo relleno el ultimo tile por la derecha
                set_bkg_tiles(temp2, count, 1, 1, &(LEVEL1+temp));

                 //incrementa Cnt con el tamaño de X para que se cargue la proxima fila
                temp = temp + MAP_SIZE_X;
            }
        }
        */








    //drawPoints(gochiList[0].x, 5, SCREEN_WIDTH - (8*4), 16);//hasta 256

    frame++;

    }

}

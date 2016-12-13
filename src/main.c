#include <gb/gb.h>
#include <stdio.h>
#include <time.h>
#include <gb/drawing.h>
#include "spriteTiles.h"
#include "mapTiles.h"
#include "collision.h"

#define TRUE 1
#define FALSE 0

#define PRECISION_BITS 6
#define INC_BITS <<PRECISION_BITS
#define DEC_BITS >>PRECISION_BITS

#define GRAVITY 8

//CHARACTERS
#define MAX_ENEMY_NUMBER 1//Limite esta en 4
//Payer
#define PLAYER_WIDTH 16
#define PLAYER_HEIGHT 32
#define PLAYER_WEIGHT 5
#define PLAYER_JUMP 3
#define PLAYER_SPEED 1
//Enemy Popo
#define ENEMY_POPO_WIDTH 8
#define ENEMY_POPO_HEIGHT 8
#define ENEMY_POPO_SPEED 1



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

/**
Datos del mapa maps.c Los arrays estan implementados en maps.c
*/
#define MAP_SIZE_X 128
#define MAP_SIZE_Y 18
#define LEVEL_WIDTH (MAP_SIZE_X * 8)
#define LEVEL_HEIGHT (MAP_SIZE_Y * 8)


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
    UINT16 y;
    UBYTE flip;
    UINT16 velocityDesc;
    UINT16 velocityAsc;
    UBYTE state;
    UBYTE frame;
};

struct EnemyPopo {
    UINT16 x;
    UINT16 y;
    UBYTE direction;
    UBYTE state;
    UBYTE frame;
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

UINT16 getScroll(UINT16 objX) {

    if(objX > SCREEN_WIDTH2){
        if(objX < LEVEL_WIDTH - SCREEN_WIDTH2){
            return objX - SCREEN_WIDTH2;
        }else{
            return LEVEL_WIDTH - SCREEN_WIDTH;
        }
    }else{
        return 0;
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
    if(gameFrame%15 == 0) {
        if(characterFrame == 1){
            characterFrame = 0;
        }else{
            characterFrame = 1;
        }
    }
    return characterFrame;
}
UBYTE getPlayerFrameRun(UBYTE gameFrame, UBYTE playerFrame) {
    if(gameFrame%4 == 0) {
        playerFrame = (playerFrame +1)%3;
    }
    return playerFrame;
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

UINT16 getGravitySpeed(UINT16 gravityForce, UINT16 weight){
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

UINT16 getStrongJump(UINT16 strongJump){
    /*
    Regla de tres INVERSA
    (Las pruebas se hicieron con desplazamientos de 8 (3 bits), de esta manera, si se cambia esto, la fisica se adapta)
    8               = strongJump
    PRECISION_BITS  = newStrongJump
    */
    return (8 * strongJump) / PRECISION_BITS;
}

void main() {
    //Mierda temporal
    //printf("%d",3);
    //printf("Inicio del programa");

    //Declaracion
    UBYTE frame;
    UINT16 temp;
    UINT16 temp2;
    UWORD count;

    UINT8 screenCountX;

    //Maneja el contador de numero de enemigos en el nivel
    UINT8 numberEnemiesPopo;


    UBYTE isInGround;
    UINT16 tileIndex;



    UBYTE keys;
    UBYTE keyB_Up;
    UBYTE keyB_Down;

    UINT16 newX;
    UINT16 newY;



    char digitPoints[5];
    struct EnemyPopo enemyPopoList[MAX_ENEMY_NUMBER];
    struct Player player;
    struct Camera camera;

    temp = 0;
    temp2 = 0;
    count = 0;
    screenCountX = 0;

    keys = 0;
    keyB_Up = TRUE;
    keyB_Down = FALSE;

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

    //Tiles del fondo
    SWITCH_ROM_MBC1(2);//Banco de memoria 2
    //posicion inicial, numero y tiles
    set_bkg_data(0, TOTAL_MAP_TILES, MAP_TILES);

    //Sprites
    //Carga en la VRAM los tiles para los sprites
    SPRITES_8x8;
    SWITCH_ROM_MBC1(2);//Salto al banco de memoria 2
    //posicion de memoria, cantidad de tiles, tiles
    set_sprite_data(0, TOTAL_TILES, SPRITE_TILES);
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


    for(count = 0; count != MAX_ENEMY_NUMBER; count++){
        enemyPopoList[count].state = STATE_UNACTIVE;
    }
    //Se debe de reastrar el nivel en busca de posiciones (O definirlas previamete)
    numberEnemiesPopo = MAX_ENEMY_NUMBER;

    for(count = 0; count != numberEnemiesPopo; count++){
        enemyPopoList[count].x = (60 + (count * 60))INC_BITS;
        enemyPopoList[count].y = (SCREEN_HEIGHT-ENEMY_POPO_HEIGHT-8) INC_BITS;
        enemyPopoList[count].frame = TILE_ENEMY_POPO_IDLE_F1;
        enemyPopoList[count].state = STATE_RUN;
        enemyPopoList[count].direction = FALSE;
    }
    frame = 0;
    while(TRUE) {
        /**
        Gestion del game pad
        */
        /**
        Sincornización con el blanqueo de la pantalla.

        Espera a que la pantalla esta lista para para pintarse (Se detiene la lógica de los calculos)
        En la GameBoy mientras se hace el blanqueo de la pantalla no se puede acceder a la memoria.
        De esta manera, una vez acaba el blanqueo de la memoria, se estable el pequeño tiempo en el que
        se puede acceder a la memoria de video
        */
        keys = joypad();//Lee el pad


        if(keyB_Up == TRUE){//No se esta pulsando ninguna tecla
            if(keyPressB(keys) == TRUE){
                keyB_Down = TRUE;//Entra evento down
                keyB_Up = FALSE;
            }
        }else{
            keyB_Down = FALSE;
            if(keyPressB(keys) == TRUE){//En el frame anterior ya se pulsaba
                keyB_Up = FALSE;
            }else{
                keyB_Up = TRUE;
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
            newY -= player.velocityAsc;
            player.velocityAsc -= getGravitySpeed(GRAVITY, PLAYER_WEIGHT);
        }else{
            player.velocityAsc = 0;
            player.velocityDesc += getGravitySpeed(GRAVITY, PLAYER_WEIGHT);
            newY += player.velocityDesc;
        }
        isInGround = isCollisionDown(player.x DEC_BITS, (player.y + (1 INC_BITS)) DEC_BITS, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1);


        /**
        Inputs
        */
        if(keyPressSTART(keys) == TRUE){}
        if(keyPressSELECT(keys) == TRUE){}
        if(keyPressUP(keys) == TRUE) {
        }
        else if(keyPressDOWN(keys) == TRUE) {
        }
        if(keyPressLEFT(keys) == TRUE) {
            if(player.state == STATE_IDLE || player.state == STATE_RUN || isInGround == FALSE){
                player.flip = TRUE;
                player.state = STATE_RUN;
                newX -=  PLAYER_SPEED INC_BITS;
            }
        }
        else if(keyPressRIGHT(keys) == TRUE) {
            if(player.state == STATE_IDLE || player.state == STATE_RUN || isInGround == FALSE){
                player.flip = FALSE;
                newX +=  PLAYER_SPEED INC_BITS;
                player.state = STATE_RUN;
            }
        }
        else{
            //Cuando mete piñas y reciba, habra wue definir que estados no lo ponen automaticamente en idle
           player.state = STATE_IDLE;
        }
        if(keyPressA(keys) == TRUE) {

        }
        if(keyB_Down == TRUE) {
            if(isInGround){
                player.velocityAsc = getStrongJump(PLAYER_JUMP INC_BITS);

            }

        }


        /**
        Colisiones player
        */
        tileIndex = isCollisionDown(player.x DEC_BITS, newY DEC_BITS, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1);
        if(tileIndex!=0){
            newY = (((tileIndex / MAP_SIZE_X) * 8) - PLAYER_HEIGHT) INC_BITS;
            player.velocityDesc = 0;
        }
        tileIndex = isCollisionUp(player.x DEC_BITS, newY DEC_BITS, PLAYER_WIDTH, MAP_SIZE_X, LEVEL1);
        if(tileIndex!=0){
            newY = (((tileIndex / MAP_SIZE_X) * 8) + 8) INC_BITS;
            player.velocityAsc = 0;
            player.velocityDesc = 0;
        }
        player.y = newY;
        tileIndex = isCollisionRight(newX DEC_BITS, player.y DEC_BITS, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1);
        if(tileIndex!=0){
            newX = (((tileIndex % MAP_SIZE_X) * 8)-PLAYER_WIDTH) INC_BITS;
        }
        tileIndex = isCollisionLeft(newX DEC_BITS, player.y DEC_BITS, PLAYER_HEIGHT, MAP_SIZE_X, LEVEL1);
        if(tileIndex!=0){
            newX = (((tileIndex % MAP_SIZE_X) * 8)+8) INC_BITS;
        }
        player.x = newX;


        //Set limits screen
        if(player.x DEC_BITS < 1){
            player.x = 1 INC_BITS;
        }
        else if((player.x DEC_BITS) + PLAYER_WIDTH >= LEVEL_WIDTH){
            player.x = (LEVEL_WIDTH - PLAYER_WIDTH) INC_BITS;
        }

        /**
        Colisiones enemigos
        */
        for(count = 0; count != numberEnemiesPopo; count++){
            temp = (SPRITE_ENEMY_1 + count);
            if(enemyPopoList[count].state != STATE_UNACTIVE
               && isInScreen(camera.scroll, (enemyPopoList[count].x DEC_BITS), ENEMY_POPO_WIDTH) == TRUE){
                //Intena moverse
                if(enemyPopoList[count].direction == TRUE){
                    newX = enemyPopoList[count].x + (ENEMY_POPO_SPEED INC_BITS);
                }else{
                    newX = enemyPopoList[count].x - (ENEMY_POPO_SPEED INC_BITS);
                }
                tileIndex = isCollisionLeft(newX DEC_BITS, enemyPopoList[count].y DEC_BITS, ENEMY_POPO_HEIGHT, MAP_SIZE_X, LEVEL1);
                if(tileIndex != 0){
                   newX = (((tileIndex % MAP_SIZE_X) * 8)+8) INC_BITS;
                   enemyPopoList[count].direction = blink01(enemyPopoList[count].direction);
                }
                tileIndex = isCollisionRight(newX DEC_BITS, enemyPopoList[count].y DEC_BITS, ENEMY_POPO_WIDTH, ENEMY_POPO_HEIGHT, MAP_SIZE_X, LEVEL1);
                if(tileIndex != 0){
                   newX = (((tileIndex % MAP_SIZE_X) * 8)-ENEMY_POPO_WIDTH) INC_BITS;
                   enemyPopoList[count].direction = blink01(enemyPopoList[count].direction);
                }
                //|| (newX DEC_BITS)== 0
                //|| (newX DEC_BITS) + ENEMY_POPO_WIDTH == LEVEL_WIDTH
                //No hay suelo
                tileIndex = isCollisionDown(newX DEC_BITS, enemyPopoList[count].y DEC_BITS, ENEMY_POPO_WIDTH, ENEMY_POPO_HEIGHT, MAP_SIZE_X, LEVEL1);
                if(tileIndex !=0){
                   enemyPopoList[count].direction = blink01(enemyPopoList[count].direction);
                }
                enemyPopoList[count].x = newX;
            }
        }

        wait_vbl_done();
        /**
        Actualizacion de tiles del player
        */
        if(isInGround == FALSE){
            //Cabeza
            set_sprite_tile(SPRITE_PLAYER_1, TILE_PLAYER_HEAD_1);
            set_sprite_tile(SPRITE_PLAYER_2, TILE_PLAYER_HEAD_2);
            set_sprite_tile(SPRITE_PLAYER_3, TILE_PLAYER_HEAD_3);
            set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_HEAD_4);
            //Cuerpo
            set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
            set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_BODY_2);
            //Piernas
            set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_JUMP_1);
            set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_JUMP_2);
        }
        else if(player.state == STATE_RUN){
            //Cabeza
            set_sprite_tile(SPRITE_PLAYER_1, TILE_PLAYER_HEAD_1);
            set_sprite_tile(SPRITE_PLAYER_2, TILE_PLAYER_HEAD_2);
            set_sprite_tile(SPRITE_PLAYER_3, TILE_PLAYER_HEAD_3);
            set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_HEAD_4);
            //Cuerpo
            set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
            set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_BODY_2);
            //Piernas
            player.frame = getPlayerFrameRun(frame, player.frame);
            set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_RUN_F1_1 + (player.frame<<1));
            set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_RUN_F1_2 + (player.frame<<1));


        }
        else if(player.state == STATE_IDLE){
            if(isInGround){
                /*
                //Cabeza
                set_sprite_tile(SPRITE_PLAYER_1, TILE_PLAYER_HEAD_1);
                set_sprite_tile(SPRITE_PLAYER_2, TILE_PLAYER_HEAD_2);
                set_sprite_tile(SPRITE_PLAYER_3, TILE_PLAYER_HEAD_3);
                set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_HEAD_4);
                //Cuerpo
                set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_BODY_2);
                //Piernas
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_FOOTS_2);

                //Atack frame 1
                //Cabeza
                set_sprite_tile(SPRITE_PLAYER_1, TILE_PLAYER_HEAD_1);
                set_sprite_tile(SPRITE_PLAYER_2, TILE_PLAYER_HEAD_2);
                set_sprite_tile(SPRITE_PLAYER_3, TILE_PLAYER_HEAD_3);
                set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_HEAD_4);
                //Cuerpo
                set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_F1_1);//Especial
                //Piernas
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);//Especial
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_F1_2);
                //Atack frame 2
                //Cabeza
                 */
                set_sprite_tile(SPRITE_PLAYER_1, TILE_PLAYER_HEAD_1);
                set_sprite_tile(SPRITE_PLAYER_2, TILE_PLAYER_HEAD_2);
                set_sprite_tile(SPRITE_PLAYER_3, TILE_PLAYER_HEAD_3);
                set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_ATACK_F2_1);//Especial
                set_sprite_tile(SPRITE_PLAYER_9, TILE_PLAYER_ATACK_F2_2);//Especial(Sobreesale)
                //Cuerpo
                set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_F2_3);//Especial
                set_sprite_tile(SPRITE_PLAYER_10, TILE_PLAYER_ATACK_F2_4);//Especial(Sobreesale)
                //Piernas
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_F1_2);//Especial

            }
        }


        /**
        Actualizacion de tiles de los enemigos
        */
        for(count = 0; count != numberEnemiesPopo; count++){
            temp = (SPRITE_ENEMY_1 + count);
            if(enemyPopoList[count].state != STATE_UNACTIVE
               && isInScreen(camera.scroll, (enemyPopoList[count].x DEC_BITS), ENEMY_POPO_WIDTH) == TRUE){
                  set_sprite_tile(temp, enemyPopoList[count].frame);

                if(enemyPopoList[count].state == STATE_RUN){
                    enemyPopoList[count].frame = getFrameIdle(frame, enemyPopoList[count].frame);
                    set_sprite_tile(SPRITE_ENEMY_1+count, TILE_ENEMY_POPO_IDLE_F1 + (enemyPopoList[count].frame));
                }
            }
            else{
                set_sprite_tile(temp, TILE_BLANK);
            }
        }

        //Actualizo los valores de la camara
        camera.lastX = camera.scroll;
        camera.scroll = getScroll(player.x DEC_BITS);
        screenCountX += (camera.scroll-camera.lastX);

        frame++;

        //Movimiento de los sprites //ANCHOR-> abajo derecha
        if(player.flip == TRUE){
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
        /*
        move_sprite(SPRITE_PLAYER_1, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+16+temp2);
        move_sprite(SPRITE_PLAYER_2, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+16+temp2);
        move_sprite(SPRITE_PLAYER_3, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+24+temp2);
        move_sprite(SPRITE_PLAYER_4, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+24+temp2);
        move_sprite(SPRITE_PLAYER_5, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+32+temp2);
        move_sprite(SPRITE_PLAYER_6, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+32+temp2);
        move_sprite(SPRITE_PLAYER_7, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+40);
        move_sprite(SPRITE_PLAYER_8, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+40);

        //Atack frame 1
        move_sprite(SPRITE_PLAYER_1, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+16);
        move_sprite(SPRITE_PLAYER_2, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+16);
        move_sprite(SPRITE_PLAYER_3, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+24);
        move_sprite(SPRITE_PLAYER_4, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+24);

        move_sprite(SPRITE_PLAYER_5, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+32);
        move_sprite(SPRITE_PLAYER_6, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+32);//ESPECIAL

        move_sprite(SPRITE_PLAYER_7, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+40);//ESPECIAL
        move_sprite(SPRITE_PLAYER_8, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+40);
        */

        //Atack frame 2

        move_sprite(SPRITE_PLAYER_1, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+16);
        move_sprite(SPRITE_PLAYER_2, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+16);
        move_sprite(SPRITE_PLAYER_3, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+24);
        move_sprite(SPRITE_PLAYER_4, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+24);
        move_sprite(SPRITE_PLAYER_9, (player.x DEC_BITS) - camera.scroll+24-(temp*3), (player.y DEC_BITS)+24);//ESPECIAL

        move_sprite(SPRITE_PLAYER_5, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+32);
        move_sprite(SPRITE_PLAYER_6, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+32);//ESPECIAL
        move_sprite(SPRITE_PLAYER_10, (player.x DEC_BITS) - camera.scroll+24-(temp*3), (player.y DEC_BITS)+32);//ESPECIAL

        move_sprite(SPRITE_PLAYER_7, (player.x DEC_BITS) - camera.scroll+8+temp, (player.y DEC_BITS)+40);
        move_sprite(SPRITE_PLAYER_8, (player.x DEC_BITS) - camera.scroll+16-temp, (player.y DEC_BITS)+40);


        for(count = 0; count != numberEnemiesPopo; count++){
            temp = (SPRITE_ENEMY_1 + count);
            if(enemyPopoList[count].state == STATE_RUN
               && isInScreen(camera.scroll, (enemyPopoList[count].x DEC_BITS), ENEMY_POPO_WIDTH) == TRUE){
                move_sprite(temp,
                    (enemyPopoList[count].x DEC_BITS) - camera.scroll +8,
                    (enemyPopoList[count].y DEC_BITS) +16);
            if(enemyPopoList[count].direction == 0){
                set_sprite_prop(SPRITE_ENEMY_1 + count, S_FLIPX);
            }else{
                set_sprite_prop(SPRITE_ENEMY_1 + count, 0);
            }
            }
        }



        //Movimiento del fondo
        //move_bkg(camera.x, bgPosY);
        scroll_bkg(camera.scroll-camera.lastX, 0);
        //Al avanzar un tile se actualiza el mapa
        if(screenCountX == 8){
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
            //18 son los tiles que entran en la pantalla en alto
            for(count = 0; count != 18; count++ ){

                //Solo relleno el ultimo tile por la derecha
                set_bkg_tiles(temp2, count, 1, 1, &(LEVEL1+temp));

                 //incrementa Cnt con el tamaño de X para que se cargue la proxima fila
                temp = temp + MAP_SIZE_X;
            }
        }
        else if(screenCountX == -8){
            screenCountX = 0;
            /**
            Como el tile que queremos rellenar de mas a la izquierda se encuentra fuera de la pantalla
            resto -1
            */
            temp = (camera.scroll>>3)-1;
            /**
            El modulo se debe a que una vez superados los 32 tiles (32*8=256) la cuenta se reseta a 0
            (Le sigue el 0, no el 33)
            Es elmotivo de porque si el personaje esta enla posicion 257, se tilea a partir de la posición 0
            */
            temp2 = temp % 32;
            //18 son los tiles que entran en la pantalla en alto
            for(count = 0; count != 18; count++ ){

                //Solo relleno el ultimo tile por la derecha
                set_bkg_tiles(temp2, count, 1, 1, &(LEVEL1+temp));

                 //incrementa Cnt con el tamaño de X para que se cargue la proxima fila
                temp = temp + MAP_SIZE_X;
            }
        }







        //temp = isInScreen(camera.scroll, (enemyPopoList[0].x DEC_BITS), ENEMY_POPO_WIDTH);
        //drawPoints(temp, 5, SCREEN_WIDTH - (8*4), 16);//hasta 256



    }
    //puts("gameFrame32:");

}

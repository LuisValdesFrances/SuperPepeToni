#include <stdio.h>
#include <gb/gb.h>
#include <gb/drawing.h>
#include "constans.h"
#include "font.h"
#include "spriteTiles.h"
#include "mapTiles.h"
#include "keys.h"
#include "collision.h"
#include "enemyPosition.h"
#include "utils.h"
#include "text.h"

extern const unsigned char FONT_TILES[];//Informacion de los tiles (imagenes)
extern const unsigned char MAP_TILES[];//Informacion de los tiles (imagenes)
extern const unsigned char SPRITE_TILES[];//Informacion de los tiles (imagenes)
extern const unsigned char LEVEL1[];//Colisiones y distribucion
extern unsigned UBYTE platformLevel_1_X[];//Posiciones de los malos
extern unsigned UBYTE platformLevel_1_Y[];//Posiciones de los malos
extern unsigned UBYTE platformLevel_1_Path[];//Posiciones de los malos
extern unsigned UBYTE gochiLevel_1_X[];//Posiciones de los malos
extern unsigned UBYTE gochiLevel_1_Y[];//Posiciones de los malos
extern unsigned UBYTE popoLevel_1_X[];//Posiciones de los malos
extern unsigned UBYTE popoLevel_1_Y[];//Posiciones de los malos
extern unsigned UBYTE babitLevel_1_X[];//Posiciones de los malos
extern unsigned UBYTE babitLevel_1_Y[];//Posiciones de los malos

struct Player {
    UINT16 x;
    UBYTE y;
    UBYTE flip;
    UBYTE velocityDesc;
    UBYTE velocityAsc;
    UBYTE state;
    UBYTE frame;
    UBYTE suffCount;
    UBYTE airAtack;
};

struct Enemy {
    UBYTE type;
    UBYTE x;
    UBYTE y;
    UBYTE flip;
    UBYTE frame;
    UBYTE expCount;
};

struct Platform {
    UBYTE x;
    UBYTE y;
    UBYTE path;
};

struct Bullet {
    UINT16 x;
    UBYTE y;
    UBYTE active;
};

struct Camera {
    UINT16 scrollX;
    UINT16 lastX;
    UBYTE scrollY;
    UBYTE lastY;
};

//Quitar el player de aqui y pasar su posicion por parametro
UBYTE checkPlayerDamage(struct Camera *camera, struct Player *player, struct Enemy *enemy){

    UBYTE enemyW;
    UBYTE enemyH;

    if((*enemy).type != 0 && (*enemy).expCount == 0){

        if((*enemy).type == POPO){
            enemyW=POPO_WIDTH;
            enemyH=POPO_HEIGHT;
        }
        else if((*enemy).type == GOCHI){
            enemyW=GOCHI_WIDTH;
            enemyH=GOCHI_HEIGHT;
        }
        else if((*enemy).type == BABIT){
            enemyW=BABIT_WIDTH;
            enemyH=BABIT_HEIGHT;
        }

        if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*enemy).x DEC_BITS), (*enemy).y, enemyW, enemyH)){
            if((*player).suffCount == 0
                &&
                checkCollision(
                    ((*player).x DEC_BITS), (*player).y, PLAYER_WIDTH, PLAYER_HEIGHT,
                    ((*enemy).x DEC_BITS), (*enemy).y, enemyW, enemyH)){
                    (*player).suffCount = SUFF_COUNT;
            return TRUE;
            }
        }
    }
    return FALSE;
}

void checkEnemyDamage(struct Camera *camera, struct Player *player, struct Enemy *enemy, BYTE isInGround){
    UINT16 temp;
    UBYTE frameDamage;
    UBYTE enemyW;
    UBYTE enemyH;

    if((*enemy).type != 0 && (*enemy).expCount == 0){

        if((*enemy).type == POPO){
            enemyW = POPO_WIDTH;
            enemyH = POPO_HEIGHT;
        }
        else if((*enemy).type == GOCHI){
            enemyW = GOCHI_WIDTH;
            enemyH = GOCHI_HEIGHT;
        }
        else if((*enemy).type == BABIT){
            enemyW = BABIT_WIDTH;
            enemyH = BABIT_HEIGHT;
        }
        if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*enemy).x DEC_BITS), (*enemy).y, enemyW, enemyH)){
            //Dependiendo de si es una patada o un pu�etazo, el frame que causa da�o es uno u otro
            if((*player).state == STATE_ATACK){
                if(isInGround){
                    if((*player).frame > 3 && (*player).frame < 9){
                        frameDamage = TRUE;
                    }else{
                        frameDamage = FALSE;
                    }
                }else{
                    if((*player).frame > 0){
                        frameDamage = TRUE;
                    }else{
                        frameDamage = FALSE;
                    }
                }
                if(frameDamage == TRUE){

                    temp = ((*player).x DEC_BITS) + PLAYER_WIDTH;
                    if((*player).flip){
                        temp = ((*player).x DEC_BITS) - (PLAYER_WIDTH>>1);
                    }
                    if(
                        checkCollision(
                        temp, (*player).y, (PLAYER_WIDTH>>1), PLAYER_HEIGHT,
                        ((*enemy).x DEC_BITS), (*enemy).y, enemyW, enemyH)){

                            (*enemy).expCount = 1;

                            //Si ha pegado una patada voladora, desactivo el ataque
                            (*player).state = STATE_IDLE;
                    }
                }
            }
        }
    }
}

void movePlatform(struct Platform *platform, UBYTE platformW, UBYTE platformH, UBYTE speed, unsigned char *level){
    UBYTE newY;
    UINT16 x;
    if((*platform).x != 0){
        //La posicion x indica el tile en el que se encuentra, por eso necesito multiplicar por 8 para obtener la posicion
        x = (*platform).x;
        x = x<<3;
        if((*platform).path){
            newY = (*platform).y + speed;
        }else{
            newY = (*platform).y - speed;        }
        if(isCollisionDownT(x, newY, platformW, platformH+1, MAP_SIZE_X, level)){
            newY = (*platform).y;
            (*platform).path = blink01((*platform).path);
        }
        if(isCollisionUpT(x, newY, platformW, MAP_SIZE_X, level)){
            newY = (*platform).y;
            (*platform).path = blink01((*platform).path);
        }

        (*platform).y = newY;
    }
}

void moveEnemy(struct Camera *camera, struct Enemy *enemy, unsigned char *level){
    UINT16 newX;
    UBYTE enemyW;
    UBYTE enemyH;
    UBYTE speed;

    if((*enemy).type != 0 && (*enemy).expCount == 0){
        if((*enemy).type == POPO){
            enemyW = POPO_WIDTH;
            enemyH = POPO_HEIGHT;
            speed = POPO_SPEED;
        }
        else if((*enemy).type == GOCHI){
            enemyW = GOCHI_WIDTH;
            enemyH = GOCHI_HEIGHT;
            speed = GOCHI_SPEED;
        }
        else if((*enemy).type == BABIT){
            enemyW = BABIT_WIDTH;
            enemyH = BABIT_HEIGHT;
            speed = 0;
        }
        if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*enemy).x DEC_BITS), (*enemy).y, enemyW, enemyH)){
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
            if(!isCollisionDown(newX DEC_BITS, (*enemy).y, enemyW, enemyH+1, MAP_SIZE_X, level)){
                newX = (*enemy).x;
                (*enemy).flip = blink01((*enemy).flip);
            }

            (*enemy).x = newX;
        }
        /*
        else{
            //Si se ha quedado a la izquierda fuera del scroll, lo desactivo
            if(((*enemy).x DEC_BITS) + enemyW < (*camera).scrollX){
                (*enemy).expCount = 4;
            }
        }
        */
    }
}

void moveBullet(struct Camera *camera, struct Bullet *bullet, UBYTE bulletW, UBYTE bulletH, UBYTE speed, unsigned char *level){
    UINT16 newX;
    newX = (*bullet).x - speed;
    if((*bullet).active){
        if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*bullet).x DEC_BITS), (*bullet).y, bulletW, bulletH)){
            if(isCollisionLeft(newX DEC_BITS, (*bullet).y, bulletW, MAP_SIZE_X, level)){
                (*bullet).active = FALSE;
            }else{
                (*bullet).x = newX;
            }
        }else{
             (*bullet).active = FALSE;
        }
    }
}

void drawGochi(struct Camera *camera, struct Enemy *gochi, UBYTE count, UBYTE frame){

    UBYTE temp;
    UBYTE count2;
    UINT16 x;
    UBYTE y;

    x = (*gochi).x << 3;
    y = (*gochi).y << 3;

    temp = count*6;//6 es el numero de sprites
    if(isInScreen((*camera).scrollX, (*camera).scrollY, x, y, GOCHI_WIDTH, GOCHI_HEIGHT)
       && (*gochi).type != 0){
        if((*gochi).expCount == 0){
            set_sprite_tile(SPRITE_ENEMY_16X24_1+temp, TILE_GOCHI_1_F1);
            set_sprite_tile(SPRITE_ENEMY_16X24_2+temp, TILE_GOCHI_2_F1);
            set_sprite_tile(SPRITE_ENEMY_16X24_3+temp, TILE_GOCHI_3_F1 + ((*gochi).frame*4));
            set_sprite_tile(SPRITE_ENEMY_16X24_4+temp, TILE_GOCHI_4_F1 + ((*gochi).frame*4));
            set_sprite_tile(SPRITE_ENEMY_16X24_5+temp, TILE_GOCHI_5_F1 + ((*gochi).frame*4));
            set_sprite_tile(SPRITE_ENEMY_16X24_6+temp, TILE_GOCHI_6_F1 + ((*gochi).frame*4));

            (*gochi).frame = getFrameIdle(frame, (*gochi).frame, 10);//OJO

            count2= 6;
            do{
                if((*gochi).flip){
                    set_sprite_prop(SPRITE_ENEMY_16X24_1 + temp + 6-count2, S_FLIPX);
                }else{
                    set_sprite_prop(SPRITE_ENEMY_16X24_1 + temp + 6-count2, 0);
                }
            }while(count2--);
        }else{
            if((*gochi).expCount < 4){
                set_sprite_tile(SPRITE_ENEMY_16X24_1+temp, TILE_EXP_1_F1 + (4*((*gochi).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_16X24_2+temp, TILE_EXP_2_F1 + (4*((*gochi).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_16X24_3+temp, TILE_EXP_3_F1 + (4*((*gochi).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_16X24_4+temp, TILE_EXP_4_F1 + (4*((*gochi).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_16X24_5+temp, TILE_BLANK);
                set_sprite_tile(SPRITE_ENEMY_16X24_6+temp, TILE_BLANK);
            }else{
                /*Estos sprites se usan tambien para la plataforma*/
                count2 = 5;
                do
                {
                    set_sprite_tile(SPRITE_ENEMY_16X24_1 + count2 + temp, TILE_BLANK);
                }
                while(count2--);
                //Muere
                (*gochi).type = 0;
            }
            if(frame%6==0){
                (*gochi).expCount++;
            }
        }
    }
}

void moveSpriteGochi(struct Camera *camera, struct Enemy *gochi, UBYTE count){
    UBYTE temp;
    UBYTE temp2;
    UINT16 x;
    UBYTE y;
    x = (*gochi).x<<3;
    y = (*gochi).y<<3;
    temp = count*6;//6 es el numero de sprites
    if(isInScreen((*camera).scrollX, (*camera).scrollY, x, y, GOCHI_WIDTH, GOCHI_HEIGHT)
       && (*gochi).type != 0){

        if((*gochi).flip){
            temp2 = 8;
        }else{
            temp2 = 0;
        }
        move_sprite(SPRITE_ENEMY_16X24_1+temp, x - (*camera).scrollX +8 + temp2, y - (*camera).scrollY +16);
        move_sprite(SPRITE_ENEMY_16X24_2+temp, x - (*camera).scrollX +16 - temp2, y - (*camera).scrollY +16);
        move_sprite(SPRITE_ENEMY_16X24_3+temp, x - (*camera).scrollX +8 + temp2, y - (*camera).scrollY +24);
        move_sprite(SPRITE_ENEMY_16X24_4+temp, x - (*camera).scrollX +16 - temp2, y - (*camera).scrollY +24);
        move_sprite(SPRITE_ENEMY_16X24_5+temp, x - (*camera).scrollX +8 + temp2, y - (*camera).scrollY +32);
        move_sprite(SPRITE_ENEMY_16X24_6+temp, x - (*camera).scrollX +16 - temp2, y - (*camera).scrollY +32);
    }
}

void drawPlatform(struct Camera *camera, struct Platform *platform, UBYTE count){
    UBYTE temp;
    UINT16 x;
    //La posicion x indica el tile en el que se encuentra, por eso necesito multiplicar por 8 para obtener la posicion
    x = (*platform).x;
    x = x<<3;
    if((*platform).x != 0 && isInScreen((*camera).scrollX, (*camera).scrollY, x, (*platform).y, PLATFORM_WIDTH, PLATFORM_HEIGHT)){
        temp = count*4;//4 es el numero de tiles
        //Puede que otro enemigo que usa los mismos sprites haya flipeado
        set_sprite_prop(SPRITE_ENEMY_16X24_1 + temp, 0);
        set_sprite_prop(SPRITE_ENEMY_16X24_2 + temp, 0);
        set_sprite_prop(SPRITE_ENEMY_16X24_3 + temp, 0);
        set_sprite_prop(SPRITE_ENEMY_16X24_4 + temp, 0);
        set_sprite_tile(SPRITE_ENEMY_16X24_1 + temp, TILE_PLATFORM_1);
        set_sprite_tile(SPRITE_ENEMY_16X24_2 + temp, TILE_PLATFORM_2);
        set_sprite_tile(SPRITE_ENEMY_16X24_3 + temp, TILE_PLATFORM_3);
        set_sprite_tile(SPRITE_ENEMY_16X24_4 + temp, TILE_PLATFORM_4);
    }
}

void moveSpritePlatform(struct Camera *camera, struct Platform *platform, UBYTE count){
    UBYTE temp;
    UINT16 x;
    //La posicion x indica el tile en el que se encuentra, por eso necesito multiplicar por 8 para obtener la posicion
    x = (*platform).x;
    x = x<<3;
    if((*platform).x != 0 && isInScreen((*camera).scrollX, (*camera).scrollY, x, (*platform).y, PLATFORM_WIDTH, PLATFORM_HEIGHT)){
        temp = count*4;//4 es el numero de tiles
        move_sprite(SPRITE_ENEMY_16X24_1 + temp, x - (*camera).scrollX +8, ((*platform).y) - (*camera).scrollY  +16);
        move_sprite(SPRITE_ENEMY_16X24_2 + temp, x - (*camera).scrollX +16, ((*platform).y) - (*camera).scrollY  +16);
        move_sprite(SPRITE_ENEMY_16X24_3 + temp, x - (*camera).scrollX +8, ((*platform).y) - (*camera).scrollY  +24);
        move_sprite(SPRITE_ENEMY_16X24_4 + temp, x - (*camera).scrollX +16, ((*platform).y) - (*camera).scrollY  +24);
    }
}

UBYTE updatePlatform(struct Platform *platform, struct Player *player, unsigned char *map){

    UINT16 x;
    UBYTE y;
    UBYTE result;
    result = FALSE;

    movePlatform(platform, PLATFORM_WIDTH, PLATFORM_HEIGHT, PLATFORM_SPEED, map);

    x = platform->x;
    y = checkCollisionDown(
    (*player).x DEC_BITS, (*player).y, PLAYER_WIDTH, PLAYER_HEIGHT, x<<3, platform->y, PLATFORM_WIDTH, PLATFORM_HEIGHT);
    if(y){
        result = TRUE;
        (*player).y = y - PLAYER_HEIGHT;
        (*player).velocityDesc = 0;
    }

    y = checkCollisionUp(
    (*player).x DEC_BITS, (*player).y, PLAYER_WIDTH, x<<3, platform->y, PLATFORM_WIDTH, PLATFORM_HEIGHT);
    if(y){
        //result = TRUE;
        (*player).y = y;
        (*player).velocityAsc = 0;
    }

    return result;
}

void drawPopo(struct Camera *camera, struct Enemy *popo, UBYTE count, UBYTE frame){
    UBYTE temp;
    temp = count*1;//1 es el numero de tiles
    if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*popo).x DEC_BITS), (*popo).y, POPO_WIDTH, POPO_HEIGHT)
       && (*popo).type != 0){
        set_sprite_tile(SPRITE_ENEMY_8X8_1 + temp, TILE_POPO_F1 + ((*popo).frame));
        (*popo).frame = getFrameIdle(frame, (*popo).frame, 15);
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
    if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*popo).x DEC_BITS), (*popo).y, POPO_WIDTH, POPO_HEIGHT) && (*popo).type != 0){
        move_sprite(SPRITE_ENEMY_8X8_1 + temp,((*popo).x DEC_BITS) - (*camera).scrollX +8, ((*popo).y) - (*camera).scrollY  +16);
    }
}

void drawBullet(struct Bullet *bullet, UBYTE count){
    UBYTE temp;
    temp = count*1;//1 es el numero de tiles
    if((*bullet).active){
        set_sprite_tile(SPRITE_BULLET + temp, TILE_BULLET);
    }else{
        set_sprite_tile(SPRITE_BULLET + temp, TILE_BLANK);
    }
}

void moveSpriteBullet(struct Camera *camera, struct Bullet *bullet, UBYTE count){
    UBYTE temp;
    temp = count*1;//1 es el numero de tiles
    if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*bullet).x DEC_BITS), (*bullet).y, BULLET_WIDTH, BULLET_HEIGHT) && (*bullet).active){
        move_sprite(SPRITE_BULLET + temp,((*bullet).x DEC_BITS) - (*camera).scrollX +8, ((*bullet).y) - (*camera).scrollY +16);
    }
}

void showBullet(struct Bullet *bulletList[], UBYTE maxBullet, UINT16 posX, UBYTE posY){
    UBYTE i;
    struct Bullet *b;

    for(i = 0; i < maxBullet; i++){
        //Accedo a un puntero que apunta a un vector de structs
        b = &bulletList[i];

        if(!b->active){
            b->active = TRUE;
            b->x = posX;
            b->y = posY;
            i = maxBullet;
        }
    }
}

void drawBabit(struct Camera *camera, struct Enemy *babit, struct Bullet *bulletList[], UBYTE maxBullet, UBYTE count, UBYTE frame){

    UBYTE temp;
    UBYTE count2;
    temp = count*6;//6 es el numero de sprites
    if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*babit).x DEC_BITS), (*babit).y, BABIT_WIDTH, BABIT_HEIGHT)
       && (*babit).type != 0){

        //Giro al personaje
        count2 = 11;
        do{
            set_sprite_prop(SPRITE_ENEMY_16X24_1 + count2 + temp, 0);
        }while(count2--);

        if((*babit).expCount == 0){
            set_sprite_tile(SPRITE_ENEMY_24X32_1+temp, TILE_BABIT_1_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_2+temp, TILE_BABIT_2_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_4+temp, TILE_BABIT_4_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_5+temp, TILE_BABIT_5_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_7+temp, TILE_BABIT_7_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_8+temp, TILE_BABIT_8_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_9+temp, TILE_BABIT_9_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_10+temp, TILE_BABIT_10_F1);
            set_sprite_tile(SPRITE_ENEMY_24X32_11+temp, TILE_BABIT_11_F1);

            (*babit).frame = (*babit).frame+1;

            if((*babit).frame < 40){
                set_sprite_tile(SPRITE_ENEMY_24X32_3+temp, TILE_BABIT_1_F2);//especial
                set_sprite_tile(SPRITE_ENEMY_24X32_6+temp, TILE_BABIT_2_F2);//especial
            }else{
                set_sprite_tile(SPRITE_ENEMY_24X32_3+temp, TILE_BABIT_3_F1);//especial
                set_sprite_tile(SPRITE_ENEMY_24X32_6+temp, TILE_BABIT_6_F1);//especial
                if((*babit).frame == 50){
                   (*babit).frame = 0;
                    //Disparo
                    showBullet(bulletList, maxBullet, (*babit).x - (7 INC_BITS), (*babit).y + 3);
                }
            }
        }else{
            /*
            expCount 1 -> Se pinta frame 1 explosion
            expCount 2 -> Se pinta frame 2 explosion
            expCount 3 -> Se pinta frame 3 explosion
            expCount 4 -> Se Se borran los frames
            */
            if((*babit).expCount < 4){

                set_sprite_tile(SPRITE_ENEMY_24X32_1+temp, TILE_BLANK);
                set_sprite_tile(SPRITE_ENEMY_24X32_2+temp, TILE_BLANK);
                set_sprite_tile(SPRITE_ENEMY_24X32_3+temp, TILE_BLANK);
                set_sprite_tile(SPRITE_ENEMY_24X32_4+temp, TILE_EXP_1_F1 + (4*((*babit).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_24X32_5+temp, TILE_EXP_2_F1 + (4*((*babit).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_24X32_6+temp, TILE_BLANK);
                set_sprite_tile(SPRITE_ENEMY_24X32_7+temp, TILE_EXP_3_F1 + (4*((*babit).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_24X32_8+temp, TILE_EXP_4_F1 + (4*((*babit).expCount-1)));
                set_sprite_tile(SPRITE_ENEMY_24X32_9+temp, TILE_BLANK);
                set_sprite_tile(SPRITE_ENEMY_24X32_10+temp, TILE_BLANK);
                set_sprite_tile(SPRITE_ENEMY_24X32_11+temp, TILE_BLANK);
            //Ha terminado la explosion
            }else{
                count2 = 11;
                do{
                    set_sprite_tile(SPRITE_ENEMY_24X32_1 + count2 + temp, TILE_BLANK);
                }while(count2--);
                (*babit).type = 0;
            }

            if(frame%6==0){
                (*babit).expCount++;
            }
        }
    }
}

void moveSpriteBabit(struct Camera *camera, struct Enemy *babit, UBYTE count){
    UBYTE temp;
    temp = count*11;//11 es el numero de sprites
    if(isInScreen((*camera).scrollX, (*camera).scrollY, ((*babit).x DEC_BITS), (*babit).y, BABIT_WIDTH, BABIT_HEIGHT)
       && (*babit).type != 0){

        move_sprite(SPRITE_ENEMY_24X32_1+temp,((*babit).x DEC_BITS) - (*camera).scrollX +16, ((*babit).y) - (*camera).scrollY  +16);
        move_sprite(SPRITE_ENEMY_24X32_2+temp,((*babit).x DEC_BITS) - (*camera).scrollX +24, ((*babit).y) - (*camera).scrollY  +16);

        move_sprite(SPRITE_ENEMY_24X32_3+temp,((*babit).x DEC_BITS) - (*camera).scrollX +8, ((*babit).y) - (*camera).scrollY  +24);
        move_sprite(SPRITE_ENEMY_24X32_4+temp,((*babit).x DEC_BITS) - (*camera).scrollX +16, ((*babit).y) - (*camera).scrollY  +24);
        move_sprite(SPRITE_ENEMY_24X32_5+temp,((*babit).x DEC_BITS) - (*camera).scrollX +24, ((*babit).y) - (*camera).scrollY  +24);

        move_sprite(SPRITE_ENEMY_24X32_6+temp,((*babit).x DEC_BITS) - (*camera).scrollX +8, ((*babit).y) - (*camera).scrollY  +32);
        move_sprite(SPRITE_ENEMY_24X32_7+temp,((*babit).x DEC_BITS) - (*camera).scrollX +16, ((*babit).y) - (*camera).scrollY  +32);
        move_sprite(SPRITE_ENEMY_24X32_8+temp,((*babit).x DEC_BITS) - (*camera).scrollX +24, ((*babit).y) - (*camera).scrollY  +32);

        move_sprite(SPRITE_ENEMY_24X32_9+temp,((*babit).x DEC_BITS) - (*camera).scrollX +8, ((*babit).y) - (*camera).scrollY  +40);
        move_sprite(SPRITE_ENEMY_24X32_10+temp,((*babit).x DEC_BITS) - (*camera).scrollX +16, ((*babit).y) - (*camera).scrollY  +40);
        move_sprite(SPRITE_ENEMY_24X32_11+temp,((*babit).x DEC_BITS) - (*camera).scrollX +24, ((*babit).y - (*camera).scrollY ) +40);
    }
}

//drawEnemy(&camera, &enemyList[count], count, frame);
void drawEnemy(struct Camera *camera, struct Enemy *enemy, UBYTE count, UBYTE frame){
    if((*enemy).type == POPO){
        drawPopo(camera, enemy, count, frame);
    }
    else if((*enemy).type == GOCHI){
        drawGochi(camera, enemy, count, frame);
    }
    else if((*enemy).type == BABIT){
        drawBabit(camera, enemy, count, frame);
    }
}

void moveSpriteEnemy(struct Camera *camera, struct Enemy *enemy, UBYTE count){
    if((*enemy).type == POPO){
        moveSpritePopo(camera, enemy, count);
    }
    else if((*enemy).type == GOCHI){
        moveSpriteGochi(camera, enemy, count);
    }
    else if((*enemy).type == BABIT){
        moveSpriteBabit(camera, enemy, count);
    }
}

void drawPlayer(struct Player *player, UBYTE isInGround, UBYTE frame){
    UBYTE i;

    if((*player).suffCount != 0){
        (*player).suffCount--;

        for(i = 0; i < 10; i++){
            set_sprite_tile(SPRITE_PLAYER_1+i, TILE_BLANK);
        }
    }
    if((*player).suffCount%3 < 2){
        //Movimiento de los sprites //ANCHOR-> abajo derecha

        //Cabeza
        set_sprite_tile(SPRITE_PLAYER_1, TILE_PLAYER_HEAD_1);
        set_sprite_tile(SPRITE_PLAYER_2, TILE_PLAYER_HEAD_2);
        set_sprite_tile(SPRITE_PLAYER_3, TILE_PLAYER_HEAD_3);
        set_sprite_tile(SPRITE_PLAYER_4, TILE_PLAYER_HEAD_4);

        if(!isInGround){
            //Piernas
            if((*player).state == STATE_ATACK && (*player).frame < 6){
                set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_AIR_1);
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_JUMP_1);
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_AIR_2);
                set_sprite_tile(SPRITE_PLAYER_9, TILE_PLAYER_ATACK_AIR_3);
                set_sprite_tile(SPRITE_PLAYER_10, TILE_PLAYER_ATACK_AIR_4);
            }
            else{
                set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_BODY_2);
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_JUMP_1);
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_JUMP_2);
                set_sprite_tile(SPRITE_PLAYER_9, TILE_BLANK);
                set_sprite_tile(SPRITE_PLAYER_10, TILE_BLANK);
            }
        }
        //Player en el suelo
        else{
            set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
            set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_BODY_2);
            //Tiles especiales reseteados a blanco
            set_sprite_tile(SPRITE_PLAYER_9, TILE_BLANK);//Especial(Sobreesale)
            set_sprite_tile(SPRITE_PLAYER_10, TILE_BLANK);//Especial(Sobreesale)

            if((*player).state == STATE_RUN){
                //Piernas
                (*player).frame = getPlayerFrameRun(frame, (*player).frame);
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_RUN_F1_1 + ((*player).frame<<1));
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_RUN_F1_2 + ((*player).frame<<1));
            }
            else if((*player).state == STATE_IDLE){
               //Piernas
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_FOOTS_2);

            }
            else if((*player).state == STATE_DOWN){
                set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_DOWN_1);
                set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_BODY_2);
                set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_JUMP_1);
                set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_JUMP_2);
                set_sprite_tile(SPRITE_PLAYER_9, TILE_BLANK);
                set_sprite_tile(SPRITE_PLAYER_10, TILE_BLANK);
            }
            else if((*player).state == STATE_ATACK){

                if((*player).frame < 2){
                    //Cuerpo
                    set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                    set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_F1_1);//Especial
                    //Piernas
                    set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                    set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_F1_2);//Especial
                }
                else if((*player).frame < 6){
                //Atack frame 2
                    //Cabeza
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
                else if((*player).frame < 8){
                    //Cuerpo
                    set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                    set_sprite_tile(SPRITE_PLAYER_6, TILE_PLAYER_ATACK_F1_1);//Especial
                    //Piernas
                    set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                    set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_ATACK_F1_2);//Especial
                }
                else{
                    set_sprite_tile(SPRITE_PLAYER_5, TILE_PLAYER_BODY_1);
                    set_sprite_tile(SPRITE_PLAYER_7, TILE_PLAYER_FOOTS_1);
                    set_sprite_tile(SPRITE_PLAYER_8, TILE_PLAYER_FOOTS_2);
                    (*player).state = STATE_IDLE;
                    (*player).frame = 0;
                }
            }
        }
    }
    if((*player).state == STATE_ATACK){
        (*player).frame++;
    }
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

void moveSpritePlayer(struct Camera *camera, struct Player *player, UBYTE isInGround, UBYTE frame){
    UBYTE temp;
    UBYTE temp2;


    if((*player).flip){
        flipPlayer(S_FLIPX);
        temp = 8;
    }else{
        flipPlayer(0);
        temp = 0;
    }
    temp2 = 0;
    //�apa para la animacion idle (Muevo algunos sprites un pixel)
    if(isInGround){
        if((*player).state == STATE_IDLE){
            (*player).frame = getFrameIdle(frame, (*player).frame, 15);
            temp2 = (*player).frame;
        }
    }
    (*player).flip == FALSE;

    //Coloco los sprites en su posicion
    if(isInGround){

        if((*player).state == STATE_ATACK){
                move_sprite(SPRITE_PLAYER_1, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +16 +temp2);
                move_sprite(SPRITE_PLAYER_2, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +16 +temp2);
                move_sprite(SPRITE_PLAYER_3, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +24 +temp2);
                move_sprite(SPRITE_PLAYER_4, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +24 +temp2);
                move_sprite(SPRITE_PLAYER_5, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +32 +temp2);
            if((*player).frame < 3){
                move_sprite(SPRITE_PLAYER_6, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +32);//ESPECIAL
                move_sprite(SPRITE_PLAYER_7, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +40);//ESPECIAL
            }else if((*player).frame < 9){
                //Atack frame 2
                move_sprite(SPRITE_PLAYER_9, ((*player).x DEC_BITS) - (*camera).scrollX +24-(temp*3), (*player).y - (*camera).scrollY  +24);//ESPECIAL
                move_sprite(SPRITE_PLAYER_6, ((*player).x DEC_BITS) - (*camera).scrollX +16-temp, (*player).y - (*camera).scrollY  +32);//ESPECIAL
                move_sprite(SPRITE_PLAYER_10, ((*player).x DEC_BITS) - (*camera).scrollX +24-(temp*3), (*player).y - (*camera).scrollY  +32);//ESPECIAL
            }else if((*player).frame < 12){
                move_sprite(SPRITE_PLAYER_6, ((*player).x DEC_BITS) - (*camera).scrollX +16-temp, (*player).y - (*camera).scrollY  +32);//ESPECIAL
                move_sprite(SPRITE_PLAYER_7, ((*player).x DEC_BITS) - (*camera).scrollX +8+temp, (*player).y - (*camera).scrollY  +40);//ESPECIAL
            }
        }
        //Le agacho un poco la cabeza
        else if((*player).state == STATE_DOWN){
            move_sprite(SPRITE_PLAYER_1, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +16 +8);
            move_sprite(SPRITE_PLAYER_2, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +16 +8);
            move_sprite(SPRITE_PLAYER_3, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +24 +8);
            move_sprite(SPRITE_PLAYER_4, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +24 +8);
            move_sprite(SPRITE_PLAYER_5, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +32 +6);
            move_sprite(SPRITE_PLAYER_6, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +32 +6);
            move_sprite(SPRITE_PLAYER_7, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +40+6);
            move_sprite(SPRITE_PLAYER_8, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +40+6);
        }else{
            move_sprite(SPRITE_PLAYER_1, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +16 +temp2);
            move_sprite(SPRITE_PLAYER_2, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +16 +temp2);
            move_sprite(SPRITE_PLAYER_3, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +24 +temp2);
            move_sprite(SPRITE_PLAYER_4, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +24 +temp2);
            move_sprite(SPRITE_PLAYER_5, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +32 +temp2);
            move_sprite(SPRITE_PLAYER_6, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +32 +temp2);
            move_sprite(SPRITE_PLAYER_7, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +40);
            move_sprite(SPRITE_PLAYER_8, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +40);
        }
    }else{
        move_sprite(SPRITE_PLAYER_1, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +16);
        move_sprite(SPRITE_PLAYER_2, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +16);
        move_sprite(SPRITE_PLAYER_3, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +24);
        move_sprite(SPRITE_PLAYER_4, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +24);
        move_sprite(SPRITE_PLAYER_5, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +32);
        move_sprite(SPRITE_PLAYER_6, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +32);
        move_sprite(SPRITE_PLAYER_7, ((*player).x DEC_BITS) - (*camera).scrollX +8 +temp, (*player).y - (*camera).scrollY +40);
        move_sprite(SPRITE_PLAYER_8, ((*player).x DEC_BITS) - (*camera).scrollX +16 -temp, (*player).y - (*camera).scrollY +40);
        if((*player).state == STATE_ATACK){
            move_sprite(SPRITE_PLAYER_9, ((*player).x DEC_BITS) - (*camera).scrollX +24-(temp*3), (*player).y - (*camera).scrollY  +32);//ESPECIAL
            move_sprite(SPRITE_PLAYER_10, ((*player).x DEC_BITS) - (*camera).scrollX +24-(temp*3), (*player).y - (*camera).scrollY  +40);//ESPECIAL
        }
    }
}

//               &camera,               &enemyList[count],   gochiMapX,    gochiMapY,    NUMBER_ENEMY_MAP, GOCHI
UBYTE spawnEnemy(struct Camera *camera, struct Enemy *enemy, UBYTE *listX, UBYTE *listY, UBYTE arraySize, UBYTE enemyType){

    UBYTE i;
    UBYTE enemyW;

    if((*enemy).type == 0){

        if(enemyType == POPO){
            enemyW = POPO_WIDTH;
        }
        else if(enemyType == GOCHI){
            enemyW = GOCHI_WIDTH;
        }
        else if(enemyType == BABIT){
            enemyW = BABIT_WIDTH;
        }
        for(i = 0; i < arraySize; i++){
            if(listX[i] != 0){
                //Saco bicho
                if((*camera).scrollX + SCREEN_WIDTH + enemyW >= (listX[i]<<3)){
                    (*enemy).type = enemyType;
                    (*enemy).x = listX[i];//Tile donse se spawnea
                    (*enemy).y = listY[i];//Tile donse se spawnea
                    (*enemy).frame = 0;
                    (*enemy).flip = FALSE;
                    (*enemy).expCount = 0;
                    listX[i] = 0;
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

void destroyEnemy(struct Camera *camera, struct Enemy *enemy, UBYTE enemyType){

    UBYTE enemyW;
    UINT16 x;

    x = (*enemy).x << 3;

    if((*enemy).type != 0){
        if(enemyType == POPO){
            enemyW = POPO_WIDTH;
        }
        else if(enemyType == GOCHI){
            enemyW = GOCHI_WIDTH;
        }
        else if(enemyType == BABIT){
            enemyW = BABIT_WIDTH;
        }

        //Elimino el bicho
       if((*camera).scrollX > x + enemyW){
            (*enemy).type = 0;
        }
    }
}

void drawBGXRight(struct Camera *camera, unsigned char *map){

    UINT16 tileX;
    UBYTE tileXMod;
    UBYTE count;
    //Posiciona el indice X en la posicion del vector a rellenar
    //camera.scroll>>3 es la suma de tiles de 8x8 avanzados: camera.scroll/8
    //Ejemplo: el primer tile que se avance sera 1 + 21 = 22
    //Como el tile que queremos rellenar de mas a la derecha se encuentra fuera de la pantalla (20 tiles) sumo 21

    tileX = ((*camera).scrollX>>3)+21;


    //El modulo se debe a que una vez superados los 32 tiles (32*8=256) la cuenta se reseta a 0
    //(Le sigue el 0, no el 33)
    //Es el motivo de porque si el personaje esta enla posicion 257, se tilea a partir de la posici�n 0

    tileXMod = tileX%32;

    //18 son los tiles que entran en la pantalla en alto
    for(count = 0; count != 32; count++ ){
        //Solo relleno el ultimo tile por la derecha
        set_bkg_tiles(tileXMod, count, 1, 1, &(map+tileX));
        //Incrementa con con el tama�o de X para que se cargue la proxima fila
        tileX = tileX + MAP_SIZE_X;
    }

    /*
    UINT16 count;
    UINT16 tileX;
    UINT16 indXY;
    UINT16 indY;

    tileX = ((*camera).scrollX>>3)+21;

    indY = ((*camera).scrollY>>3);
    indXY = (indY * MAP_SIZE_X);


    //18 son los tiles que entran en la pantalla en alto
    for(count = indY; count != (20+indY); count++ ){
        //Solo relleno el ultimo tile por la derecha
        set_bkg_tiles((tileX%32), count, 1, 1, &(map+tileX+indXY));

        //Incrementa con con el tama�o de X para que se cargue la proxima fila
        tileX = tileX + MAP_SIZE_X;
    }
    */
}

void drawBGXLeft(struct Camera *camera, unsigned char *map){

    UINT16 tileX;
    UBYTE tileXMod;
    UBYTE count;

    tileX = ((*camera).scrollX>>3)-1;
    tileXMod = tileX%32;

    for(count = 0; count != 32; count++ ){
        set_bkg_tiles(tileXMod, count, 1, 1, &(map+tileX));
        tileX = tileX + MAP_SIZE_X;
    }
}

/*
void drawBGYDown(struct Camera *camera, unsigned char *map){
    UINT16 count;
    UINT16 tileY;
    UINT16 indXY;
    UINT16 indX;

    tileY = ((*camera).scrollY>>3)+19;

    indX = ((*camera).scrollX>>3);
    indXY = indX + (tileY * MAP_SIZE_X);

    //20 son los tiles que entran en la pantalla en ancho
    for(count = indX; count != (22+indX); count++ ){
        //Solo relleno el ultimo tile por la derecha
        set_bkg_tiles((count%32), (tileY%32), 1, 1, &(map+indXY));
        //Incrementa con con el tama�o de X para que se cargue la proxima fila
        indXY = indXY + 1;
    }
}

void drawBGYUp(struct Camera *camera, unsigned char *map){
    /*
    UINT16 count;
    UINT16 tileY;
    UINT16 indXY;
    UINT16 indX;

    tileY = ((*camera).scrollY>>3)-1;

    indX = ((*camera).scrollX>>3);
    indXY = indX + (tileY * MAP_SIZE_X);

    //20 son los tiles que entran en la pantalla en ancho
    for(count = indX; count != (22+indX); count++ ){
        //Solo relleno el ultimo tile por la derecha
        set_bkg_tiles((count%32), (tileY%32), 1, 1, &(map+indXY));
        //Incrementa con con el tama�o de X para que se cargue la proxima fila
        indXY = indXY + 1;
    }
}
*/


void main() {

    //Mierda temporal
    //printf("%d",3);
    //printf("Inicio del programa");

    //Declaracion
    UBYTE gameState;
    UBYTE tileX;

    UBYTE frame;
    UINT16 temp;
    UBYTE count;

    BYTE screenCountX;
    BYTE screenCountY;

    UBYTE isInGround;
    UBYTE isLastInGround;

    unsigned char *map;
    UBYTE gochiMapX;
    UBYTE gochiMapY;
    UBYTE popoMapX;
    UBYTE popoMapY;
    UBYTE babitMapX;
    UBYTE babitMapY;

    UBYTE keys;
    UBYTE keyB_Up;
    UBYTE keyB_Down;
    UBYTE keyA_Up;
    UBYTE keyA_Down;

    UINT16 newX;
    UBYTE newY;

    char digitPoints[5];
    struct Platform platformList[NUMBER_PLATFORM_MAP];
    struct Enemy enemyList[MAX_ENEMY];
    struct Bullet bulletList[MAX_BULLET];
    struct Player player;
    struct Camera camera;

    //Asignacion
    gameState = GAME_STATE_PLAY;

    keyB_Up = TRUE;
    keyB_Down = FALSE;
    keyA_Up = TRUE;
    keyA_Down = FALSE;
    isInGround = FALSE;
    isLastInGround = FALSE;

    //Se carga el mapa correcto
    map = &LEVEL1;
    gochiMapX = &gochiLevel_1_X;
    gochiMapY = &gochiLevel_1_Y;
    popoMapX = &popoLevel_1_X;
    popoMapY = &popoLevel_1_Y;
    babitMapX = &babitLevel_1_X;
    babitMapY = &babitLevel_1_Y;

    tileX = 20;
    temp = 0;
    frame = 0;
    screenCountX = 16;
    screenCountY = 16;

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
    set_sprite_tile(SPRITE_DIGIT_5, TILE_0);
    //set_bkg_tiles(indexX(8 pixelex), indexY(8 pixelex), ancho, alto y mapa)
    //Viejo sistema, hasta 32 tiles me vale...
    //set_bkg_tiles(0, 0, MAP_SIZE_X, MAP_SIZE_Y, map);


    /**
    El bucle rellena la pantalla de tiles de arriba a abajo (Se recorre en Y),
    haciendo una fila completa de 22 tiles (los que entran en la pantalla)
    Al final de cada iteracion mueve el puntero 64 tiles (El tama�o en X del mapa)
    para que en la siguiente iteracion apunte a la nueva fila de la matriz
    */

    SWITCH_ROM_MBC1(2);//Banco de memoria 4


    /**
    Relleneo de tiles desde la parte superior izquierda la pantalla de tiles, todos los
    que quepan en x, y mas dos mas de margen (x-> 160/8 = 20 + 2 y-> 144/8 = 18 + 2)
    */
    //Seria 32 pero solo quiero llenar la parte visible de la pantalla (144 pixels = 18 tiles)
    for(count = 0; count != 32; count++ ){//18 son los tiles que entran en la pantalla en alto

        //Pinta una fila de tiles
        //El mapa maximo en memoria es 32 x 32
        //22 son los tiles que entran en la pantalla en ancho (160 pixels = 20 tiles)
        set_bkg_tiles(0, count, 22, 1, &(map+temp));

         //Incrementa temp con el tama�o X del mapa para que se cargue la proxima fila
		temp = temp + MAP_SIZE_X;
    }

    //set_bkg_tiles(0, 0, 22, 18, &map);

    SHOW_BKG;//Muestra el fondo
    //WIN se suporpone a BKG y usa sus mismos tiles, para dejar ver a BKG hay que moverla primero:
    //SHOW_WIN;
    SHOW_SPRITES;//Muestra los sprites
    DISPLAY_ON;
    enable_interrupts();

    //Inicializacion del player y los enemigos
    player.x = (PLAYER_WIDTH>>1) INC_BITS;
    player.y = 0;
    //camera.scrollY = (1*8);
    player.flip = FALSE;
    newX= player.x;
    newY = player.y;
    player.state = STATE_IDLE;
    player.suffCount = 0;

    count = NUMBER_PLATFORM_MAP-1;
    do{
        platformList[count].x = platformLevel_1_X[count];
        platformList[count].y = platformLevel_1_Y[count] << 3;
        platformList[count].path = platformLevel_1_Path[count];
    }
    while(count--);

    count = MAX_ENEMY-1;
    do{
        enemyList[count].type = 0;
    }
    while(count--);

    count = MAX_ENEMY-1;
    do{

        //spawnEnemy(&camera, &enemyList[count], popoMapX, popoMapY, NUMBER_ENEMY_MAP, POPO);
        spawnEnemy(&camera, &enemyList[count], gochiMapX, gochiMapY, NUMBER_ENEMY_MAP, GOCHI);
        //spawnEnemy(&camera, &enemyList[count], babitMapX, babitMapY, NUMBER_ENEMY_MAP, BABIT);

    }
    while(count--);

    drawString("POINTS WI ", 1, 0, 1);
    drawString("POINTS BG ", 1, 0, 0);

    while(TRUE) {

        /**
        Gestion del game pad
        */
        /**
        Sincronizacion con el blanqueo de la pantalla.

        Espera a que la pantalla esta lista para para pintarse (Se detiene la l�gica de los calculos)
        En la GameBoy mientras se hace el blanqueo de la pantalla no se puede acceder a la memoria.
        De esta manera, una vez acaba el blanqueo de la memoria, se estable el peque�o tiempo en el que
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
            //Poner por defect a 1 y ralentizar la plataforma
            player.velocityAsc = 0;
            player.velocityDesc += getGravitySpeed(GRAVITY, PLAYER_WEIGHT);
            /*
            Este ajuste se debe a que es aconsejable que la velocidad de caida sea superior
            a la velocidad de descenso de las plataformas m�viles, que es de 1.
            */
            if(player.velocityDesc < (2 INC_BITS)){
                player.velocityDesc = (2 INC_BITS);
            }
            /*
            Este otro ajuste se debe a que si la velocidad de caida es muy elevada,
            puede romper las colisiones
            */
            if(player.velocityDesc > (6 INC_BITS)){
                player.velocityDesc = (6 INC_BITS);
            }
            newY +=(player.velocityDesc DEC_BITS);
        }

        /**
        Inputs
        */
        if(keyPressSTART(keys)){}
        if(keyPressSELECT(keys)){}
        //Pad
        if(keyPressUP(keys)) {
        }
        else if(keyPressDOWN(keys)) {
            if(player.state != STATE_ATACK && isInGround){
                player.state = STATE_DOWN;
            }
        }
        else if(keyPressLEFT(keys)) {
            if(player.state == STATE_IDLE || player.state == STATE_RUN || !isInGround){
                if(!isInGround){
                    //Si se gira en el aire pierde el ataque
                    if(!isInGround && player.flip == FALSE){
                        player.state = STATE_IDLE;
                    }
                }else{
                    player.state = STATE_RUN;
                }
                player.flip = TRUE;
                newX -=  PLAYER_SPEED;
            }
        }
        else if(keyPressRIGHT(keys)) {
            if(player.state == STATE_IDLE || player.state == STATE_RUN || !isInGround){
                if(!isInGround){
                    //Si se gira en el aire pierde el ataque
                    if(!isInGround && player.flip == TRUE){
                        player.state = STATE_IDLE;
                    }
                }else{
                    player.state = STATE_RUN;
                }
                player.flip = FALSE;
                newX +=  PLAYER_SPEED;
            }
        }
        else{
            if(player.state != STATE_ATACK){
                player.state = STATE_IDLE;
            }
        }
        if(keyA_Down) {
            if(player.state != STATE_ATACK){
                if(isInGround){
                    player.state = STATE_ATACK;
                }else{
                    //Solo puede golpear una vez en el aire
                    if(player.airAtack == TRUE){
                        player.state = STATE_ATACK;
                        player.airAtack = FALSE;
                    }
                }
                player.frame = 0;
            }
        }
        if(keyB_Down) {
            if(isInGround && player.state != STATE_ATACK){
                player.velocityAsc = getStrongJump(PLAYER_JUMP INC_BITS);
                player.airAtack = TRUE;
            }
        }

        /**
        Colisiones player
        */
        if(isCollisionDown(player.x DEC_BITS, newY, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, map)){
            newY = (((isCollisionDown(player.x DEC_BITS, newY, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, map) / MAP_SIZE_X) << 3) - PLAYER_HEIGHT);
            player.velocityDesc = 0;
        }
        if(isCollisionUp(player.x DEC_BITS, newY, PLAYER_WIDTH, MAP_SIZE_X, map)){
            newY = (((isCollisionUp(player.x DEC_BITS, newY, PLAYER_WIDTH, MAP_SIZE_X, map) / MAP_SIZE_X) << 3) + 8);
            player.velocityAsc = 0;//En el FK2 iba de lujo
            player.velocityDesc = 0;
        }
        player.y = newY;
        if(isCollisionRight(newX DEC_BITS, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, map)){
            newX = (((isCollisionRight(newX DEC_BITS, player.y, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, map) % MAP_SIZE_X) << 3)-PLAYER_WIDTH) INC_BITS;
        }
        if(isCollisionLeft(newX DEC_BITS, player.y, PLAYER_HEIGHT, MAP_SIZE_X, map)){
            newX = (((isCollisionLeft(newX DEC_BITS, player.y, PLAYER_HEIGHT, MAP_SIZE_X, map) % MAP_SIZE_X) << 3)+8) INC_BITS;
        }
        player.x = newX;


        //Set limits screen
        if(player.x < (camera.scrollX+1) INC_BITS){
            player.x = ((camera.scrollX+1) INC_BITS);
        }
        else if((player.x DEC_BITS) + PLAYER_WIDTH >= LEVEL_WIDTH){
            player.x = (LEVEL_WIDTH - PLAYER_WIDTH) INC_BITS;
        }

        /**
        Logica enemigos
        */
        /*
        count = MAX_ENEMY-1;
        do{
            moveEnemy(&camera, &enemyList[count], map);
            checkEnemyDamage(&camera, &player, &enemyList[count], isInGround);
            if(enemyList[count].expCount == 0){
                checkPlayerDamage(&camera, &player, &enemyList[count]);
            }
        }while(count--);
        */
        //Bullet
        count = MAX_BULLET-1;
        do{
            moveBullet(&camera, &bulletList[count], BULLET_WIDTH, BULLET_HEIGHT, BULLET_SPEED, map);
            if(bulletList[count].active){
                if(checkPlayerDamage(&camera, &player, bulletList[count].x, bulletList[count].y, BULLET_WIDTH, BULLET_HEIGHT)){
                    bulletList[count].active = FALSE;
                }
            }
        }while(count--);

        //Platform
        count = NUMBER_PLATFORM_MAP-1;
        temp = FALSE;
        do{
            if(temp == FALSE){
                temp = updatePlatform(&platformList[count], &player, map);
            }else{
                updatePlatform(&platformList[count], &player, map);
            }
        }while(count--);

        //Cambio de suelo a aire
        isLastInGround = isCollisionDown(player.x DEC_BITS, player.y + 1, PLAYER_WIDTH, PLAYER_HEIGHT, MAP_SIZE_X, map) || temp;
        //En el frame anterior estaba en el suelo y ahora en el aire o viceversa
        if(isInGround != isLastInGround){
            isInGround = isLastInGround;
            player.state = STATE_IDLE;
            player.frame = 0;
        }


        wait_vbl_done();

        /**
        Actualizacion de la camara
        */
        //Actualizo los valores de la camara
        camera.lastX = camera.scrollX;
        camera.scrollX = getScrollX(player.x DEC_BITS);
        screenCountX += (camera.scrollX-camera.lastX);
        //
        camera.lastY = camera.scrollY;
        camera.scrollY = getScrollY(player.y);
        screenCountY += (camera.scrollY-camera.lastY);

        temp = (20 + (camera.scrollX >> 3));
        if(tileX < temp){
            tileX = temp;
        }

        /**
        Pintado enemigos
        */
        for(count = 0; count < MAX_ENEMY; count++){
            drawEnemy(&camera, &enemyList[count], count, frame);
            moveSpriteEnemy(&camera, &enemyList[count], count);
        }
        /*
        La plataforma usa los mismos sprites que el Gochi, por esto mismo se debe de pintar despues
        de este.
        */
        //Platform
        for(count = 0; count < NUMBER_PLATFORM_MAP; count++){
            drawPlatform(&camera, &platformList[count], count);
            moveSpritePlatform(&camera, &platformList[count], count);
        }

        /**
        Pintado Player
        */
        drawPlayer(&player, isInGround, frame);
        moveSpritePlayer(&camera, &player, isInGround, frame);

        //Movimiento del fondo
        move_bkg(camera.scrollX, camera.scrollY);
        //scroll_bkg(camera.scroll, 0);//Es automatico
        //Al avanzar un tile se actualiza el mapa
        if(screenCountX >= 24){

            screenCountX = (16 + (screenCountX - 24));

            count = MAX_ENEMY-1;
            do{
                //if(e->type == 0){//No esta ocupado

                //spawnEnemy(&camera, &enemyList[count], popoMapX, popoMapY, NUMBER_ENEMY_MAP, POPO);
                spawnEnemy(&camera, &enemyList[count], gochiMapX, gochiMapY, NUMBER_ENEMY_MAP, GOCHI);
                //spawnEnemy(&camera, &enemyList[count], babitMapX, babitMapY, NUMBER_ENEMY_MAP, BABIT);

                //destroyEnemy(&camera, &enemyList[count], POPO);
                //destroyEnemy(&camera, &enemyList[count], GOCHI);
                //destroyEnemy(&camera, &enemyList[count], BABIT);

            }
            while(count--);
            drawBGXRight(&camera, map);
        }
        else if(screenCountX <= 8){
            screenCountX = (16 - (8-screenCountX));
            /*
            count = MAX_ENEMY-1;
            do{
                //if(e->type == 0){//No esta ocupado
                if(enemyList[count].expCount == 5){
                    spawnEnemy(&camera, &enemyList[count], popoMapX, popoMapY, NUMBER_ENEMY_MAP, POPO);
                    spawnEnemy(&camera, &enemyList[count], gochiMapX, gochiMapY, NUMBER_ENEMY_MAP, GOCHI);
                    spawnEnemy(&camera, &enemyList[count], babitMapX, babitMapY, NUMBER_ENEMY_MAP, BABIT);
                }
            }
            while(count--);
            */
            drawBGXLeft(&camera, map);
        }
        /*
        if(screenCountY >= 24){
            screenCountY = (16 + (screenCountY-24));
            drawBGYDown(&camera, map);
        }
        else if(screenCountY <= 8){
            screenCountY = (16 - (8-screenCountY));
            drawBGYUp(&camera, map);
        }
        */


        //Solo refresco cada 30 frames
        //move_win(0, 136);
        if(frame%10 == 0){
            drawPoints(tileX, 5, SCREEN_WIDTH - (8*4), 16);//hasta 256
        }

        frame++;

    }
}

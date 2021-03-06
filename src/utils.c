#include <gb/gb.h>
#include "utils.h"
#include "constans.h"

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

UBYTE blink01(UBYTE value){
    if(!value){
        return TRUE;
    }else{
        return FALSE;
    }
}

UINT16 getScrollX(UINT16 objX) {
    UINT16 scX;
    scX = 0;
    /*
    La camara se mueve cuando se pasa de la mitad de la pantalla
    La segunda condicion evita que se mueva hacia la izquierda
    */
    if(objX > SCREEN_WIDTH4){
        scX = objX - SCREEN_WIDTH4;
    }
    //Limites
    if(objX < SCREEN_WIDTH4){
        scX = 0;
    }
    else if(scX > LEVEL_WIDTH - SCREEN_WIDTH){
        scX = LEVEL_WIDTH - SCREEN_WIDTH;
    }
    return scX;
}

UINT16 getScrollY(UINT16 objY) {
    UINT16 scY;
    scY = 0;
    if(objY > SCREEN_HEIGHT4){
        scY = objY - SCREEN_HEIGHT4;
    }
    //Limites
    if(objY < SCREEN_HEIGHT4){
        scY = 0;
    }
    else if(scY > LEVEL_HEIGHT - SCREEN_HEIGHT){
        scY = LEVEL_HEIGHT - SCREEN_HEIGHT;
    }

    return scY;
}

UBYTE isInScreen(UINT16 scrollX, UINT16 scrollY, UINT16 x, UINT16 y, UINT16 width, UINT16 height){
    if((x < scrollX + SCREEN_WIDTH && x + width > scrollX) && (y < scrollY + SCREEN_HEIGHT && y + height > scrollY)){
        return TRUE;
    }else{
        return FALSE;
    }
}

UBYTE getFrameIdle(UBYTE gameFrame, UBYTE characterFrame, UBYTE speed) {
    UBYTE f =characterFrame;
    if(gameFrame%speed == 0) {
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

/**
Chequea si el objeto 1 colisiona por debajo sobre el objeto 2
Develve la posicion y del objeto 2 si colisiona. 0 en cualquier otro caso
*/
UBYTE checkCollisionDown(UINT16 x1, UBYTE y1, UBYTE w1, UBYTE h1, UINT16 x2, UBYTE y2, UBYTE w2, UBYTE h2){
    if(x1 + w1 > x2 && x1 < x2 + w2){
        if(y1 + h1 > y2 && y1 + h1 < y2 + h2){
            return y2;
        }
    }
    return FALSE;
}

/**
Chequea si el objeto 1 colisiona por arriba sobre el objeto 2
Develve la posicion y del objeto 1 si colisiona. 0 en cualquier otro caso
*/
UBYTE checkCollisionUp(UINT16 x1, UBYTE y1, UBYTE w1, UINT16 x2, UBYTE y2, UBYTE w2, UBYTE h2){
    if(x1 + w1 > x2 && x1 < x2 + w2){
        if(y1 < y2 + h2 && y1 > y2){
            return y1;
        }
    }
    return FALSE;
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

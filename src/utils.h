#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

void drawPoints(UINT16 points, UINT8 digits, UINT8 positionX, UINT8 positionY);

void drawNumbers(UINT8 digits, UINT16 number, UINT8 positionX, UINT8 positionY);

UINT8 getNumberDigits(UINT16 number, UINT8 c);

UINT8 getDigit(UINT16 number, UINT8 digit);

UBYTE blink01(UBYTE value);

UINT16 getScrollX(UINT16 objX);

UINT16 getScrollY(UINT16 objY);

UBYTE isInScreen(UINT16 scrollX, UINT16 scrollY, UINT16 x, UINT16 y, UINT16 width, UINT16 height);

UBYTE getFrameIdle(UBYTE gameFrame, UBYTE characterFrame, UBYTE speed);

UBYTE getPlayerFrameRun(UBYTE gameFrame, UBYTE playerFrame);

UBYTE checkCollision(UINT16 x1, UBYTE y1, UBYTE w1, UBYTE h1, UINT16 x2, UBYTE y2, UBYTE w2, UBYTE h2);

/**
Chequea si el objeto 1 colisiona por debajo sobre el objeto 2
Develve la posicion y del objeto 2 si colisiona. 0 en cualquier otro caso
*/
UBYTE checkCollisionDown(UINT16 x1, UBYTE y1, UBYTE w1, UBYTE h1, UINT16 x2, UBYTE y2, UBYTE w2, UBYTE h2);

/**
Chequea si el objeto 1 colisiona por arriba sobre el objeto 2
Develve la posicion y del objeto 1 si colisiona. 0 en cualquier otro caso
*/
UBYTE checkCollisionUp(UINT16 x1, UBYTE y1, UBYTE w1, UINT16 x2, UBYTE y2, UBYTE w2, UBYTE h2);

UINT8 getGravitySpeed(UINT16 gravityForce, UINT16 weight);

UINT8 getStrongJump(UINT16 strongJump);


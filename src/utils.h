#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

UINT8 getNumberDigits(UINT16 number, UINT8 c);

UINT8 getDigit(UINT16 number, UINT8 digit);

UBYTE blink01(UBYTE value);

UINT16 getScroll(UINT16 objX, UINT16 scrollX);

UBYTE isInScreen(UINT16 scrollX, UINT16 x, UINT16 width);

UBYTE getFrameIdle(UBYTE gameFrame, UBYTE characterFrame, UBYTE speed);

UBYTE getPlayerFrameRun(UBYTE gameFrame, UBYTE playerFrame);

UBYTE checkCollision(UINT16 x1, UBYTE y1, UBYTE w1, UBYTE h1, UINT16 x2, UBYTE y2, UBYTE w2, UBYTE h2);

UINT8 getGravitySpeed(UINT16 gravityForce, UINT16 weight);

UINT8 getStrongJump(UINT16 strongJump);


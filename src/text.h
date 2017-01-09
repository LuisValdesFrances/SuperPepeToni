#ifndef TEXT_H_INCLUDED
#define TEXT_H_INCLUDED

const unsigned char TEXT_TITLE[] = "SUPER PEPETONI";

/**
Rellena la pantalla de tiles vacios
*/
void emptyScreen(void);
/**
Dibuja la cadena de str[] en la poscion x, y en BKY.
Si flag es 0 en BG y si flag es 1 en WIN
Si una palabra no entra en la linea, la dibuja en la siguiente
*/
void drawString( char *str, UBYTE x, UBYTE y, UBYTE flag );

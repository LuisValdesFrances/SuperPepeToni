#include <gb/gb.h>
#include "font.h"

extern const unsigned char FONT_TILES[];//Informacion de los tiles (imagenes)

// linea 'vacia' para limpiar el BKG
static const unsigned char EMPTY_LINE[] =
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/**
Rellena la pantalla de tiles vacios
*/
void emptyScreen(void){

    UBYTE temp1;

    HIDE_BKG;
    HIDE_SPRITES;

    VBK_REG = 0;

    for( temp1 = 0; temp1 < 40; temp1++){
        move_sprite(temp1,0,200);
        set_sprite_prop (temp1,0);
    }

    for( temp1 = 0; temp1 < 20; temp1++){

        set_bkg_tiles (0,temp1,20,1,(unsigned char *) &EMPTY_LINE[0]);
        set_win_tiles (0,temp1,20,1,(unsigned char *) &EMPTY_LINE[0]);

        VBK_REG = 1;
        set_bkg_tiles (0,temp1,20,1,(unsigned char *) &EMPTY_LINE[0]);
        set_win_tiles (0,temp1,20,1,(unsigned char *) &EMPTY_LINE[0]);
        VBK_REG = 0;
    }

    move_bkg (0,0);

    SHOW_BKG;
    SHOW_SPRITES;
    DISPLAY_ON;
}

/**
Dibuja la cadena de str[] en la poscion x, y en BKY.
Si flag es 0 en BG y si flag es 1 en WIN
Si una palabra no entra en la linea, la dibuja en la siguiente
*/
void drawString( char *str, UBYTE x, UBYTE y, UBYTE flag ){

    UBYTE posx, strx, nextlen;
    UBYTE posy, e;
    UBYTE c;

    strx = 0;
    posx = x;
    posy = y;

    // mientras la cadena no termine
    while(str[strx] != 0){

        // comprueba la longitud de la palabra actual de la cadena
        nextlen = 0;
        for( e = strx; (str[e] != ' ') && (str[e] != 0); e++)
            nextlen++;

        // si no cabe en pantalla bajamos de linea
        if ( posx + nextlen > 20){
            posx = 0;
            posy++;
        }

        // dibuja la palabra en la posicion que corresponda
        for(;(str[strx] != ' ') && (str[strx] != 0); strx++){
            c = str[strx] + FONT_OFFSET - 32;
            if( !flag )
                set_bkg_tiles( posx, posy, 1, 1, (unsigned char *) &c);
            else
                set_win_tiles( posx, posy, 1, 1, (unsigned char *) &c);
            posx++;
        }

        // dibuja un espacio en blanco entre las palabras de la cadena
        if ( posx != 0 ){
            c = ' ' + FONT_OFFSET - 32;
            if( !flag )
                set_bkg_tiles( posx, posy, 1, 1, (unsigned char *) &c);
            else
                set_win_tiles( posx, posy, 1, 1, (unsigned char *) &c);
            posx++;
        }

        // si no es el final de la cadena incrementamos la posicion
        if(str[strx] != 0 )
          strx++;
    }
}




del SuperPepeToni.gb
cd src 

lcc -o SuperPepeToni.gb main.c font.c keys.c collision.c spriteTiles.c mapTiles.c enemyPosition.c utils.c text.c level1.c

del *.o
move SuperPepeToni.gb ../SuperPepeToni.gb

pause
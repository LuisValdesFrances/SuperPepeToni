
del SuperPepeToni.gb
cd src 

#lcc -Wa-l -Wf-bo2 -c -o spriteTiles.o spriteTiles.c 
#lcc -Wa-l -Wf-bo2 -c -o mapTiles.o mapTiles.c
#lcc -Wa-l -Wf-bo2 -c -o maps1.o maps1.c

lcc -Wa-l -c -o spriteTiles.o spriteTiles.c 
lcc -Wa-l -c -o mapTiles.o mapTiles.c
lcc -Wa-l -c -o maps1.o maps1.c
lcc -Wa-l -c -o main.o main.c
lcc -Wa-l -c -o keys.o keys.c
lcc -Wa-l -c -o collision.o collision.c
lcc -Wa-l -c -o enemyPosition.o enemyPosition.c
lcc -Wa-l -c -o utils.o utils.c
lcc -Wa-l -c -o text.o text.c



lcc -Wl-yt3 -Wl-yo8 -o SuperPepeToni.gb keys.o collision.o enemyPosition.o utils.o text.o main.o spriteTiles.o mapTiles.o maps1.o
pause


del *.o
del *.lst
move SuperPepeToni.gb ../SuperPepeToni.gb


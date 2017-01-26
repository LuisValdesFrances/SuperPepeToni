lcc -c -o main.o main.c keys.o keys.c collision.o collision.c enemyPosition.o enemyPosition.c utils.c utils.o text.c text.o
lcc -Wf-bo2 -c -o mapTiles.o mapTiles.c
lcc -Wf-bo3 -c -o spriteTiles.o spriteTiles.c
lcc -Wf-bo4 -c -o maps.o maps.c

lcc -Wl-yt1 -Wl-yo8 -o SuperPepeToni.gb main.o keys.o collision.o mapTiles.o spriteTiles.o maps.o
pause
del *.o
--del *.lst
bgb.exe SuperPepeToni.gb
// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

int TILE_INVISIBLE = 0; // el que tengas tu en tu juego
void set_sprite_tile(int i, int j) {}
void move_sprite(int i, int j, int f) {}

class SpriteManager {
private:
	const int GB_MAX_SPRITES_DATA = 40 * 3;
	unsigned char data[80 * 3]; // 80 sprites * (index, x, y)
	unsigned int dindex;
	unsigned int index;
	unsigned char counter;

public:

	void CleanSpriteBuffer() {
		counter = 1 - counter;
		index = 0;

		if (counter == 0) {
			dindex = 0;
		}
		else {
			dindex = (80*3)-1;
		}

	}

	void SetSprite(char _iIndex, char _iX, char _iY) {
		if (index < 80) {
			if (counter == 0) {
				data[dindex++] = _iIndex;
				data[dindex++] = _iX;
				data[dindex++] = _iY;
			}
			else {
				data[dindex--] = _iY;
				data[dindex--] = _iX;
				data[dindex--] = _iIndex;
			}
			index++;
		}
	}

	void DrawSprites() {
		// delete tiles that won't be used
		if (index < 40) {
			for (int i = index; i < 40; i++) {
				set_sprite_tile(i, TILE_INVISIBLE);
			}
		}

		int iSpritesToCopy = index;
		if (iSpritesToCopy > 40) {
			iSpritesToCopy = 40;
		}

		// insert data
		if (counter == 0) {
			dindex = 0;
			for (int i = 0; i < iSpritesToCopy; i++) {
				set_sprite_tile(i, data[dindex++]);
				move_sprite(i, data[dindex++], data[dindex++]);
			}
		}
		else {
			dindex++;
			for (int i = 0; i < iSpritesToCopy; i++) {
				set_sprite_tile(i, data[dindex++]);
				move_sprite(i, data[dindex++], data[dindex++]);
			}
		}
	}
};

int main()
{
	SpriteManager sm = SpriteManager();

	while (1) {

		sm.CleanSpriteBuffer();

		// update del juego
		// hay que llamar a "sm.SetSprite" al final del update/run de cada objeto que tenga un sprite asignado
		//.....

		sm.SetSprite(1, 50, 40);
		sm.SetSprite(2, 70, 70);
		sm.SetSprite(3, 90, 90);

		// allocate sprites buffer to the 
		sm.DrawSprites();
	}
	return 0;
}


/*
Alan Ni
Game


*/

#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include "DungeonGame.h"

int main(int argc, char *argv[])
{
	DungeonGame game(1600, 900);


	game.gameLoop();

	return 0;
}
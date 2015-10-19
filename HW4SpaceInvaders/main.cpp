/*
	Alan Ni
	Space Invaders: Invasion of the Spiders

	Throw spiders at girls and win points!
	Dodge stuff from the rain girls or lose!

	(Sometimes it takes really long to compile, no idea if it's on my side or something wrong with code)
*/

#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include "SpaceInvaderGame.h"


int main(int argc, char *argv[]){
	
	SpaceInvaderGame game(1600, 900);

	game.gameLoop();

	return 0;
}
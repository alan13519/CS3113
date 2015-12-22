#pragma once
#include "SDLandOpenGL.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Map.h"
#include "Entity.h"
#include "Projectile.h"

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class DungeonGame
{
public:
	DungeonGame(GLuint passedScreenWidth, GLuint passedScreenHeight);
	~DungeonGame();
	//Main game loop
	void gameLoop();

	GLuint loadTexture(const char *image_path);

	void render();
	void update();

	//The characters and objects in the game
	Player* player;
	std::vector<Enemy*> enemies;
	std::vector<Projectile*> playerProjectiles;
	std::vector<Projectile*> enemyProjectiles;

	//Start screen Logo
	Room* startScreen;

	Map* currentMap;

	//Mouse positions
	GLfloat mouseX;
	GLfloat mouseY;

	//SDL event
	SDL_Event event;

	//All the sound
	Mix_Music* dungeonMusic;
	Mix_Chunk* shootSound;
	Mix_Chunk* explodeSound;
private:
	//-------------Variables---------------
	//Shader program, display, and context
	ShaderProgram* shaderProgram;
	SDL_Window* displayWindow;
	SDL_GLContext context;

	//Application settings
	GLuint screenWidth;
	GLuint screenHeight;
	GLboolean quit;

	//Matrices
	Matrix viewMatrix;
	Matrix projectionMatrix;

	//Time keeping float
	GLfloat lastFrameTicks;

	//Textures
	GLuint spriteTexture;

	//Ortho Projection coordinates
	GLfloat orthoProjectionLeft;
	GLfloat orthoProjectionRight;
	GLfloat orthoProjectionBottom;
	GLfloat orthoProjectionTop;

	//-------------Functions---------------
	//Sets up sprites and initial lobby
	void initialSetup();
	//Generate dungeons
	void generateLevel();


	//Update functions
	void mainMenuUpdate();
	void innLevelUpdate();
	void dungeonLevelUpdate();
	void gameOverUpdate();

	//Render functions
	void mainMenuRender();
	void innLevelRender();
	void dungeonLevelRender();
	void gameOverRender();

	//Spawns enemies a certain distance away from the players
	void spawnEnemies();
};



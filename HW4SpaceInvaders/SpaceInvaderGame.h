#pragma once
#include "SDLandOpenGLSetup.h"
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"
//#include "GameStates.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class SpaceInvaderGame
{
public:
	SpaceInvaderGame(GLuint passedScreenWidth, GLuint passedScreenHeight);
	~SpaceInvaderGame();
	
	//Main game loop
	void gameLoop();

	void drawText(std::string text, GLfloat size, GLfloat spacing);
	GLuint loadTexture(const char *image_path);
	
	void render();
	void update();

	//Displays all the text for the game
	void displayText(std::string text, ShaderProgram* shaderProgram, GLfloat x_position, GLfloat y_position);

	//Calls entity
	void spriteSetUp();

	//check victory
	GLboolean checkVictory();

private:

	//ShaderProgram* shaderProgram; //Tries to access a nullptr for some reason, no idea why, scrapped this and had a workaround
	SDL_Window* displayWindow;
	SDL_GLContext context;

	//Texture sheets
	GLuint spriteTexture;
	GLuint fontText;

	//Application settings
	GLuint screenWidth;
	GLuint screenHeight;
	GLboolean quit;

	ShaderProgram* shaderProgram;

	//Three Game States implemented as classes (Wasn't good to implement as classes, so changed)
	//GameStates mainMenu;
	//GameStates gameLevel;
	//GameStates gameOver;

	//Reset everything but the score
	void reset();

	//Matrices
	Matrix modelMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;

	//Game states as a function instead
	void mainMenuRender();
	void gameLevelRender();
	void gameOverRender();

	//Game updates
	void mainMenuUpdate();
	void gameLevelUpdate();
	void gameOverUpdate();

	//Enemy movements
	void enemyMovement(GLfloat elapsed);

	//Score keeper
	GLuint score;
	//GLuint highScore; //Local high score, because I'm lazy

	//Entities
	Entity player;
	std::vector<Entity> enemies;
	std::vector<Entity> playerSpiders;
	std::vector<Entity> enemySpiders;

	//displays win or lose
	std::string winSituation; //Can never win, just like in real life

	//Time keeping float
	GLfloat lastFrameTicks;

	//Last direction gone -1 for left border 1 for right border
	GLuint enemyDirection;




};


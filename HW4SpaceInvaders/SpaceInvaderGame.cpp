#include "SpaceInvaderGame.h"

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_GAME_OVER};

int state;

//Max amount of the combined total enemy and player bullets out at any time
//const GLuint MAX_SPIDERS = 50; 

SpaceInvaderGame::SpaceInvaderGame(GLuint passedScreenWidth, GLuint passedScreenHeight)
	: screenHeight(passedScreenHeight), screenWidth(passedScreenWidth), quit(false)
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = nullptr;
	displayWindow = SDL_CreateWindow("Space invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	//If display window isn't here, display error
	if (displayWindow == nullptr){
		std::cerr << "Display window not found" << std::endl;
		quit = true;
		return;
	}

	context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//Set up View Port
	glViewport(0, 0, passedScreenWidth, passedScreenHeight);

	//Blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	//Need this to create the shader programs
#ifdef _WINDOWS
	glewInit();
#endif
	
	shaderProgram = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	fontText = loadTexture("Pictures/font.png"); //Loads the font we will be using

	score = 0;

	winSituation = "Lose";

	enemyDirection = 1;

}


SpaceInvaderGame::~SpaceInvaderGame()
{
	delete shaderProgram;
	SDL_DestroyWindow(displayWindow);
}

//Main game loop
void SpaceInvaderGame::gameLoop(){

	SDL_Event event;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set color to black

	//ShaderProgram shaderProgram = ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	//std::string spaceText = "Space Invaders";

	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	//Set the matrices
	shaderProgram->setModelMatrix(modelMatrix);
	shaderProgram->setViewMatrix(viewMatrix);
	shaderProgram->setProjectionMatrix(projectionMatrix);

	//sets up all the entities used in game
	spriteSetUp();

	while (!quit){
		while (SDL_PollEvent(&event)){
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
				quit = true;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);
		
		update();
		//player.sprite.drawSpriteSheet(projectionMatrix, viewMatrix, modelMatrix);
		
		render();

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return;
}

void SpaceInvaderGame::drawText(std::string text, GLfloat size, GLfloat spacing)
{
	GLfloat texture_size = 1.0 / 16.0f;
	std::vector<GLfloat> vertexData;
	std::vector<GLfloat> texCoordData;

	for (GLuint index = 0; index < text.size(); ++index)
	{
		GLfloat texture_x = (GLfloat)(((GLuint)text[index]) % 16) / 16.0f;
		GLfloat texture_y = (GLfloat)(((GLuint)text[index]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * index) + (-0.5f * size), 0.5f * size,
			((size + spacing) * index) + (-0.5f * size), -0.5f * size,
			((size + spacing) * index) + (0.5f * size), 0.5f * size,
			((size + spacing) * index) + (0.5f * size), -0.5f * size,
			((size + spacing) * index) + (0.5f * size), 0.5f * size,
			((size + spacing) * index) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(shaderProgram->programID);

	glVertexAttribPointer(shaderProgram->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(shaderProgram->positionAttribute);
	glVertexAttribPointer(shaderProgram->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(shaderProgram->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, fontText);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);

	glDisableVertexAttribArray(shaderProgram->positionAttribute);
	glDisableVertexAttribArray(shaderProgram->texCoordAttribute);

	//std::cout << text << std::endl;
}

GLuint SpaceInvaderGame::loadTexture(const char *image_path)
{
	SDL_Surface *surface = IMG_Load(image_path);
	if (surface == nullptr){
		printf("Error surface data not loaded");
		return NULL;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	//GL_NEAREST to delete wrap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);

	return textureID;
}

//Renders the game depending on state
void SpaceInvaderGame::render()
{
	//std::cout << state << std::endl;
	switch (state)
	{
	case STATE_MAIN_MENU:
		mainMenuRender();
		break;
	case STATE_GAME_LEVEL:
		gameLevelRender();
		break;
	case STATE_GAME_OVER:
		gameOverRender();
		break;
	}
}
//Updates the game depending on state
void SpaceInvaderGame::update()
{
	switch (state)
	{
	case STATE_MAIN_MENU:
		//std::cout << state << std::endl;

		mainMenuUpdate();
		break;
	case STATE_GAME_LEVEL:
		gameLevelUpdate();
		break;
	case STATE_GAME_OVER:
		gameOverUpdate();
		break;
	}
}

void SpaceInvaderGame::displayText(std::string text, ShaderProgram* shaderProgram, GLfloat x_position, GLfloat y_position){
	static Matrix textMatrix; //No idea why it doesn't work as a local Matrix, will figure out later
	textMatrix.setPosition(x_position, y_position, 0.0f);
	//textMatrix.Scale(1.0f, 0.9f, 0.9f);
	shaderProgram->setModelMatrix(textMatrix);
	drawText(text, 0.5f, -0.28f); //Shader passed in by reference

}

//Sets up all the sprites used in game
void SpaceInvaderGame::spriteSetUp()
{
	spriteTexture = loadTexture("Pictures/characters.png");
	if (!spriteTexture){
		std::cout << "ABORT!! ABORT!! Setup didn't work!!!\n";
	}

	//std::cout << snowballTexture << std::endl;

	//Position of the player
	player.x = 0.0f;
	player.y = -1.8f;
	//Sprite counts for texture sheet
	player.sprite.spriteCountX = 12;
	player.sprite.spriteCountY = 8;

	player.sprite.textureID = spriteTexture;
	player.sprite.programID = shaderProgram->programID;

	//5th object on 4th row
	player.sprite.index = 4 + (12*3);

	//Set up enemies
	GLfloat xStart = -3.2f; //Projection display screen size x
	GLfloat yStart = 0.0f; //Y position starts at center

	for (GLuint x_index = 0; x_index < 10; ++x_index)
	{
		for (GLuint y_index = 0; y_index < 5; ++y_index)
		{
			Entity enemy;
			//Position of enemy
			enemy.x = xStart + (x_index * 0.7111f); // 6.4 divided by 9 (2 times the xStart divided by max index)
			enemy.y = yStart + (y_index * 0.3f);
			//Sprite counts for texture sheet
			enemy.sprite.spriteCountX = 12;
			enemy.sprite.spriteCountY = 8;

			enemy.sprite.textureID = spriteTexture;
			enemy.sprite.programID = shaderProgram->programID;

			enemy.sprite.index = 7;

			enemies.push_back(enemy);
		}
	}

	//Create 50 snowballs for each vector
	for (GLuint index = 0; index < 50; ++index)
	{
		//Player spiders
		Entity spidersForward;

		spidersForward.sprite.spriteCountX = 12;
		spidersForward.sprite.spriteCountY = 8;
		
		spidersForward.sprite.textureID = spriteTexture;
		spidersForward.sprite.programID = shaderProgram->programID;

		spidersForward.sprite.index = 10 + (11*7);
		spidersForward.visible = false;
		spidersForward.scaleModelMatrices(0.5f, 0.5f, 1.0f);

		playerSpiders.push_back(spidersForward);

		//Enemy spiders
		Entity spidersBackward;

		spidersBackward.sprite.spriteCountX = 12;
		spidersBackward.sprite.spriteCountY = 8;

		spidersBackward.sprite.textureID = spriteTexture;
		spidersBackward.sprite.programID = shaderProgram->programID;

		spidersBackward.sprite.index = 10 + (10 * 7);
		spidersBackward.visible = false;
		spidersBackward.scaleModelMatrices(0.5f, 0.5f, 1.0f);

		enemySpiders.push_back(spidersBackward);

	}

	std::cout << playerSpiders[0].height << std::endl;
	std::cout << playerSpiders[0].width << std::endl;
	std::cout << enemies[0].height << std::endl;
	std::cout << enemies[0].width << std::endl;
	std::cout << player.height << std::endl;
	std::cout << player.width << std::endl;

}

//Check if you won
GLboolean SpaceInvaderGame::checkVictory()
{
	GLboolean victory = true;
	for (GLuint index = 0; index < enemies.size(); ++index)
	{
		if (enemies[index].visible == true){
			victory = false;
		}
	}
	return victory;
}

//-----Private Functions-----

//Reset everything but the score
void SpaceInvaderGame::reset()
{
	//Position of the player
	player.x = 0.0f;
	player.y = -1.8f;

	GLfloat xStart = -3.2f; //Projection display screen size x
	GLfloat yStart = 0.0f; //Y position starts at center

	for (GLuint x_index = 0; x_index < 10; ++x_index)
	{
		for (GLuint y_index = 0; y_index < 5; ++y_index)
		{
			//Get the position from the vector
			GLuint index = x_index + (y_index * 10);

			//Position of enemy
			enemies[index].x = xStart + (x_index * 0.7111f); // 6.4 divided by 9 (2 times the xStart divided by max index)
			enemies[index].y = yStart + (y_index * 0.3f);

			enemies[index].visible = true;
			enemies[index].collisionEnabled = true;
		}
	}

	//Reset all the bullets
	for (GLuint index = 0; index < 50; ++index)
	{
		playerSpiders[index].visible = false;
		enemySpiders[index].visible = false;
	}
}

//Coordinates are mostly hardcoded
//Displays the main menu title screen (2 strings)
void SpaceInvaderGame::mainMenuRender()
{
	displayText("Space Invaders", shaderProgram, -1.3f, 0.55f);

	displayText("Press Enter to Begin", shaderProgram, -1.85f, -0.15f);
}
//Renders the actual game level
void SpaceInvaderGame::gameLevelRender()
{
	displayText("Score: " + std::to_string(score), shaderProgram, -3.3f, 1.8f);
}
//GameOver screen
void SpaceInvaderGame::gameOverRender()
{
	displayText("Game Over", shaderProgram, -0.75f, 1.0f);

	displayText("You " + winSituation, shaderProgram, -0.75f, 0.50f);

	displayText("Press ESC To", shaderProgram, -1.15f, 0.0f);

	displayText("Return To Menu", shaderProgram, -1.4f, -0.50f);
}

//Checks if enter key is pressed
void SpaceInvaderGame::mainMenuUpdate()
{
	const Uint8 *key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_RETURN] || key[SDL_SCANCODE_KP_ENTER] && state == 0)
	{
		state = 1;
		std::cout << "Entered Game Level" << std::endl;
	}
}
//Updates game loop
void SpaceInvaderGame::gameLevelUpdate()
{
	//Time keeping
	GLfloat ticks = (GLfloat)SDL_GetTicks() / 1000.0f;
	GLfloat elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	//Draw the player
	player.draw(projectionMatrix, viewMatrix, shaderProgram);


	//Draw all the enemies
	for (GLuint index = 0; index < enemies.size(); ++index)
	{
		if (enemies[index].visible == true){
			enemies[index].draw(projectionMatrix, viewMatrix, shaderProgram);
			//Give random chance for enemies to shoot projectiles
			for (GLuint spiderIndex = 0; spiderIndex < enemySpiders.size(); ++spiderIndex)
			{
				//Fire the first non-fired spider
				if (enemySpiders[spiderIndex].visible == false && ((rand()) % 51) == index)
				{
					enemySpiders[spiderIndex].x = enemies[index].x;
					enemySpiders[spiderIndex].y = enemies[index].y;
					enemySpiders[spiderIndex].visible = true;
					break;
				}
			}
		}
	}
	//Keyboard states
	const Uint8 *key = SDL_GetKeyboardState(NULL);

	//Player movement
	if (key[SDL_SCANCODE_D] && player.x <= 3.35)
	{
		player.x += elapsed * 2.0f;
		//Play move right animation
	}
	if (key[SDL_SCANCODE_A] && player.x >= -3.35)
	{
		player.x -= elapsed * 2.0f;
		//Play move left animation
	}

	//If spiders are visible move them
	for (GLuint index = 0; index < playerSpiders.size(); ++index)
	{
		if (playerSpiders[index].visible == true){
			playerSpiders[index].draw(projectionMatrix, viewMatrix, shaderProgram);
			playerSpiders[index].y += elapsed * 2.0f;
			for (GLuint enemyIndex = 0; enemyIndex < enemies.size(); ++enemyIndex)
			{
				//spider collision with enemy adds score if hits
				if (playerSpiders[index].collisionDetection(&(enemies[enemyIndex])))
				{
					score += 10;
					playerSpiders[index].visible = false;
					enemies[enemyIndex].visible = false;
					enemies[enemyIndex].collisionEnabled = false;
				}
				//Check spider collision with border
				if (playerSpiders[index].y >= 2.0)
				{
					playerSpiders[index].visible = false;
				}
			}
		}
	}
	//Enemy spiders
	for (GLuint index = 0; index < enemySpiders.size(); ++index)
	{
		if (enemySpiders[index].visible == true){
			enemySpiders[index].draw(projectionMatrix, viewMatrix, shaderProgram);
			enemySpiders[index].y -= elapsed * 2.0f;
			//Game over if collision with player
			if (enemySpiders[index].collisionDetection(&player))
			{
				std::cout << "GAME OVER YOU GOT HIT\n";
				state = 2;
			}
			//Check spider collision with border
			if (enemySpiders[index].y <= -2.0)
			{
				enemySpiders[index].visible = false;
			}
		}
	}

	//Variable to keep track of time until you can fire again
	static GLfloat fireTracker = 0.0f;
	if (key[SDL_SCANCODE_SPACE] && fireTracker <= 0.0f)
	{
		for (GLuint index = 0; index < playerSpiders.size(); ++index)
		{
			//Fire the first non-fired spider
			if (playerSpiders[index].visible == false)
			{
				playerSpiders[index].x = player.x;
				playerSpiders[index].y = player.y;
				playerSpiders[index].visible = true;
				break;
			}
		}
		fireTracker = 0.5f;
	}
	fireTracker -= elapsed;

	enemyMovement(elapsed);

	//check victory
	if (checkVictory()){
		winSituation = "Win!";
		state = 2;
		//Score not reset
	}
}
//Checks if ESC key is pressed
void SpaceInvaderGame::gameOverUpdate()
{
	const Uint8 *key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_ESCAPE] && state == 2)
	{
		state = 0;
		std::cout << "Entered Main Menu" << std::endl;
		reset();
		score = 0;
	}
}

void SpaceInvaderGame::enemyMovement(GLfloat elapsed)
{
	//enemyDirection = -1 for left border 1 for right border
	GLuint futureDirection = 0;
	if (enemyDirection == 1)
	{
		for (GLuint index = 0; index < enemies.size(); ++index)
		{
			enemies[index].x += elapsed * 0.5f;
			if (enemies[index].collisionEnabled == true && enemies[index].x >= 3.35)
			{
				futureDirection = -1;
			}
		}
	}
	else if (enemyDirection == -1)
	{
		for (GLuint index = 0; index < enemies.size(); ++index)
		{
			enemies[index].x -= elapsed * 0.5f;
			if (enemies[index].collisionEnabled == true && enemies[index].x <= -3.35)
			{
				futureDirection = 1;
			}
		}
	}
	//The prophecy came true!
	if (futureDirection != 0)
	{
		enemyDirection = futureDirection;
		for (GLuint index = 0; index < enemies.size(); ++index)
		{
			if (enemies[index].collisionEnabled == true && enemies[index].y <= -1.95)
			{
				state = 2;
			}
		}
	}
}

//Debug info
/*
	std::cout << playerSpiders[0].height << std::endl;
	std::cout << playerSpiders[0].width << std::endl;
	std::cout << enemies[0].height << std::endl;
	std::cout << enemies[0].width << std::endl;
	std::cout << player.height << std::endl;
	std::cout << player.width << std::endl;

*/
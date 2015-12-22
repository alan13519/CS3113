#include "DungeonGame.h"

enum GameState { STATE_MAIN_MENU, STATE_INN_LEVEL, STATE_DUNGEON_LEVEL, STATE_GAME_OVER};

GLuint state;

#define MAX_BULLETS 200
#define MAX_ENEMIES 50
#define FIRE_RATE 1.0f
#define SPAWN_RATE 5.0f

DungeonGame::DungeonGame(GLuint passedScreenWidth, GLuint passedScreenHeight)
	: screenHeight(passedScreenHeight), screenWidth(passedScreenWidth), quit(false)
{
	//Initalize SDL video
	SDL_Init(SDL_INIT_VIDEO);

	//Create a window where we display the game
	displayWindow = nullptr;
	displayWindow = SDL_CreateWindow("Dungeons", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	//If display window isn't here, display error
	if (displayWindow == nullptr){
		std::cerr << "Display Window not found" << std::endl;
		quit = true;
		return;
	}

	//Create a SDL context
	context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

	//Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Need this to create the shader programs
#ifdef _WINDOWS
	glewInit();
#endif

	//Create a default shader program
	shaderProgram = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	//Set orthographic projection
	projectionMatrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	//Set view and projection matrix
	shaderProgram->setViewMatrix(viewMatrix);
	shaderProgram->setProjectionMatrix(projectionMatrix);

	//Set view port
	glViewport(0, 0, screenWidth, screenHeight);

	//Set up player character
	player = new Player(shaderProgram);
	

	//Set up sound
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	dungeonMusic = nullptr;

	dungeonMusic = Mix_LoadMUS("Music/dungeon.mp3");
	shootSound = Mix_LoadWAV("Music/shoot.wav");
	explodeSound = Mix_LoadWAV("Music/explode.wav");

	if (!dungeonMusic || !shootSound || !explodeSound)
	{
		std::cerr << "Music loaded unsuccessfully\n";
	}

	//Play music
	Mix_PlayMusic(dungeonMusic, -1);


	//Make projectile pool  
	for (size_t index = 0; index < MAX_BULLETS; ++index)
	{
		Projectile* playerProjectile = new Projectile(shaderProgram, 1);
		playerProjectile->visible = false;
		playerProjectiles.push_back(playerProjectile);

		Projectile* enemyProjectile = new Projectile(shaderProgram, 3);
		enemyProjectile->visible = false;
		enemyProjectiles.push_back(enemyProjectile);
	}

	//Start screen Image (Probably should change this...later)
	startScreen = new Room(6.0f, 2.0f, 0.0f, 0.0f);
	startScreen->textureID = loadTexture("Sprites/startGame.png");

	//=========Color Setup=========
	GLfloat red = 0.0f;
	GLfloat green = 0.0f;
	GLfloat blue = 0.0f;

	red /= 255.0f;
	green /= 255.0f;
	blue /= 255.0f;

	glClearColor(red, green, blue, 1.0f);
	//=======Color Setup End=======



}


DungeonGame::~DungeonGame()
{
	//Free music/sound 
	Mix_FreeChunk(shootSound);
	Mix_FreeChunk(explodeSound);
	Mix_FreeMusic(dungeonMusic);

	//Delete the bullets
	for (size_t index = 0; index < MAX_BULLETS; ++index)
	{
		delete playerProjectiles[index];
		delete enemyProjectiles[index];
	}

	//Delete the enemies
	for (size_t index = 0; index < enemies.size(); ++index)
	{
		delete enemies[index];
	}

	//Free the rest
	delete startScreen;
	delete shaderProgram;
	delete player;
	SDL_DestroyWindow(displayWindow);

	
}

void DungeonGame::gameLoop()
{
	//Make map
	Map map;
	map.generateMap(60);

	currentMap = &map;

	//Start out state
	state = STATE_MAIN_MENU;
	//The main game loop
	while (!quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
			{
				quit = true;
			}
		}

		//Display the background color
		glClear(GL_COLOR_BUFFER_BIT);

		//pass in event more mouse
		update();

		render();

		SDL_GL_SwapWindow(displayWindow);

	}
	SDL_Quit();
	return;
}

//Loads images
GLuint DungeonGame::loadTexture(const char *image_path)
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

void DungeonGame::render()
{
	switch (state)
	{
	case STATE_MAIN_MENU:
		mainMenuRender();
		break;
	case STATE_INN_LEVEL:
		innLevelRender();
		break;
	case STATE_DUNGEON_LEVEL:
		dungeonLevelRender();
		break;
	case STATE_GAME_OVER:
		gameOverRender();
		break;
	}
	return;
}

void DungeonGame::update()
{
	switch (state)
	{
	case STATE_MAIN_MENU:
		mainMenuUpdate();
		break;
	case STATE_INN_LEVEL:
		innLevelUpdate();
		break;
	case STATE_DUNGEON_LEVEL:
		dungeonLevelUpdate();
		break;
	case STATE_GAME_OVER:
		gameOverUpdate();
		break;
	}
	return;
}

void  DungeonGame::initialSetup()
{
	return;
}

void DungeonGame::generateLevel()
{
	return;
}

//Update functions
void DungeonGame::mainMenuUpdate()
{
	//If mouse click occurs than switch states
	if (event.type == SDL_MOUSEBUTTONDOWN){
		state = STATE_DUNGEON_LEVEL;
	}
	return;
}

void DungeonGame::innLevelUpdate()
{
	return;
}

void DungeonGame::dungeonLevelUpdate()
{
	/*GLint alphaAmountUniform = glGetUniformLocation(shaderProgram->programID,
		"alphaValue");
	glUniform1f(alphaAmountUniform, -5.0f);*/

	viewMatrix.identity();
	viewMatrix.Translate(-player->x, -player->y, 0.0);
	shaderProgram->setViewMatrix(viewMatrix);

	//Time keeping
	GLfloat ticks = (GLfloat)SDL_GetTicks() / 1000.0f;
	GLfloat elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	//Check for keyboard state
	const Uint8 *key = SDL_GetKeyboardState(NULL);

	//Player movement WASD
	if (key[SDL_SCANCODE_W])
	{
		player->acceleration_y = 1.0f;
	}
	if (key[SDL_SCANCODE_A])
	{
		player->acceleration_x = -1.0f;
	}
	if (key[SDL_SCANCODE_S])
	{
		player->acceleration_y = -1.0f;
	}
	if (key[SDL_SCANCODE_D])
	{
		player->acceleration_x = 1.0f;
	}

	//If the player is not pressing these keys then set the corresponding acceleration to 0
	if (!key[SDL_SCANCODE_W] && !key[SDL_SCANCODE_S])
	{
		player->acceleration_y = 0.0f;
	}
	if (!key[SDL_SCANCODE_A] && !key[SDL_SCANCODE_D])
	{
		player->acceleration_x = 0.0f;
	}

	//Variable to keep track of time until you can fire again
	static GLfloat fireRate = 0.0f;
	if (event.type == SDL_MOUSEBUTTONDOWN && fireRate <= 0.0f)
	{
		//Mouse click location
		mouseX = (((GLfloat)(event.button.x) / screenWidth) * 3.55f * 2.0f) - 3.55f + player->x;
		mouseY = (((GLfloat)(screenHeight - event.button.y + player->y) / screenHeight) * 2.0f * 2.0f) - 2.0f + player->y;

		for (size_t index = 0; index < MAX_BULLETS; ++index)
		{
			if (!playerProjectiles[index]->visible)
			{
				playerProjectiles[index]->x = player->x;
				playerProjectiles[index]->y = player->y;

				//Make projectile visible
				playerProjectiles[index]->visible = true;

				//Fire projectile at a velocity
				playerProjectiles[index]->velocity_x = (mouseX - player->x);
				playerProjectiles[index]->velocity_y = (mouseY - player->y);

				Mix_PlayChannel(-1, shootSound, 0);

				break;
			}
		}
		fireRate = FIRE_RATE / 2.0f;
	}
	fireRate -= elapsed;

	//Update the Player character
	player->updateEntity(elapsed, currentMap);

	if (player->health <= 0)
	{
		Mix_PlayChannel(-1, explodeSound, 0);
		state = STATE_GAME_OVER;
	}

	//Update enemies
	for (size_t index = 0; index < enemies.size(); ++index)
	{
		//Check if enemy exists
		if (!enemies[index]) continue;
		enemies[index]->updateEntity(elapsed, currentMap);
		if (enemies[index]->health <= 0)
		{
			Mix_PlayChannel(-1, explodeSound, 0);

			delete enemies[index];
			enemies[index] = nullptr;

			//Erases the indexth element

			/*Room* room = currentMap->rooms[(rand() % currentMap->rooms.size())];
			enemies[index]->x = room->roomX;
			enemies[index]->y = room->roomY;

			enemies[index]->health = 100;*/
		}
	}

	for (size_t index = 0; index < enemies.size(); ++index)
	{
		//Check if enemy exists
		if (!enemies[index]) continue;
		if (enemies[index]->collisionDetection(player))
		{

			player->health -= 10.0f * elapsed;
		}
	}

	//Update the bullets if they are visible
	for (size_t index = 0; index < MAX_BULLETS; ++index)
	{
		//Check if there is available player bullet
		if (playerProjectiles[index]->visible)
		{
			playerProjectiles[index]->updateProjectile(elapsed, currentMap);
			//Check bullet collision
			for (size_t secondIndex = 0; secondIndex < enemies.size(); ++secondIndex)
			{
				//Check if enemy exists
				if (!enemies[secondIndex]) continue;
				if (enemies[secondIndex]->pointEntityCollision(playerProjectiles[index]->x,
					playerProjectiles[index]->y))
				{
					enemies[secondIndex]->health -= 50;
					playerProjectiles[index]->visible = false;
					break;
				}
			}
		}
		//Check if there is available enemy bullet
		if (enemyProjectiles[index]->visible)
		{
			enemyProjectiles[index]->updateProjectile(elapsed, currentMap);

			//Check player bullet collision
			if (player->pointEntityCollision(enemyProjectiles[index]->x,
				enemyProjectiles[index]->y))
			{
				player->health -= 25;
				enemyProjectiles[index]->visible = false;
			}
		}
	}

	//Enemy spawn rate
	static GLfloat enemySpawnRate = 0.0f;
	if (enemySpawnRate <= 0.0f)
	{
		spawnEnemies();
		enemySpawnRate = SPAWN_RATE;
	}
	enemySpawnRate -= elapsed;

	//Enemy Update
	for (size_t index = 0; index < enemies.size(); ++index)
	{
		//Check if enemy exists
		if (!enemies[index]) continue;
		enemies[index]->moveTowardsEntity(player, elapsed);
		if (!(enemies[index]->x - player->x >= 1.5f)
			&& !(enemies[index]->y - player->y >= 1.5f)
			&& !(player->x - enemies[index]->x >= 1.5f)
			&& !(player->y - enemies[index]->y >= 1.5f)
			&& enemies[index]->fireRate <= 0.0f)
		{
			for (size_t secondIndex = 0; secondIndex < enemyProjectiles.size(); ++secondIndex)
			{
				if (enemyProjectiles[secondIndex]->visible == false && fireRate <= 0.0f)
				{
					enemies[index]->fireAtTarget(player, enemyProjectiles[secondIndex]);

					//Plays sound
					Mix_PlayChannel(-1, shootSound, 0);

					enemies[index]->fireRate = FIRE_RATE;

					break;
				}
			}
		}
		enemies[index]->fireRate -= elapsed;
	}

	//Exit the game by pressing ESC
	if (key[SDL_SCANCODE_ESCAPE] && state == 2)
	{
		state = STATE_GAME_OVER;
		quit = true;
	}


	return;
}

void DungeonGame::gameOverUpdate()
{
	return;
}

//Render functions
void DungeonGame::mainMenuRender()
{
	//Use the shader program
	glUseProgram(shaderProgram->programID);

	//Texture of the room
	GLfloat texCoords[] = { 0.0, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};
	GLfloat vertices[] =
	{ 
		startScreen->getBottomLeft("x"), startScreen->getBottomLeft("y"),
		startScreen->getTopRight("x"), startScreen->getTopRight("y"),
		startScreen->getTopLeft("x"), startScreen->getTopLeft("y"),
		startScreen->getTopRight("x"), startScreen->getTopRight("y"),
		startScreen->getBottomLeft("x"), startScreen->getBottomLeft("y"),
		startScreen->getBottomRight("x"), startScreen->getBottomRight("y")
	};

	shaderProgram->setModelMatrix(startScreen->modelMatrix);

	//Bind triangles
	glVertexAttribPointer(shaderProgram->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(shaderProgram->positionAttribute);

	//Bind Texture
	glVertexAttribPointer(shaderProgram->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(shaderProgram->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, startScreen->textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(shaderProgram->positionAttribute);
	glDisableVertexAttribArray(shaderProgram->texCoordAttribute);

	glDisable(GL_TEXTURE_2D);

	return;
}

void DungeonGame::innLevelRender()
{
	return;
}

void DungeonGame::dungeonLevelRender()
{

	currentMap->drawMap(projectionMatrix, viewMatrix, shaderProgram);

	//Draw all the enemies
	for (size_t index = 0; index < enemies.size(); ++index)
	{
		//Check if enemy exists
		if (!enemies[index]) continue;
		if (enemies[index]){
			enemies[index]->draw(projectionMatrix, viewMatrix, shaderProgram);
		}
	}
	//Draw all the bullets
	for (size_t index = 0; index < MAX_BULLETS; ++index)
	{
		if (playerProjectiles[index]->visible)
		{
			playerProjectiles[index]->draw(projectionMatrix, viewMatrix, shaderProgram);
		}
		if (enemyProjectiles[index]->visible)
		{
			enemyProjectiles[index]->draw(projectionMatrix, viewMatrix, shaderProgram);
		}
	}

	//Draw the player character
	player->draw(projectionMatrix, viewMatrix, shaderProgram);
	
	return;
}

void DungeonGame::gameOverRender()
{
	return;
}

//Spawns enemies
void DungeonGame::spawnEnemies()
{
	//Checks whether we found a room
	GLboolean foundRoom = false;
	//Will store found room into this pointer
	Room* room = nullptr;
	//Loop through until we find a room
	while (!foundRoom){
		for (size_t index = 0; index < currentMap->rooms.size(); ++index)
		{
			room = currentMap->rooms[index];
			//Check if the rooms are a certain distance away from the player
			if (currentMap->rooms[index]->roomX - player->x >= abs(3))
			{
				room = currentMap->rooms[index];
				foundRoom = true;
				break;
			}
			//Check if the rooms are a certain distance away from the player
			else if (currentMap->rooms[index]->roomY - player->y >= abs(3))
			{
				room = currentMap->rooms[index];
				foundRoom = true;
				break;
			}
			for (size_t index = 0; index < enemies.size(); ++index)
			{
				//Check if enemy exists
				if (!enemies[index]) continue;
				//If the current Room spawn point already has an entity on top of it
				if (!enemies[index]->pointEntityCollision(room->roomX, room->roomY))
				{
					foundRoom = false;
					break;
				}
			}
		}
	}

	//Used for breaking out of nested loop
	GLboolean notDone = true;

	//Checks if there is already an enemy there
	GLboolean noEnemyThere = true;

	//Spawn an inital enemy
	//Check if there's an enemy vector space
	for (size_t index = 0; index < enemies.size() + 1; ++index)
	{
		
		//If there was no empty space in the entire vector
		if ((index == enemies.size()) && enemies.size() <= MAX_ENEMIES)
		{
			Enemy* enemy = new Enemy(shaderProgram);
			enemy->x = room->roomX;
			enemy->y = room->roomY;

			enemies.push_back(enemy);

			std::cout << "Pushed backed an enemy\n";

			break;
		}
		//If there's an empty space in vector
		else if (enemies[index] == nullptr)
		{
			Enemy* enemy = new Enemy(shaderProgram);
			enemy->x = room->roomX;
			enemy->y = room->roomY;

			enemies[index] = enemy;
			break;
		}
	}

	return;
	
}
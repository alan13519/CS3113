#include "Entity.h"

#define FRICTION 0.5f
#define FIXED_TIMESTEP 0.0166666f

//Prototypes
GLuint loadTexture(const char *image_path);
GLfloat lerp(GLfloat v0, GLfloat v1, GLfloat t);

Entity::Entity(ShaderProgram* shaderProgram = nullptr)
{
	//Initial Setup
	x = 0.0f;
	y = 0.0f;

	width = 0.1f;
	height = 0.1f;

	velocity_x = 0.0f;
	velocity_y = 0.0f;

	acceleration_x = 0.0f;
	acceleration_y = 0.0f;

	//Blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Room* lastBottomRight = nullptr;
	Room* lastTopRight = nullptr;
	Room* lastTopLeft = nullptr;
	Room* lastBottomLeft = nullptr;

	//Set visible to be true
	visible = true;
}

Entity::~Entity()
{
	//delete currentRoom;
}

void Entity::draw(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram)
{
	modelMatrix.m[3][0] = x;
	modelMatrix.m[3][1] = y;

	shaderProgram->setModelMatrix(modelMatrix);

	sprite.drawSpriteSheet(projectionMatrix, viewMatrix, modelMatrix, width, height);

	return;
}

GLboolean Entity::collisionDetection(Entity *entity)
{
	GLboolean collision = true;

	if (!collisionEnabled || !(entity->collisionEnabled))
	{
		collision = false;
	}
	//R1's left larger then R2's right
	if ((x - width / 2) >= (entity->x + entity->width / 2)){
		collision = false;
	} //R1's right smaller then R2's left
	else if ((x + width / 2) <= (entity->x - entity->width / 2)){
		collision = false;
	} //R1's top lower than R2's bottom
	else if ((y + height / 2) <= (entity->y - entity->height / 2)){
		collision = false;
	} //R1's bottom higher than R2's top
	else if ((y - height / 2) >= (entity->y + entity->height / 2)){
		collision = false;
	}

	return collision;
}

//update the entity
void Entity::updateEntity(GLfloat elapsedTime, Map* map)
{

	velocity_x += acceleration_x * elapsedTime;
	velocity_y += acceleration_y * elapsedTime;
	
	//Friction application
	if (velocity_x > 0.0f)
	{
		velocity_x -= FRICTION * elapsedTime;
	}
	if (velocity_x < 0.0f)
	{
		velocity_x += FRICTION * elapsedTime;
	}
	if (velocity_y > 0.0f)
	{
		velocity_y -= FRICTION * elapsedTime;
	}
	if (velocity_y < 0.0f)
	{
		velocity_y += FRICTION * elapsedTime;
	}

	//Set max velocity
	if (velocity_x > 1.0f)
	{
		velocity_x = 1.0f;
	}
	if (velocity_x < -1.0f)
	{
		velocity_x = -1.0f;
	}
	if (velocity_y > 1.0f)
	{
		velocity_y = 1.0f;
	}
	if (velocity_y < -1.0f)
	{
		velocity_y = -1.0f;
	}

	//Set the current room of each contact flag
	if (findCurrentRoom(map, bottomRightPoint("x"), bottomRightPoint("y")))
	{
		lastBottomRight = findCurrentRoom(map, bottomRightPoint("x"), bottomRightPoint("y"));
	}
	if (findCurrentRoom(map, topRightPoint("x"), topRightPoint("y")))
	{
		lastTopRight = findCurrentRoom(map, topRightPoint("x"), topRightPoint("y"));
	}
	if (findCurrentRoom(map, topLeftPoint("x"), topLeftPoint("y")))
	{
		lastTopLeft = findCurrentRoom(map, topLeftPoint("x"), topLeftPoint("y"));
	}
	if (findCurrentRoom(map, bottomLeftPoint("x"), bottomLeftPoint("y")))
	{
		lastBottomLeft = findCurrentRoom(map, bottomLeftPoint("x"), bottomLeftPoint("y"));
	}

	//==============Move Along the X Axis===============
	x += velocity_x * elapsedTime;
	
	//contact flag collision booleans
	GLboolean collisionTopRight = false;
	GLboolean collisionBottomRight = false;
	GLboolean collisionTopLeft = false;
	GLboolean collisionBottomLeft = false;

	//Check outside collision using the data of currentRoom
	for (size_t index = 0; index < map->rooms.size(); ++index)
	{
		//Check for the contact flags of the entity
		
		//Check if top right contact flag is colliding
		if (contactPointCollision(topRightPoint("x"), topRightPoint("y"), map->rooms[index]))
		{
			collisionTopRight = true;
		}
		//check if top left contact flag is colliding
		if (contactPointCollision(topLeftPoint("x"), topLeftPoint("y"), map->rooms[index]))
		{
			collisionTopLeft = true;
		}
		//Check if bottom right contact flag is colliding
		if (contactPointCollision(bottomRightPoint("x"), bottomRightPoint("y"), map->rooms[index]))
		{
			collisionBottomRight = true;
		}
		//Check if bottom left contact flag is colliding
		if (contactPointCollision(bottomLeftPoint("x"), bottomLeftPoint("y"), map->rooms[index]))
		{
			collisionBottomLeft = true;
		}
		//If all contact flags are colliding then no problem
		if (collisionTopLeft && collisionBottomRight && collisionBottomLeft && collisionTopRight)
		{
			break;
		}
	}


	//move the entity to correct location
	if (!collisionBottomRight)
	{
		x = lastBottomRight->roomX +
			(lastBottomRight->roomWidth / 2.0f)
			- (width / 2.0f);
	}
	else if (!collisionTopRight)
	{
		x = lastTopRight->roomX +
			(lastTopRight->roomWidth / 2.0f)
			- (width / 2.0f);
	}
	else if (!collisionTopLeft)
	{
		x = lastTopLeft->roomX -
			(lastTopLeft->roomWidth / 2.0f)
			+ (width / 2.0f);
	}
	else if (!collisionBottomLeft)
	{
		x = lastBottomLeft->roomX -
			(lastBottomLeft->roomWidth / 2.0f)
			+ (width / 2.0f);
	}


	//==============Move Along the Y Axis===============
	y += velocity_y * elapsedTime;

	collisionTopRight = false;
	collisionBottomRight = false;
	collisionTopLeft = false;
	collisionBottomLeft = false;

	//Check outside collision using the data of currentRoom
	for (size_t index = 0; index < map->rooms.size(); ++index)
	{
		//Check for the contact flags of the entity

		//Check if top right contact flag is colliding
		if (contactPointCollision(topRightPoint("x"), topRightPoint("y"), map->rooms[index]))
		{
			collisionTopRight = true;
		}
		//check if top left contact flag is colliding
		if (contactPointCollision(topLeftPoint("x"), topLeftPoint("y"), map->rooms[index]))
		{
			collisionTopLeft = true;
		}
		//Check if bottom right contact flag is colliding
		if (contactPointCollision(bottomRightPoint("x"), bottomRightPoint("y"), map->rooms[index]))
		{
			collisionBottomRight = true;
		}
		//Check if bottom left contact flag is colliding
		if (contactPointCollision(bottomLeftPoint("x"), bottomLeftPoint("y"), map->rooms[index]))
		{
			collisionBottomLeft = true;
		}
		//If all contact flags are colliding then no problem
		if (collisionTopLeft && collisionBottomRight && collisionBottomLeft && collisionTopRight)
		{
			break;
		}
	}

	if (!collisionTopLeft)
	{
		y = lastTopLeft->roomY + (lastTopLeft->roomHeight / 2.0f) - (height / 2.0f);
	}
	else if (!collisionTopRight)
	{
		y = lastTopRight->roomY + (lastTopRight->roomHeight / 2.0f) - (height / 2.0f);
	}
	else if (!collisionBottomRight)
	{
		y = lastBottomRight->roomY - (lastBottomRight->roomHeight / 2.0f) + (height / 2.0f);
	}
	else if (!collisionBottomLeft)
	{
		y = lastBottomLeft->roomY - (lastBottomLeft->roomHeight / 2.0f) + (height / 2.0f);
	}
	

	return;
}

//Finds the room the entity is currently in
Room* Entity::findCurrentRoom(Map* map, GLfloat pointX, GLfloat pointY)
{
	
	//Collision detection for entity and the rooms
	for (size_t index = 0; index < map->rooms.size(); ++index)
	{
		if (contactPointCollision(pointX, pointY, map->rooms[index]))
		{
			return map->rooms[index];
		}
	}
	return nullptr;
}

//Checks whether a contact flag is colliding with a room
GLboolean Entity::contactPointCollision(GLfloat x, GLfloat y, Room* room)
{
	GLboolean collision = true;

	//Check for current room the entity is in
	if ((room->roomX - room->roomWidth / 2.0f) > x)
	{
		collision = false;
	}
	else if ((room->roomX + room->roomWidth / 2.0f) < x)
	{
		collision = false;
	}
	else if ((room->roomY + room->roomHeight / 2.0f) < y)
	{
		collision = false;
	}
	else if ((room->roomY - room->roomHeight / 2.0f) > y)
	{
		collision = false;
	}
	return collision;
}

//Point entity collision
GLboolean Entity::pointEntityCollision(GLfloat xPoint, GLfloat yPoint)
{
	GLboolean collision = true;

	if ((x - width / 2.0f) > xPoint)
	{
		collision = false;
	}
	else if ((x + width / 2.0f) < xPoint)
	{
		collision = false;
	}
	else if ((y + height / 2.0f) < yPoint)
	{
		collision = false;
	}
	else if ((y - height / 2.0f) > yPoint)
	{
		collision = false;
	}
	return collision;
}

//============Contact Flags=================
GLfloat Entity::topRightPoint(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = x + (width / 2.0f);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = y + (height / 2.0f);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}

GLfloat Entity::topLeftPoint(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = x - (width / 2.0f);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = y + (height / 2.0f);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}

GLfloat Entity::bottomRightPoint(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = x + (width / 2.0f);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = y - (height / 2.0f);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}

GLfloat Entity::bottomLeftPoint(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = x - (width / 2);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = y - (height / 2);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}
//============Contact Flags end===============


Character::Character(ShaderProgram* shaderProgram = nullptr)
	: Entity(shaderProgram)
{
	//Everyone starts with 100 health
	health = 100;
}

Character::~Character()
{

}

void Character::attack()
{
	return;
}

void Character::defend()
{
	return;
}


Player::Player(ShaderProgram* shaderProgram = nullptr)
	: Character(shaderProgram)
{
	//Initalize the sprite sheet data
	sprite.textureID = loadTexture("Sprites/Objects.png");
	sprite.programID = shaderProgram->programID;

	sprite.spriteCountX = 5;
	sprite.spriteCountY = 1;

	sprite.index = 0;

}

Player::~Player()
{

}

void Player::faceToMouse()
{
	return;
}

Enemy::Enemy(ShaderProgram* shaderProgram = nullptr)
	: Character(shaderProgram)
{
	sprite.textureID = loadTexture("Sprites/Objects.png");
	sprite.programID = shaderProgram->programID;

	sprite.spriteCountX = 5;
	sprite.spriteCountY = 1;

	sprite.index = 4;

	fireRate = 0.0f;
}

Enemy::~Enemy()
{
}

//Move towards enemy
void Enemy::moveTowardsEntity(Entity* entity, GLfloat elapsedTime)
{
	//Acceleration movements
	if (entity->x > x)
	{
		if (acceleration_x < 0.0f) acceleration_x = 0.0f;
		acceleration_x += 0.25f * elapsedTime;
	}
	if	(x > entity->x)
	{
		if (acceleration_x > 0.0f) acceleration_x = 0.0f;
		acceleration_x -= 0.25f  * elapsedTime;
	}
	if (entity->y > y)
	{
		if (acceleration_y < 0.0f) acceleration_y = 0.0f;
		acceleration_y += 0.25f * elapsedTime;
	}
	if (y > entity->y)
	{
		if (acceleration_y > 0.0f) acceleration_y = 0.0f;
		acceleration_y -= 0.25f  * elapsedTime;
	}

	
	
	return;
}

void Enemy::fireAtTarget(Entity* entity, Projectile* projectile)
{
	projectile->x = x;
	projectile->y = y;

	//Makes projectile visible
	projectile->visible = true;

	//Fire projectile at a velocity
	projectile->velocity_x = -x + entity->x;
	projectile->velocity_y = -y + entity->y;

}


//Linear interpolation (Not used)
GLfloat lerp(GLfloat v0, GLfloat v1, GLfloat t)
{
	return (1.0f - t) * v0 + (t * v1);
}

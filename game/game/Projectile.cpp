#include "Projectile.h"

//Prototypes
GLuint loadTexture(const char *image_path);

Projectile::Projectile(ShaderProgram* shaderProgram = nullptr, GLuint index = -1)
{
	//Initial Setup
	x = 0.0f;
	y = 0.0f;

	width = 0.025f;
	height = 0.025f;

	velocity_x = 0.0f;
	velocity_y = 0.0f;

	sprite.textureID = loadTexture("Sprites/Objects.png");
	sprite.programID = shaderProgram->programID;

	sprite.spriteCountX = 5;
	sprite.spriteCountY = 1;

	sprite.index = index;

	//Set visible to be true
	visible = false;
}


Projectile::~Projectile()
{
}

void Projectile::draw(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram)
{
	modelMatrix.m[3][0] = x;
	modelMatrix.m[3][1] = y;

	shaderProgram->setModelMatrix(modelMatrix);

	sprite.drawSpriteSheet(projectionMatrix, viewMatrix, modelMatrix, width, height);

	return;
}


GLboolean Projectile::collisionDetection(Projectile *projectile)
{
	GLboolean collision = true;

	//R1's left larger then R2's right
	if ((x - width / 2) >= (projectile->x + projectile->width / 2)){
		collision = false;
	} //R1's right smaller then R2's left
	else if ((x + width / 2) <= (projectile->x - projectile->width / 2)){
		collision = false;
	} //R1's top lower than R2's bottom
	else if ((y + height / 2) <= (projectile->y - projectile->height / 2)){
		collision = false;
	} //R1's bottom higher than R2's top
	else if ((y - height / 2) >= (projectile->y + projectile->height / 2)){
		collision = false;
	}

	return collision;
}

//update the projectile
void Projectile::updateProjectile(GLfloat elapsedTime, Map* map)
{
	//==============Move Along the X and Y Axis===============
	x += velocity_x * elapsedTime;
	y += velocity_y * elapsedTime;

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

	//If collided with something, destroy the object
	if (!collisionTopRight && !collisionTopLeft && !collisionBottomRight && !collisionBottomLeft)
	{
		velocity_x *= -1.0f;
		velocity_x /= 2.0f;
		velocity_y *= -1.0f;
		velocity_y /= 2.0f;
		visible = false;
	}

}


//Checks whether a contact flag is colliding with a room
GLboolean Projectile::contactPointCollision(GLfloat x, GLfloat y, Room* room)
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

//============Contact Flags=================
GLfloat Projectile::topRightPoint(std::string xOrY)
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

GLfloat Projectile::topLeftPoint(std::string xOrY)
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

GLfloat Projectile::bottomRightPoint(std::string xOrY)
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

GLfloat Projectile::bottomLeftPoint(std::string xOrY)
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
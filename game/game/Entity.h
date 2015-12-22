#pragma once
#include "SDLandOpenGL.h"
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Spritesheet.h"
#include "Map.h"
#include "Projectile.h"

class Entity
{
public:
	Entity(ShaderProgram* shaderProgram);
	~Entity();

	//---------------Variables----------------
	
	std::string name;

	//Model matrix
	Matrix modelMatrix;

	GLfloat x;
	GLfloat y;

	GLfloat width;
	GLfloat height;

	GLboolean collisionEnabled;

	GLfloat velocity_x;
	GLfloat velocity_y;

	GLfloat acceleration_x;
	GLfloat acceleration_y;

	Spritesheet sprite;

	Room* lastBottomRight;
	Room* lastTopRight;
	Room* lastTopLeft;
	Room* lastBottomLeft;

	GLboolean visible;

	//---------------Functions--------------
	
	void draw(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram);
	GLboolean collisionDetection(Entity *entity);

	//update the entity
	void updateEntity(GLfloat elapsedTime, Map* map);

	//Find current room the object is in
	Room* findCurrentRoom(Map* map, GLfloat pointX, GLfloat pointY);

	//Checks whether a contact flag is colliding with a room
	GLboolean contactPointCollision(GLfloat x, GLfloat y, Room* room);

	//Point entity collision
	GLboolean pointEntityCollision(GLfloat xPoint, GLfloat yPoint);

	//Contact flags
	GLfloat topRightPoint(std::string xOrY);
	GLfloat topLeftPoint(std::string xOrY);
	GLfloat bottomRightPoint(std::string xOrY);
	GLfloat bottomLeftPoint(std::string xOrY);

private:

};

class Character : public Entity
{
public:
	Character(ShaderProgram* shaderProgram);
	~Character();

	//---------------Variables----------------

	GLuint health;
	std::string characterName;
	std::string characterFaction;
	std::string characterJob; //character class 

	//---------------Functions--------------

	void attack();
	void defend();

private:
};

class Player : public Character
{
public:
	Player(ShaderProgram* shaderProgram);
	~Player();

	//---------------Functions--------------
	void faceToMouse();
private:
};

class Enemy : public Character
{
public:
	Enemy(ShaderProgram* shaderProgram);
	~Enemy();

	GLfloat fireRate;

	//---------------Functions--------------
	void moveTowardsEntity(Entity* entity, GLfloat elapsedTime);
	void fireAtTarget(Entity* entity, Projectile* projectile);
private:
};

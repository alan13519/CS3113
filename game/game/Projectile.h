#pragma once

#include "ShaderProgram.h"
#include "Spritesheet.h"
#include "Map.h"

class Projectile
{
public:
	Projectile(ShaderProgram* shaderProgram, GLuint index);
	~Projectile();

	//Model matrix
	Matrix modelMatrix;

	GLfloat x;
	GLfloat y;

	GLfloat width;
	GLfloat height;

	GLfloat velocity_x;
	GLfloat velocity_y;

	Spritesheet sprite;

	GLboolean visible;

	//---------------Functions--------------

	void draw(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram);
	GLboolean collisionDetection(Projectile* projectile);

	//update the projectile
	void updateProjectile(GLfloat elapsedTime, Map* map);

	//Checks whether a contact flag is colliding with a room
	GLboolean contactPointCollision(GLfloat x, GLfloat y, Room* room);

	//Contact flags
	GLfloat topRightPoint(std::string xOrY);
	GLfloat topLeftPoint(std::string xOrY);
	GLfloat bottomRightPoint(std::string xOrY);
	GLfloat bottomLeftPoint(std::string xOrY);

private:
};


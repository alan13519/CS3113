#pragma once
#include "SDLandOpenGLSetup.h"
#include "Spritesheet.h"
#include "Matrix.h"
#include "ShaderProgram.h"

class Entity
{
public:
	Entity();
	~Entity();

	void draw(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram);
	GLboolean collisionDetection(Entity *entity);

	GLfloat x;
	GLfloat y;

	GLfloat width;
	GLfloat height;

	//Not used in this game
	GLfloat acceleration_x;
	GLfloat acceleration_y;

	//Set Model Matrices
	void setMatrices();

	//Scale Model Matrices
	void scaleModelMatrices(GLfloat x, GLfloat y, GLfloat z);

	Matrix modelMatrix;

	Spritesheet sprite;

	//Visiblity
	GLboolean visible;

	GLboolean collisionEnabled;
	

private:
	//static GLfloat entityVertices[12];
	//static GLfloat texCoords[12];

	GLuint loadTexture(const char *image_path); 


	GLfloat velocity_x;
	GLfloat velocity_y;

};


#pragma once
#include "SDLandOpenGL.h"
#include "Matrix.h"

class Spritesheet
{
public:
	Spritesheet();
	~Spritesheet();

	void drawSpriteSheet(const Matrix &projectionMatrix, 
		const Matrix &viewMatrix, const Matrix &modelMatrix,
		GLfloat givenWidth, GLfloat givenHeight);

	GLuint programID;
	GLuint textureID;

	GLuint spriteCountX;
	GLuint spriteCountY;
	GLuint index;
	GLuint size;

	//Animated flag
	GLboolean animated;

	//UV coordinates
	GLfloat u;
	GLfloat v;
private:
	std::vector<GLuint> frames;
	GLfloat elapsedAnimation;
	GLuint frameIndex;

	GLfloat framesPerSecond;
};
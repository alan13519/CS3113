#include "Spritesheet.h"

Spritesheet::Spritesheet()
{
	spriteCountX = 0;
	spriteCountY = 0;
	index = 0;
	size = 1;
	
	animated = false;
	elapsedAnimation = 0.0f;

	//Must set the program ID and texture ID or ELSE...bad stuff happen
	programID = 0;
	textureID = 0;
}

Spritesheet::~Spritesheet()
{

}

//Draws the sprite
void Spritesheet::drawSpriteSheet(const Matrix &projectionMatrix, const Matrix &viewMatrix, const Matrix &modelMatrix)
{
	glUseProgram(programID); //Default programID is 0
	
	Matrix modelviewMatrix;

	modelviewMatrix = viewMatrix * modelMatrix;

	//std::cout << spriteCountX << " " << spriteCountY << std::endl;

	GLfloat u = (GLfloat)(((GLuint)index) % spriteCountX) / (GLfloat)spriteCountX;
	GLfloat v = (GLfloat)(((GLuint)index) / spriteCountX) / (GLfloat)spriteCountY;

	GLfloat spriteWidth = 1.0 / (GLfloat)spriteCountX;
	GLfloat spriteHeight = 1.0 / (GLfloat)spriteCountY;

	GLfloat quadUVs[] = { u, v,
		u, v + (spriteHeight*size),
		u + spriteWidth, v + (spriteHeight*size),
		u, v,
		u + spriteWidth, v + (spriteHeight*size),
		u + spriteWidth, v
	}; 

	GLfloat quad[] = { -spriteWidth, spriteHeight*size,
		-spriteWidth, -spriteHeight*size,
		spriteWidth, -spriteHeight*size,
		-spriteWidth, spriteHeight*size,
		spriteWidth, -spriteHeight*size,
		spriteWidth, spriteHeight*size };

	GLuint positionAttribute = glGetAttribLocation(programID, "position");
	GLuint texCoordAttribute = glGetAttribLocation(programID, "texCoord");

	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, false, 0, quad);
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, false, 0, quadUVs);
	glEnableVertexAttribArray(texCoordAttribute);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);

	glDisable(GL_TEXTURE_2D);
}

//animate the sprite sheet, pass in the indexs of all the frames used in animation and the fps
void Spritesheet::animate(std::vector<GLuint> frames, GLfloat fps)
{
	if (frames.size() > 1){
		animated = true;
		framesPerSecond = fps;
		this->frames = frames;
		frameIndex = 0;
	}
}

//Updates the sprite with animation based on elapsed time
void Spritesheet::update(GLfloat elapsed)
{
	if (animated) //not to be confused with animate
	{
		elapsedAnimation += elapsed;
		if (elapsedAnimation > 1.0f / framesPerSecond)
		{
			frameIndex++;
			elapsedAnimation = 0.0f;
			//If frame index is greater than the total frames, reset
			if (frameIndex > frames.size() - 1)
			{
				frameIndex = 0;
			}
		}
	}
}
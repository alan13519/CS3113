#include "Entity.h"

//GLfloat Entity::entityVertices[12] = { -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };

//GLfloat Entity::texCoords[12] = { 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

Entity::Entity()
{
	//Initial Setup
	x = 0.0f;
	y = 0.0f;

	width = 0.20f;
	height = 0.20f;

	//Not used
	acceleration_x = 0.0f;
	acceleration_y = 0.0f;
	//Initial Setup end
	
	//Blend
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	visible = true;
	collisionEnabled = true;

}


Entity::~Entity()
{
}

void Entity::draw(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram){
	modelMatrix.m[3][0] = x;
	modelMatrix.m[3][1] = y;

	shaderProgram->setModelMatrix(modelMatrix);

	sprite.drawSpriteSheet(projectionMatrix, viewMatrix, modelMatrix);

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

void Entity::scaleModelMatrices(GLfloat x, GLfloat y, GLfloat z){
	//modelMatrix.Scale(2.0f, 2.0f, 1.0f);

	modelMatrix.Scale(x, y, z);

	width *= x;
	height *= y;

	return;
}

GLuint Entity::loadTexture(const char *image_path){
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}
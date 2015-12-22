#pragma once
#include "SDLandOpenGL.h"
#include "ShaderProgram.h"
#include "Matrix.h"

class Room
{
public:
	Room(GLfloat width, GLfloat height, GLfloat x, GLfloat y);
	~Room();

	//room dimensions
	GLfloat roomWidth;
	GLfloat roomHeight;

	//x and y coordinates
	GLfloat roomX;
	GLfloat roomY;

	//Get the distance between 2 rooms
	GLfloat getDistance(Room* room);

	//Rooms this room is connected to
	std::vector<Room*> roomsConnectedTo;

	//Texture ID of the room
	GLuint textureID;

	//Collision detection between one room and another room
	GLboolean collisionDetection(Room* room);

	//Model Matrix for the room
	Matrix modelMatrix;

	//Get positions of the room
	GLfloat getTopRight(std::string xOrY);
	GLfloat getTopLeft(std::string xOrY);
	GLfloat getBottomRight(std::string xOrY);
	GLfloat getBottomLeft(std::string xOrY);


};

//Consists of several rooms
class Map
{
public:
	Map();
	~Map();

	void generateMap(GLuint numRooms);
	
	//Draw map onto screen
	void drawMap(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram);

	//The rooms of the map
	std::vector<Room*> rooms;

	//Check if room is isolated
	GLboolean isIsolatedRoom(Room* room);

	Room* startingRoom;
};

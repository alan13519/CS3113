#include "Map.h"

enum Direction {topRight, topLeft, bottomRight, bottomLeft};

//Seperate the rooms
void roomSeperation(Direction direction, Room* room1, Room* room2);

//Returns texture ID of an image
GLuint loadTexture(const char *image_path);

//Sets the room image here, as default variables
Room::Room(GLfloat width = 1.0f, GLfloat height = 1.0f, GLfloat x = 0.0f, GLfloat y = 0.0f) :
roomWidth(width), roomHeight(height), roomX(x), roomY(y)
{

	//Use 
	textureID = loadTexture("Sprites/white.png");
}

Room::~Room()
{

}

//Get the distance between 2 rooms as a float
GLfloat Room::getDistance(Room* room)
{
	//Conversion from GLuint to int
	GLfloat distanceX = roomX - room->roomX;
	GLfloat distanceY = roomY - room->roomY;

	//Absolute value of distance
	distanceX = abs(distanceX);
	distanceY = abs(distanceY);

	//Get hypotenuse a^2 + b^2 = c^2
	GLfloat hypotenuse = sqrt((distanceX * distanceX) + (distanceY * distanceY));

	//Hypotenuse should be the distance
	return hypotenuse;
}

//Collision detection between one room and another
GLboolean Room::collisionDetection(Room* room)
{
	GLboolean collision = true;

	//R1's left larger then R2's right
	if ((roomX - roomWidth / 2.0f) >= (room->roomX + room->roomWidth / 2.0f)){
		collision = false;
	} //R1's right smaller then R2's left
	else if ((roomX + roomWidth / 2.0f) <= (room->roomX - room->roomWidth / 2.0f)){
		collision = false;
	} //R1's top lower than R2's bottom
	else if ((roomY + roomHeight / 2.0f) <= (room->roomY - room->roomHeight / 2.0f)){
		collision = false;
	} //R1's bottom higher than R2's top
	else if ((roomY - roomHeight / 2.0f) >= (room->roomY + room->roomHeight / 2.0f)){
		collision = false;
	}

	return collision;
}

//Get either the x or y positions of the rooms

GLfloat Room::getTopRight(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = roomX + (roomWidth / 2.0f);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = roomY + (roomHeight / 2.0f);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}

GLfloat Room::getTopLeft(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = roomX - (roomWidth / 2.0f);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = roomY + (roomHeight / 2.0f);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}

GLfloat Room::getBottomRight(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = roomX + (roomWidth / 2.0f);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = roomY - (roomHeight / 2.0f);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}

GLfloat Room::getBottomLeft(std::string xOrY)
{
	GLfloat point = 0.0f;
	if (xOrY == "X" || xOrY == "x")
	{
		point = roomX - (roomWidth / 2);
	}
	else if (xOrY == "Y" || xOrY == "y")
	{
		point = roomY - (roomHeight / 2);
	}
	else
	{
		std::cerr << "Invalid entry\n";
	}
	return point;
}


/*
========================================================================
							MAP SETUP
========================================================================
*/
Map::Map()
{

}

Map::~Map()
{
	for (size_t index = 0; index < rooms.size(); ++index)
	{
		delete rooms[index];
	}
}

void Map::generateMap(GLuint numRooms)
{
	//delete all items in the room vector
	for (size_t index = 0; index < rooms.size(); ++index)
	{
		delete rooms[index];
	}
	rooms.clear();

	/*initialize random seed */
	srand((unsigned int)time(NULL));

	//Initalize some variables to make the rooms
	GLfloat width;
	GLfloat height;
	GLfloat x;
	GLfloat y;

	//Make the rooms with random sizes overlapping over each other
	for (size_t index = 0; index < numRooms; ++index)
	{
		//Set the lengths of the room
		width = (GLfloat)((rand() % 9) * 0.25f + 1.0f) * 1.5f;
		height = (GLfloat)((rand() % 9) * 0.25f + 1.0f) * 1.5f;
		
		//Set the x position and y position
		x = 0; // (GLfloat)(((rand() % 355) - 177) / 100.0f);
		y = 0; // (GLfloat)((rand() % 300 - 100) / 100.0f);

		if (index == numRooms - 1)
		{
			width = 4.5f;
			height = 4.5f;
		}

		//Create a pointer of the room and push it back into the vector
		Room* room = new Room(width, height, x, y);
		rooms.push_back(room);
	}
	//std::cout << rooms.size() << std::endl;


	//Seperate the rooms by checking collision
	for (size_t index = 0; index < rooms.size(); ++index)
	{
		GLboolean colliding = false;
		//run loop while room is colliding with other rooms
		do
		{
			//Get a direction using index as a key
			Direction direction = (Direction)(index % 4);

			//Assume colliding is false before checking
			colliding = false;

			//Check the collision of current room to the rest of the rooms and move it in a direction
			for (size_t secondIndex = 0; secondIndex < rooms.size(); ++secondIndex)
			{
				//Check if it's not the same room
				if (rooms[index] != rooms[secondIndex])
				{
					//If room is colliding continue with the while loop, else it will check other rooms
					if (rooms[index]->collisionDetection(rooms[secondIndex]))
					{
						colliding = true;

						//Seperate the colliding rooms
						roomSeperation(direction, rooms[index], rooms[secondIndex]);

						break;
					}
				}
			}
		} while (colliding);
	}

	//Put some hallways
	for (size_t index = 0; index < rooms.size(); ++index)
	{
		//If the room is not touching any rooms
		if (isIsolatedRoom(rooms[index]))
		{
			//Check if you can place any hallways
		}
	}

	//Displays room information (DEBUGGING INFORMATION)
	//for (size_t index = 0; index < rooms.size(); index++)
	//{
	//	std::cout << "Position: "
	//		<< (GLfloat)rooms[index]->roomX
	//		<< " "
	//		<< (GLfloat)rooms[index]->roomY
	//		<< " Size: "
	//		<< (GLfloat)rooms[index]->roomHeight
	//		<< " "
	//		<< (GLfloat)rooms[index]->roomWidth
	//		<< std::endl;

	//	std::cout << rooms[index]->getTopRight("X")
	//		<< rooms[index]->getBottomRight("x")
	//		<< std::endl;

	//}

	//For loop ends when no rooms are colliding with each other
	return;
}

//Draws the rooms onto screen
void Map::drawMap(const Matrix &projectionMatrix, const Matrix &viewMatrix, ShaderProgram* shaderProgram)
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

	for (size_t index = 0; index < rooms.size(); ++index)
	{
		//If the room doesn't exist, skip it
		if (!rooms[index])
		{
			continue;
		}

		//Vertices of the room
		GLfloat vertices[] = 
		{	rooms[index]->getBottomLeft("x"), rooms[index]->getBottomLeft("y"),
			rooms[index]->getTopRight("x"), rooms[index]->getTopRight("y"),
			rooms[index]->getTopLeft("x"), rooms[index]->getTopLeft("y"),
			rooms[index]->getTopRight("x"), rooms[index]->getTopRight("y"),
			rooms[index]->getBottomLeft("x"), rooms[index]->getBottomLeft("y"),
			rooms[index]->getBottomRight("x"), rooms[index]->getBottomRight("y")
		};
		
		shaderProgram->setModelMatrix(rooms[index]->modelMatrix);

		//Bind triangles
		glVertexAttribPointer(shaderProgram->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(shaderProgram->positionAttribute);

		//Bind Texture
		glVertexAttribPointer(shaderProgram->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(shaderProgram->texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, rooms[index]->textureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(shaderProgram->positionAttribute);
		glDisableVertexAttribArray(shaderProgram->texCoordAttribute);

		glDisable(GL_TEXTURE_2D);
	}
	return;
}

//Run time = O(n^2), very taxing
GLboolean Map::isIsolatedRoom(Room* room)
{
	GLboolean isIsolated = true;
	//Check all the rooms
	for (size_t index = 0; index < rooms.size(); ++index)
	{
		//Skip if there is no room avaiable
		if (!rooms[index]) continue;
		if (rooms[index] != room)
		{
			//check if the rooms touch
			//Top with bottom
			if (room->roomX + (room->roomHeight / 2) ==
				rooms[index]->roomX - (rooms[index]->roomHeight / 2))
			{
				isIsolated = false;
				break;
			}
			//Bottom with top
			else if (room->roomX - (room->roomHeight / 2) ==
				rooms[index]->roomX + (rooms[index]->roomHeight / 2))
			{
				isIsolated = false;
				break;
			}
			//Right with left
			else if (room->roomY + (room->roomWidth / 2) ==
				rooms[index]->roomY - (rooms[index]->roomWidth / 2))
			{
				isIsolated = false;
				break;
			}
			//Left with right
			else if (room->roomY - (room->roomWidth / 2) ==
				rooms[index]->roomY + (rooms[index]->roomWidth / 2))
			{
				isIsolated = false;
				break;
			}
		}
	}
	return isIsolated;
}

/*

	Didn't finish d Algorithm yet

*/
//Takes in a bunch of rooms and returns the shortest path
std::vector<Room*> dijkstraAlgorithm(std::vector<Room*> rooms
	, Room* startingRoom, Room* endingRoom)
{
	std::vector<Room*> shortestPath;
	std::vector<GLboolean> hasBeenVisited;
	std::vector<GLuint> tentativeDistance;
	GLboolean hasEndingRoom = false;

	//Find starting room and ending room
	for (size_t index = 0; index < rooms.size(); ++index)
	{
		//If the room doesn't exist, skip it
		if (!rooms[index])
		{
			continue;
		}
		if (rooms[index] == startingRoom)
		{
			shortestPath.push_back(rooms[index]);
			tentativeDistance.push_back(0);
		}
		if (rooms[index] == endingRoom)
		{
			hasEndingRoom = true;
		}
		if (rooms[index] != startingRoom)
		{
			//Set the each room has not visited
			hasBeenVisited.push_back(false);
			tentativeDistance.push_back(-1);
		}
	}
	

	//check if starting room and ending room are there
	if (shortestPath.size() == 1 && hasEndingRoom)
	{
		//Run the main algorithm


	}
	return shortestPath;
}

//Returns texture ID of an image
GLuint loadTexture(const char *image_path){
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

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	SDL_FreeSurface(surface);

	return textureID;
}

//Seperate the rooms
void roomSeperation(Direction direction, Room* room1, Room* room2)
{

	if (direction == topRight)
	{
		if (rand() % 2 == 0) //Move to right of room
		{
			room1->roomX = room2->roomX + (room1->roomWidth / 2) + (room2->roomWidth / 2);
		}
		else //Move above other room
		{
			room1->roomY = room2->roomY + (room1->roomHeight / 2) + (room2->roomHeight / 2);
		}
	}
	else if (direction == topLeft)
	{
		if (rand() % 2 == 0) //Move to left of room
		{
			room1->roomX = room2->roomX - (room1->roomWidth / 2) - (room2->roomWidth / 2);
		}
		else //Move above other room
		{
			room1->roomY = room2->roomY + (room1->roomHeight / 2) + (room2->roomHeight / 2);
		}
	}
	else if (direction == bottomRight)
	{
		if (rand() % 2 == 0) //Move to right of room
		{
			room1->roomX = room2->roomX + (room1->roomWidth / 2) + (room2->roomWidth / 2);
		}
		else //Move below other room
		{
			room1->roomY = room2->roomY - (room1->roomHeight / 2) - (room2->roomHeight / 2);
		}
	}
	else if (direction == bottomLeft)
	{
		if (rand() % 2 == 0) //Move to left of room
		{
			room1->roomX = room2->roomX - (room1->roomWidth / 2) - (room2->roomWidth / 2);
		}
		else //Move below other room
		{
			room1->roomY = room2->roomY - (room1->roomHeight / 2) - (room2->roomHeight / 2);
		}
	}
	return;
}
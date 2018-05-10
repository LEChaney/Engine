//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2017 Media Design School
//
// Description  : In Progress
// Author       : Hugo Adams
// Mail         : Hugo.ada7271@mediadesign.school.nz
//

#include "LevelLoader.h"
#include "Utils.h"
#include "Entity.h"
#include "Scene.h"
#include "PrimitivePrefabs.h"
#include "ModelUtils.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <glm\gtc\matrix_transform.hpp>


const int MaxMapWidth = 40;
const int MaxMapHeight = 40; // must be even

NDArray<char,MaxMapWidth,MaxMapHeight> ReadFile(std::string _path)
{
	NDArray<char, MaxMapWidth, MaxMapHeight> charLevel;

	for (int i = 0; i < MaxMapHeight; i++)  // Setting all of charLevel to ignore chara 
	{
		for (int j = 0; j < MaxMapWidth; j++)
		{
			charLevel[i][j] = '0';
		}
	}

	std::fstream myfile;
	myfile.open(_path);
	std::string line;

	for (int i = 0; i < MaxMapHeight; i++)
	{
		if (std::getline(myfile, line))
		{
			for (int j = 0; j < line.size(); j++)
			{
				charLevel[i][j] = line[j];
			}
		}
	}

	myfile.close();

	return charLevel;
}

NDArray<char, 20, 20> ItemFile(std::string _path)
{
	NDArray<char, 20, 20> itemPlace;
	for (int i = 0; i < 20; i++)  // Setting all of charLevel to ignore chara 
	{
		for (int j = 0; j < 20; j++)
		{
			itemPlace[i][j] = 'x';
		}
	}

	std::fstream myfile;
	myfile.open(_path);
	std::string line;

	for (int i = 0; i < 20; i++)
	{
		if (std::getline(myfile, line))
		{
			for (int j = 0; j < line.size(); j++)
			{
				itemPlace[i][j] = line[j];
			}
		}
	}

	myfile.close();

	return itemPlace;
}


void TranslateCharLevel(const NDArray<char, MaxMapWidth, MaxMapHeight> charLevel, Scene& scene)
{

	NDArray<char, 20, 20> straightTrack = ItemFile("Assets/Maps/straight road item.txt");
	NDArray<char, 20, 20> curveTrack = ItemFile("Assets/Maps/curve road item.txt");
	//TransformComponent pickupTransform{};
	//pickupTransform.scale.x = 0.5f;
	//pickupTransform.scale.y = 0.5f;
	//pickupTransform.scale.z = 0.5f;

	// make object vector here 
	float fscale = 1.0f;
	
	for (int i = 0; i < MaxMapHeight; i++)
	{
		for (int j = 0; j < MaxMapWidth; j += 1)
		{
			if (charLevel[i][j] == '0')
			{
				continue;
			}
			else // (charLevel[i][j] == '1')
			{
				bool isStraight = false;
				float rotation = 0;
				switch (charLevel[i][j])
				{
				case 'u':
				{
					isStraight = true;
					break;
				}
				case 'l':
				{
					rotation = 90;
					isStraight = true;
					break;
				}
				case 'd':
				{
					rotation = 180;
					isStraight = true;
					break;
				}
				case 'r':
				{
					rotation = 270;
					isStraight = true;
					break;
				}
				case '1':
				{
					
					isStraight = false;
					break;
				}
				case '2':
				{
					rotation = 270;
					isStraight = false;
					break;
				}
				case '3':
				{
					rotation = 180;
					isStraight = false;
					break;
				}
				case '4':
				{
					rotation = 90;
					isStraight = false;
					break;
				}
				default:
				{
					continue;
				}
				}

				
				Entity& en = scene.createEntity(COMPONENT_MODEL | COMPONENT_TRANSFORM);
				NDArray<char, 20, 20> track;
				if (isStraight)
				{	//straightline
					en.model = ModelUtils::loadModel("Assets/Models/Track/straightUp.obj");
					track = straightTrack;
				}
				else
				{
					//curved piece
					en.model = ModelUtils::loadModel("Assets/Models/Track/curve1.obj");
					track = curveTrack;
				}
				
				// make object here and put in an vector
				//set its position and scale
				
				while (rotation >= 360)
				{
					rotation -= 360;
				}
				
				en.transform.position = glm::vec3(j * (fscale * 50.0f), -10 * fscale, (i * (fscale * 50.0f)));

				en.transform.eulerAngles = glm::vec3(270 * 3.14159/180, rotation * 3.14159/180, 0);
				en.transform.scale = glm::vec3(fscale, fscale, fscale);

				for (int i = 0; i < 19; i++)
				{
					for (int j = 0; j < 19; j++)
					{
						if (track[i][j] != 'x' && track[i][j] != ' ')
						{
							if (randomInt(1, 9) > track[i][j] - '0')
							{
								continue;
							}
							Entity& pickup = Prefabs::createSphere(scene);			
							pickup.transform.position.x = (-45.0f +  10.0f * i);
							pickup.transform.position.z = (-45.0f +  10.0f * j * 0.5f);
							
							if(isStraight)
							{
								glm::mat4 rot = glm::rotate({}, (rotation - 90) * 3.14159f / 180.0f, glm::vec3(0, 1, 0));
								pickup.transform.position = rot * glm::vec4(pickup.transform.position, 1.0f);
							}
							else
							{
								glm::mat4 rot = glm::rotate({}, (rotation - 180) * 3.14159f / 180.0f, glm::vec3(0, 1, 0));
								pickup.transform.position = rot * glm::vec4(pickup.transform.position, 1.0f);
							}

							pickup.transform.position += en.transform.position;
							pickup.transform.position.y = 0;
							pickup.addComponents(COMPONENT_PICKUP);
						}
					}
				}
				//Entity& pickup = Prefabs::createSphere(scene, pickupTransform);
				//pickup.transform.position = glm::vec3(-25 + j * 2, 0.5f, i * 2);
				//pickup.addComponents(COMPONENT_PICKUP);
			}
		}
	}
}

void CreateLevel(Scene& scene, std::string path)
{
	TranslateCharLevel(ReadFile(path), scene);
}


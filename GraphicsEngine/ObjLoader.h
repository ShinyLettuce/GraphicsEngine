#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace ObjLoader
{
	struct ObjVector3
	{
		union
		{
			struct
			{
				float x, y, z;
			};
			float elements[3];
		};
	};

	typedef unsigned int ObjIndex;

	struct Obj
	{
		std::vector<ObjVector3> vertices;
		std::vector<ObjVector3> normals;
		std::vector<ObjIndex> indices;
	};

	inline Obj Load(const char* aFilePath)
	{
		enum LoadState
		{
			None,
			LoadVertex,
			LoadNormal,
			LoadFace
		} loadState{ LoadState::None };

		Obj obj;

		std::ifstream file{ aFilePath };
		std::string line;
		std::string element;

		unsigned int elementIndex = 0;

		while (std::getline(file, line))
		{
			std::stringstream stream{ line };
			while (std::getline(stream, element, ' '))
			{
				switch (loadState)
				{
					case LoadState::None:
					{
						if (element == "v")
						{
							loadState = LoadState::LoadVertex;
							obj.vertices.push_back(ObjVector3{});
						}
						else if (element == "vn")
						{
							loadState = LoadState::LoadNormal;
							obj.normals.push_back(ObjVector3{});
						}
						else if (element == "f")
						{
							loadState = LoadState::LoadFace;
						}
						break;
					}
					case LoadState::LoadVertex:
					{		
						if (elementIndex >= 1 && elementIndex <= 3)
						{
							const float f = std::stof(element);
							obj.vertices.back().elements[elementIndex - 1] = f;
						}
						break;
					}
					case LoadState::LoadNormal:
					{			
						if (elementIndex >= 1 && elementIndex <= 3)
						{
							const float f = std::stof(element);
							obj.normals.back().elements[elementIndex - 1] = f;
						}
						break;
					}
					case LoadState::LoadFace:
					{
						std::string face;
						std::stringstream stream{ element };
						std::getline(stream, face, '/');

						ObjIndex index = std::stoi(face);
						obj.indices.emplace_back(index);
						break;
					}
				}
				++elementIndex;
			}
			elementIndex = 0;
			loadState = LoadState::None;
		}

		return obj;
	}
}
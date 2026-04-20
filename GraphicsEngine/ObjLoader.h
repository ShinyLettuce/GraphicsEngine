#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>

namespace Obj
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

	struct ObjFace
	{
		unsigned int vertex;
		unsigned int normal;
	};

	struct Obj
	{
		std::vector<ObjVector3> vertices;
		std::vector<ObjVector3> normals;
		std::vector<ObjFace> faces;
	};

	inline Obj LoadFromFile(const char* aFilePath)
	{
		Obj obj;

		std::ifstream file{ aFilePath };

		if (!file.is_open())
		{
			std::cout << "Failed to open .obj file \"" << aFilePath << "\"\n";

			return obj;
		}

		std::string line;

		while (std::getline(file, line))
		{
			std::stringstream stream{ line };
			std::string prefix;
			stream >> prefix;

			if (prefix == "v")
			{
				ObjVector3 vertex;
				stream >> vertex.x >> vertex.y >> vertex.z;
				obj.vertices.emplace_back(vertex);
			}
			else if (prefix == "vn")
			{
				ObjVector3 normal;
				stream >> normal.x >> normal.y >> normal.z;
				obj.normals.emplace_back(normal);
			}
			else if (prefix == "f")
			{
				ObjFace face;

				while (stream >> face.vertex)
				{
					stream.ignore(1); // Skip /
					// We dont have texture index
					stream.ignore(1); // Skip /
					stream >> face.normal;

					face.vertex -= 1;
					face.normal -= 1;

					obj.faces.push_back(face);
				}
			}
		}

		return obj;
	}
}
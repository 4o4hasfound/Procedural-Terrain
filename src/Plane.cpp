#include "Plane.h"

std::vector<float> genNonSymPlaneUniform(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int div)
{
	std::vector<float> plane;
	glm::vec3 dir03 = (v3 - v0) / float(div);
	glm::vec3 dir12 = (v2 - v1) / float(div);

	// dir2 and dir3
	for (int i = 0; i < div + 1; i++)
	{
		// dir1
		for (int j = 0; j < div + 1; j++)
		{
			glm::vec3 acrossj = ((v1 + (float)i * dir12) - (v0 + (float)i * dir03)) / float(div);
			glm::vec3 crntVec = v0 + (float)i * dir03 + (float)j * acrossj;
			// Position
			plane.push_back(crntVec.x);
			plane.push_back(crntVec.y);
			plane.push_back(crntVec.z);
			// Tex UV
			plane.push_back(float(j) / div);
			plane.push_back(float(i) / div);
		}
	}

	return plane;
}

// Plane indices for triangle patches
std::vector<unsigned int> genPlaneInd(int div)
{
	std::vector<unsigned int> indices;

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;
			// Top triangle								   ____
			indices.push_back(index);					// |  /
			indices.push_back(index + (div + 1) + 1);	// | /
			indices.push_back(index + (div + 1));		// |/
			// Bot triangle
			indices.push_back(index);					//   /|
			indices.push_back(index + 1);				//	/ |
			indices.push_back(index + (div + 1) + 1);	// /__|
		}
	}

	return indices;
}

// Plane indices for quad patches
std::vector<unsigned int> genPlaneIndTes(int div)
{
	std::vector<unsigned int> indices;

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;			// 3___2
			indices.push_back(index);					//     |
			indices.push_back(index + 1);				//     |
			indices.push_back(index + (div + 1) + 1);	//  ___|
			indices.push_back(index + (div + 1));		// 0   1
		}
	}

	return indices;
}

#pragma once
#ifndef _PLANE_H_
#define _PLANE_H_

#include <vector>
#include <iostream>
#include <GLM/glm.hpp>

std::vector<float> genNonSymPlaneUniform(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, int div);

// Plane indices for triangle patches
std::vector<unsigned int> genPlaneInd(int div);

// Plane indices for quad patches
std::vector<unsigned int> genPlaneIndTes(int div);


#endif
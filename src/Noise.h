#pragma once
#ifndef _NOISE_H_
#define _NOISE_H_

#include <math.h>
#include <limits>

#include <GLM/glm.hpp>
#include <iostream>

#define numOctaves 8

#define persistence 0.5

unsigned int hash(unsigned int x);

unsigned int hash(glm::vec2 v);

float floatConstruct(unsigned int m);

float Noise(float x, float y);

float SmoothedNoise(int x, int y); 

float Interpolate(float a, float b, float x);

float InterpolatedNoise(float x, float y);

float noise(float x, float y, int octaves = numOctaves);

#endif
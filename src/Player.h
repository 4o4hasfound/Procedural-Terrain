#pragma once
#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <iostream>
#include <limits>

#include "Camera.h"
#include "Terrain.h"
#include "Chunk.h"
#include "Noise.h"

class Player {
public:
	Player();
	Player(Camera* camera, Terrain* terrain, float height, float gravity);

	void step(float dt);

	void jump(float strength = 1.0f);
private:
	float m_height;
	float m_gravity;
	float m_lastHeight = 0.0f;
	bool m_onGround = true;
	float m_velocity=0.0f;
	Camera* m_camera;
	Terrain* m_terrain;
};

#endif
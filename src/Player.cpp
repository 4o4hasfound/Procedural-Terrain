#include "Player.h"

Player::Player() {

}

Player::Player(Camera* camera, Terrain* terrain, float height, float gravity) : m_camera(camera), m_terrain(terrain), m_height(height), m_gravity(gravity) {

}

void Player::step(float dt) {
	m_velocity += m_gravity * dt;
	if (m_velocity < -0.7) m_velocity = -0.7;
	m_camera->Position.y += m_velocity;
	float h = noise(glm::abs(m_camera->Position.x), glm::abs(m_camera->Position.z)) * m_terrain->heightScale;
	if (m_camera->Position.y <= h + m_height) {
		m_camera->Position.y = h + m_height;
		m_velocity = 0.0f;
		m_onGround = true;
	}
	else m_onGround = false;
}

void Player::jump(float strength) {
	if (m_onGround) m_velocity = strength;
}
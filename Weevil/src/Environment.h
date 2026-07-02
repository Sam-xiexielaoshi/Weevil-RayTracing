#pragma once

#include <glm/glm.hpp>
#include <cmath>
#include <glm/gtc/constants.hpp>
#include <string>
#include <vector>

class Environment
{
public:
	bool LoadHDR(const std::string& filepath);
	glm::vec3 Sample(const glm::vec3& dirction) const;
	void SetExposure(float exposure) { m_Exposure = exposure; }
	void SetRotation(float rotation) { m_Rotation = rotation; }

private:
	uint32_t m_Width = 0, m_Height = 0;

	float m_Exposure = 1.0f;
	float m_Rotation = 0.0f;

	std::vector<glm::vec3> m_Pixels;
};
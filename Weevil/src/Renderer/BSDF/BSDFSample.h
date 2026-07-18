#pragma once
struct BSDFSample
{
    glm::vec3 Direction;
    glm::vec3 Weight;

    glm::vec3 HalfVector = glm::vec3(0.0f);

    float PDF = 0.0f;

    bool IsDelta = false;
};
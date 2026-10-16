#pragma once
#include "glm/glm.hpp"

struct KeyFrame
{
    float time;
    glm::vec3 position;
    glm::vec3 target;
};

class KeyFrameAnimation
{
public:
    static void SetKeyFrames(const std::vector<KeyFrame>& keyFrames) { s_KeyFrames = keyFrames; }
    static glm::mat4 getAnimatedViewMatrix(float currentTime);

private:
    static glm::vec3 catmullRom(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t);

private:
    static std::vector<KeyFrame> s_KeyFrames;
};

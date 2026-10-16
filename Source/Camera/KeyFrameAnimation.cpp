#include "KeyFrameAnimation.h"

#include "glm/ext/matrix_transform.hpp"

glm::mat4 KeyFrameAnimation::getAnimatedViewMatrix(float currentTime)
{
    if (s_KeyFrames.size() < 2) return glm::mat4(1.0f);

    if (currentTime <= s_KeyFrames.front().time)
        return glm::lookAt(s_KeyFrames.front().position, s_KeyFrames.front().target, glm::vec3(0.0f, 1.0f, 0.0f));
    if (currentTime >= s_KeyFrames.back().time)
        return glm::lookAt(s_KeyFrames.back().position, s_KeyFrames.back().target, glm::vec3(0.0f, 1.0f, 0.0f));

    uint32_t i = 0;
    while (i < s_KeyFrames.size() - 2 && s_KeyFrames[i + 1].time < currentTime)
    {
        i++;
    }

    const auto& k1 = s_KeyFrames[i];
    const auto& k2 = s_KeyFrames[i + 1];

    const auto& k0 = (i > 0) ? s_KeyFrames[i - 1] : k1;
    const auto& k3 = (i < s_KeyFrames.size() - 2) ? s_KeyFrames[i + 2] : k2;

    float segmentProgress = (currentTime - k1.time) / (k2.time - k1.time);

    glm::vec3 camPos = catmullRom(k0.position, k1.position, k2.position, k3.position, segmentProgress);
    glm::vec3 camTarget = glm::mix(k1.target, k2.target, segmentProgress);
    return glm::lookAt(camPos, camTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::vec3 KeyFrameAnimation::catmullRom(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
                                        const glm::vec3& p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    return 0.5f * ((2.0f * p1) +
        (-p0 + p2) * t +
        (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
        (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

std::vector<KeyFrame> KeyFrameAnimation::s_KeyFrames;

#pragma once
#include <GLm/glm.hpp>
class Frustum
{
public:
    void update();
    int PointInFrustum(const glm::vec3& point);
    int SphereInFrustum(const glm::vec3& point, float radius);
    int CubeInFrustum(const glm::vec3& center, float x, float y, float z);
    float getDistance(const glm::vec4& plane, const glm::vec3& point);
    enum {
        FRUSTUM_OUTSIDE = 0,
        FRUSTUM_INTERSECT,
        FRUSTUM_INSIDE,
    };
    glm::mat4 clip;
    glm::vec4 planes[6];
};
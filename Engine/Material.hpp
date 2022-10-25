#pragma once
#include <GLM/vec3.hpp>
#include <GLM/vec4.hpp>
#include <GLEW/glew.h>
#include <string>

struct Material {
    std::string name;

    struct Texture {
        int32_t index{ -1 };
        uint32_t texCoord{ 0 };
    };

    struct PBR {
        glm::vec4 baseColorFactor{ 1.f };
        Texture baseColorTexture;

        float metallicFactor{ 1.0f };
        float roughnessFactor{ 1.0f };
        Texture metallicRoughnessTexture;
    } pbr;

    struct NormalTexture : Texture {
        float scale{ 1.0f };
    } normalTexture;

    struct OcclusionTexture : Texture {
        float strength{ 1.0f };
    } occlusionTexture;

    Texture emissiveTexture;

    glm::vec3 emissiveFactor{ 0.f };

    enum class AlphaMode : uint8_t {
        Opaque,
        Mask,
        Blend
    } alphaMode{ AlphaMode::Opaque };

    float alphaCutoff{ 0.5f };
    bool doubleSided{ false };
};

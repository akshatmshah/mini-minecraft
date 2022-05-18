#ifndef NOISE_H
#define NOISE_H
#include "glm_includes.h"

class Noise
{
public:
    Noise();
    static float perlinNoise2D(glm::vec2 uv);
    static float surflet2D(glm::vec2 P, glm::vec2 gridPoint);
    static float perlinNoise3D(glm::vec3 p);
    static float surflet3D(glm::vec3 P, glm::vec3 gridPoint);
    static float random1(glm::vec2 p);
    static glm::vec2 random2(glm::vec2 p);
    static glm::vec3 random3(glm::vec3 p);
    static float fract(float num);
    static float fractalBrownianMotion(glm::vec2 uv);
    static float interpNoise2D(float x, float z);
    static float WorleyNoise(glm::vec2 uv);
    static float WorleyNoise(glm::vec2 uv,  glm::vec2 *out_cellCenter);
//    static float smoothstep(float edge0, float edge1, float x);
    static float step(float e, float x);
};

#endif // NOISE_H

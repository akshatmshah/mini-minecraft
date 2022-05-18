#include "noise.h"
#include <algorithm>

Noise::Noise()
{}

float Noise::perlinNoise2D(glm::vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; dx++) {
        for(int dy = 0; dy <= 1; dy++) {
            surfletSum += surflet2D(uv, glm::floor(uv) + glm::vec2(dx, dy));
        }
    }
    return surfletSum;
}

float Noise::surflet2D(glm::vec2 P, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    glm::vec2 gradient = 2.f * random2(gridPoint) - glm::vec2(1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->fP with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float Noise::perlinNoise3D(glm::vec3 p) {
    float surfletSum = 0.f;
    // Iterate over the eight integer corners surrounding uv
    for(int dx = 0; dx <= 1; dx++) {
        for(int dy = 0; dy <= 1; dy++) {
            for(int dz = 0; dz <= 1; dz++) {
                surfletSum += surflet3D(p, glm::floor(p) + glm::vec3(dx, dy, dz));
            }
        }
    }
    return surfletSum;
}

float Noise::surflet3D(glm::vec3 p, glm::vec3 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec3 t2 = glm::vec3(abs(p[0] - gridPoint[0]), abs(p[1] - gridPoint[1]), abs(p[2] - gridPoint[2]));
    glm::vec3 t = glm::vec3(1.f) - 6.f * glm::vec3(pow(t2[0], 5.f), pow(t2[1], 5.f), pow(t2[2], 5.f)) + 15.f * glm::vec3(pow(t2[0], 4.f), pow(t2[1], 4.f), pow(t2[2], 4.f)) - 10.f * glm::vec3(pow(t2[0], 3.f), pow(t2[1], 3.f), pow(t2[2], 3.f));
    // Get the random vector for the grid point (assume we wrote a function random2
    // that returns a vec2 in the range [0, 1])
    glm::vec3 gradient = random3(gridPoint) * 2.f - glm::vec3(1., 1., 1.);
    // Get the vector from the grid point to P
    glm::vec3 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y * t.z;
}


float Noise::random1(glm::vec2 p) {
    float t = glm::dot(p, glm::vec2(127.1, 311.7));
    return fract(glm::sin(t * 43758.5453));
}

glm::vec2 Noise::random2(glm::vec2 p) {
    glm::vec2 vec = glm::vec2(glm::dot(p, glm::vec2(127.1, 311.7)),
                              glm::dot(p, glm::vec2(269.5,183.3)));
    return glm::vec2(fract(glm::sin(vec[0] * 43758.5453)), fract(glm::sin(vec[1] * 43758.5453)));
}

glm::vec3 Noise::random3(glm::vec3 p) {
    glm::vec3 vec = glm::vec3(glm::dot(p, glm::vec3(127.1, 311.7, 232.1)),
                         glm::dot(p, glm::vec3(269.5, 183.3, 548.2)),
                         glm::dot(p, glm::vec3(420.6, 631.2, 176.5)));
    return glm::vec3(fract(glm::sin(vec[0] * 43758.5453)), fract(glm::sin(vec[1] * 43758.5453)), fract(glm::sin(vec[2] * 43758.5453)));
}


float Noise::fract(float num) {
    return num - floor(num);
}

float Noise::fractalBrownianMotion(glm::vec2 uv) {
    float total = 0;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 2.f;
    float amp = 0.5f;
    for(int i = 1; i <= octaves; i++) {
        total += interpNoise2D(uv[0] * freq,
                               uv[1] * freq) * amp;

        freq *= 2.f;
        amp *= persistence;
    }
    return total;
}

float Noise::interpNoise2D(float x, float z) {
    int intX = int(floor(x));
    float fractX = fract(x);
    int intZ = int(floor(z));
    float fractZ = fract(z);

    float v1 = (perlinNoise2D(glm::vec2(intX, intZ)) + 1.f) / 2.f;
    float v2 = (perlinNoise2D(glm::vec2(intX + 1, intZ)) + 1.f) / 2.f;
    float v3 = (perlinNoise2D(glm::vec2(intX, intZ + 1)) + 1.f) / 2.f;
    float v4 = (perlinNoise2D(glm::vec2(intX + 1, intZ + 1)) + 1.f) / 2.f;

    float i1 = glm::mix(v1, v2, fractX);
    float i2 = glm::mix(v3, v4, fractX);
    return glm::mix(i1, i2, fractZ);
}

float Noise::WorleyNoise(glm::vec2 uv) {
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::vec2(fract(uv[0]), fract(uv[1]));
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            minDist = std::min(minDist, dist);
        }
    }
    return minDist;
}

float Noise::WorleyNoise(glm::vec2 uv, glm::vec2 *out_cellCenter) {
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvFract = glm::vec2(fract(uv[0]), fract(uv[1]));
    float minDist = 1.0; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell'
            glm::vec2 diff = neighbor + point - uvFract; // Distance between fragment coord and neighbor’s Voronoi point
            float dist = glm::length(diff);
            if(dist < minDist) {
                minDist = dist;
                *out_cellCenter = point;
            }
        }
    }
    return minDist;
}

//float Noise::smoothstep(float edge0, float edge1, float x) {
//    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
//    return x * x * (3 - 2 * x);
//}

float Noise::step(float e, float x) {
    return x < e ? 0.f : 1.f;
}


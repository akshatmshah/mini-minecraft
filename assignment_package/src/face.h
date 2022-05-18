#pragma once

#include <glm/glm.hpp>
#include <unordered_map>
#include "halfedge.h"

class HalfEdge;

class Face {
public:
    Face(glm::vec4 color);
    Face(glm::vec4 color, HalfEdge* he);

    static inline int objCount{ 0 };

    glm::vec4 color; //Color of face
    HalfEdge* he;
    int id;
    std::unordered_map<int, glm::vec2> uvMap;

    void setHalfEdge(HalfEdge* he);
    void setColor(glm::vec4 color);

};


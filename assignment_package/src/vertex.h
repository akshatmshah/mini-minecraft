#pragma once
#include <QListWidgetItem>
#include <glm/glm.hpp>
#include "halfedge.h"
#include "joint.h"

class HalfEdge;

class Vertex
{
public:
    Vertex(glm::vec3 pos);
    Vertex(glm::vec3 pos, HalfEdge* he);

    static inline int objCount{ 0 };

    glm::vec4 pos; //Position of vertex
    HalfEdge* he;
    int id;
    glm::ivec2 IDs;
    glm::vec2 weights;

    void setHalfEdge(HalfEdge* he);
    void setPos(glm::vec4 pos);
    void addJoints(int id1, float weight1, int id2, float weight2);

};



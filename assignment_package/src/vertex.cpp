#include "vertex.h"


Vertex::Vertex(glm::vec3 pos):
    pos(glm::vec4(pos, 1)), he(nullptr), id(objCount++), IDs(glm::ivec2()), weights(glm::vec2())
{}

Vertex::Vertex(glm::vec3 pos, HalfEdge* he):
    pos(glm::vec4(pos, 1)), he(he), id(objCount++), IDs(), weights()
{}

void Vertex::setHalfEdge(HalfEdge* he)
{
    this->he = he;
    he->setVertex(this);
}

void Vertex::setPos(glm::vec4 pos)
{
    this->pos = pos;
}

void Vertex::addJoints(int id1, float weight1, int id2, float weight2)
{
    this->IDs = glm::ivec2(id1, id2);
    this->weights = glm::vec2(weight1, weight2);
}

#include "face.h"

Face::Face(glm::vec4 color):
    color(color), he(nullptr), id(objCount++), uvMap()
{}

Face::Face(glm::vec4 color, HalfEdge* he):
    color(color), he(he), id(objCount++)
{}

void Face::setHalfEdge(HalfEdge* he) {
    this->he = he;
}

void Face::setColor(glm::vec4 color) {
    this->color = color;
}


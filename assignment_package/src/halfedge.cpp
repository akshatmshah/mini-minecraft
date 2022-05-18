#include "halfedge.h"

HalfEdge::HalfEdge():
    next(nullptr), sym(nullptr), face(nullptr), vert(nullptr), id(objCount++)
{}

HalfEdge::HalfEdge(HalfEdge* next, HalfEdge* sym, Face* face, Vertex* vert):
    next(next), sym(sym), face(face), vert(vert), id(objCount++)
{}

void HalfEdge::setNext(HalfEdge* he) {
    this->next = he;
}
void HalfEdge::setSym(HalfEdge* he) {
    this->sym = he;
    he->sym = this;
}
void HalfEdge::setFace(Face* face) {
    this->face = face;
    face->setHalfEdge(this);
}
void HalfEdge::setVertex(Vertex* vert) {
    this->vert = vert;
}

#pragma once

#include <glm/glm.hpp>
#include "face.h"
#include "vertex.h"

class Face;
class Vertex;

class HalfEdge
{
public:
    HalfEdge();
    HalfEdge(HalfEdge* next, HalfEdge* sym, Face* face, Vertex* vert);

    static inline int objCount{ 0 };

    HalfEdge* next;
    HalfEdge* sym;
    Face* face;
    Vertex* vert;
    int id;

    void setNext(HalfEdge* he);
    void setSym(HalfEdge* he);
    void setFace(Face* face);
    void setVertex(Vertex* vert);

};

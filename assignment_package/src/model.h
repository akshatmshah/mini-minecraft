#pragma once
#include "smartpointerhelp.h"
#include "texture.h"
#include "face.h"
#include "vertex.h"
#include "halfedge.h"

class Model
{
public:
    Model(OpenGLContext* context, const char *modelPath, const char *texturePath);

    Texture m_texture;
    bool isBound;

    std::vector<uPtr<Face>> faces;
    std::vector<uPtr<Vertex>> verts;
    std::vector<uPtr<HalfEdge>> halfedges;

    void createMesh(const char *modelPath);
    void bindTexture();
    void loadTexture();

};




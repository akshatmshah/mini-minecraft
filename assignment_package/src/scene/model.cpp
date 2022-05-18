#include "model.h"
#include <iostream>
#include <glm/gtx/io.hpp>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <filesystem>

Model::Model(OpenGLContext* context, const char *modelPath, const char *texturePath) :
    m_texture(context), faces(), verts(), halfedges(), isBound(false)
{
    m_texture.create(texturePath);
    loadTexture();
    createMesh(modelPath);
}

void Model::bindTexture()
{
    m_texture.bind(0);
}

void Model::loadTexture()
{
    m_texture.load(0);
}

void Model::createMesh(const char *modelPath) {
    std::unordered_map<Vertex*, std::unordered_set<int>> HEto; //maps vertex pointers to the indices of half edges that point towards vertex
    std::unordered_map<Vertex*, std::unordered_set<int>> HEaway; //maps vertex pointers to the indices of half edges that point away from vertex
    std::vector<glm::vec2> uvs;

    //reading file
    std::string path(modelPath);
    std::string line;
    std::ifstream MyReadFile(path);
    if (!MyReadFile) {
        std::cout << "File could not be opened" << std::endl;
    }
    while (getline(MyReadFile, line)) {
        std::stringstream ss(line);
        std::string item;
        std::vector<std::string> elems;
        //splits line by space
        for (int i = 0; std::getline(ss, item, ' '); ++i) {
            //ignore lines that don't contain vertex position or face data
            if (i == 0 && item.compare("v") != 0 && item.compare("vt") != 0 && item.compare("f") != 0) {
                break;
            }
            elems.push_back(item);
        }
        if (elems.size() > 0) {
            if (elems[0].compare("v") == 0) {
                //creates vertex
                uPtr<Vertex> vert  = mkU<Vertex>(glm::vec3(std::stof(elems[1]), std::stof(elems[2]), std::stof(elems[3])));
                this->verts.push_back(std::move(vert));
            } else if (elems[0].compare("vt") == 0) {
                uvs.push_back(glm::vec2(std::stof(elems[1]), std::stof(elems[2])));
            } else if (elems[0].compare("f") == 0) {
                //creates face
//                uPtr<Face> face = mkU<Face>(glm::vec4(((float) rand() / (RAND_MAX)), ((float) rand() / (RAND_MAX)), ((float) rand() / (RAND_MAX)), 1.f));
                uPtr<Face> face = mkU<Face>(glm::vec4(1.f, 0.5f, 0.5f, 1.f));
                HalfEdge *prevHE = nullptr; //previous half edge in face
                HalfEdge *firstHE = nullptr; //first half edge created
                Vertex *firstVert = nullptr; //first vertex in face
                for (int i = 1; i < (int) elems.size(); ++i) {
                    //creates half edge that will point away from the current vertesx
                    uPtr<HalfEdge> he = mkU<HalfEdge>();
                    he->setFace(face.get());
                    int firstSlash = elems[i].find('/');
                    Vertex *vert = verts[std::stoi(elems[i].substr(0, firstSlash)) - 1].get();
                    face->uvMap[vert->id] = uvs[std::stoi(elems[i].substr(firstSlash + 1, elems[i].find('/', firstSlash + 1) - firstSlash - 1)) - 1];
                    Vertex *nextVert = (i == (int)elems.size() - 1) ? firstVert : verts[std::stoi(elems[i+1].substr(0, elems[i+1].find('/'))) - 1].get();
                    //checks if map contains vert
                    if (HEaway.find(vert) == HEaway.end()) {
                        std::unordered_set<int> set;
                        HEaway[vert] = set;
                    }
                    //inserts the next index of halfedges, as it will be the index of the new half edge
                    HEaway[vert].insert(this->halfedges.size());
                    //checks if map contains nextVert
                    if (HEto.find(nextVert) == HEto.end()) {
                        std::unordered_set<int> set;
                        HEto[nextVert] = set;
                    }
                    //inserts the next index of halfedges, as it will be the index of the new half edge
                    HEto[nextVert].insert(this->halfedges.size());
                    //loops through all half edges that point towards vert and checks if they point away from the nextVert
                    for (auto& elem: HEto[vert]) {
                        if (HEaway[nextVert].find(elem) != HEaway[nextVert].end()) {
                            he->setSym(this->halfedges[elem].get());
                        }
                    }
                    if (firstHE == nullptr) {
                        firstHE = he.get();
                    }
                    if (firstVert == nullptr) {
                        firstVert = vert;
                    }
                    if (prevHE != nullptr) {
                        prevHE->setNext(he.get());
                        vert->setHalfEdge(prevHE);
                    }
                    prevHE = he.get();
                    this->halfedges.push_back(std::move(he));
                }
                //sets pointers related to last half edge
                if (prevHE != nullptr && firstHE != nullptr && firstVert != nullptr) {
                    prevHE->setNext(firstHE);
                    firstVert->setHalfEdge(prevHE);
                }
                this->faces.push_back(std::move(face));
            }
        }
    }
}

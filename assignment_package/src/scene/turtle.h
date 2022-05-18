#ifndef TURTLE_H
#define TURTLE_H
#include "glm_includes.h"

class Turtle {
public:
    glm::vec2 pos;
    glm::vec2 orient;
    float length;
    int depth;

    Turtle();
    Turtle(glm::vec2 pos, glm::vec2 orient, float dist, int depth);
    void rotateL(float angle);
    void rotateR(float angle);
    void moveF();


};

#endif // TURTLE_H

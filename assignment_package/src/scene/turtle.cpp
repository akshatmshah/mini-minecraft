#include "turtle.h"

Turtle::Turtle(glm::vec2 pos, glm::vec2 orient, float dist, int depth)
    : pos(pos), orient(orient), length(dist), depth(depth) {}

Turtle::Turtle()
    : pos(glm::vec2(0.f, 0.f)), orient(glm::vec2(0.f, 1.f)),
      length(1.f), depth(1){}


//𝑥2=cos𝛽𝑥1−sin𝛽𝑦1
//𝑦2=sin𝛽𝑥1+cos𝛽𝑦1
void Turtle::rotateL(float angle){
    orient = glm::vec2(glm::cos(-angle)*orient.x - glm::sin(-angle)*orient.y, glm::sin(-angle)*orient.x + glm::cos(-angle)*orient.y);
}

void Turtle::rotateR(float angle){
    orient = glm::vec2(glm::cos(angle)*orient.x - glm::sin(angle)*orient.y, glm::sin(angle)*orient.x + glm::cos(angle)*orient.y);
}

void Turtle::moveF(){
    //randomize this
    pos = pos + (orient * length);
}

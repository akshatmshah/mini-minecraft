#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "smartpointerhelp.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

class Joint {
public:
    Joint(Joint* parent, glm::vec3 pos, glm::quat rotation);

    Joint* parent;
    std::vector<uPtr<Joint>> children;
    glm::vec3 relative_pos;
    glm::quat rotation;
    glm::mat4 bindMatrix;
    int id;
    bool isSelected;

    static inline int objCount{ 0 };

    glm::mat4 getLocalTransformation();
    glm::mat4 getOverallTransformation();
    glm::vec3 getAbsolutePos();
    void addChild(uPtr<Joint>);
    glm::mat4 bind();
    void rotate(glm::vec3 axis, float degrees);

    void setPos(glm::vec3);

};



#include "joint.h"

Joint::Joint(Joint* parent, glm::vec3 pos, glm::quat rotation)
    :parent(parent), children(), relative_pos(pos), rotation(rotation), bindMatrix(glm::mat4()), id(objCount++), isSelected(false)
{}


void Joint::addChild(uPtr<Joint> child){
    this->children.push_back(std::move(child));
}

glm::mat4 Joint::getLocalTransformation() {
    return glm::translate(glm::mat4(1.f), relative_pos) * glm::mat4_cast(this->rotation);
}

glm::mat4 Joint::getOverallTransformation() {
    if (parent == nullptr) {
        return this->getLocalTransformation();
    }
    return parent->getOverallTransformation() * this->getLocalTransformation();
}

glm::mat4 Joint::bind() {
    this->bindMatrix = glm::inverse(this->getOverallTransformation());
    return this->bindMatrix;
}

void Joint::setPos(glm::vec3 pos)
{
    this->relative_pos = pos;
}

void Joint::rotate(glm::vec3 axis, float degrees) {
    this->rotation *= glm::rotate(glm::quat(), glm::radians(degrees), axis);
}

glm::vec3 Joint::getAbsolutePos() {
    if (parent == nullptr) {
        return this->relative_pos;
    }
    return parent->getAbsolutePos() + this->relative_pos;
}


//
// Created by Max Godefroy on 21/11/2020.
//

#include "kryne-engine/3DObjects/Object3D.h"


Object3D::Object3D()
{
    this->id = Object3D::getId();
    this->scale = glm::vec3(1.f, 1.f, 1.f);
}


uint32_t Object3D::nextId()
{
    // Use static atomic to handle next id thread-safely
    static std::atomic<uint32_t> uid { 0 };
    return ++uid;
}


void Object3D::calculateLocalTransform()
{
    const auto scaleMatrix = glm::scale(glm::mat4(1), this->scale);
    const auto rotationMatrix = glm::toMat4(this->quaternion);
    const auto translationMatrix = glm::translate(glm::mat4(1), this->position);
    this->localTransform = translationMatrix * rotationMatrix * scaleMatrix;
}


void Object3D::updateTransform(bool force)
{
    if ((this->matrixWorldNeedsUpdate || force) && this->visible)
    {
        calculateLocalTransform();

        this->matrixWorld = this->parent != nullptr ?
                this->parent->matrixWorld * this->localTransform :
                this->localTransform;
        this->normalMatrix = mat3(transpose(inverse(this->matrixWorld)));

        for (const auto& child : this->children)
            child->updateTransform(true);

        this->matrixWorldNeedsUpdate = false;
    }
}


const glm::mat4 &Object3D::updateParents(const Object3D *caller)
{
    if (this->matrixWorldNeedsUpdate)
        this->calculateLocalTransform();

    this->matrixWorld = this->parent != nullptr ?
                        this->parent->updateParents(this) * this->localTransform :
                        this->localTransform;
    this->normalMatrix = mat3(transpose(inverse(this->matrixWorld)));

    for (const auto &child : this->children) {
        if (child.get() != caller)
            child->setWorldMatrixNeedsUpdate();
    }

    return this->matrixWorld;
}


void Object3D::add(unique_ptr<Object3D> child)
{
    if (child->parent != nullptr)
        child->parent->remove(child.get());

    child->parent = this;
    this->children.push_back(std::move(child));
}


unique_ptr<Object3D> Object3D::remove(Object3D *childToRemove)
{
    for (auto it = this->children.begin(); it != this->children.end(); it++) {
        if (it->get() == childToRemove) {
            auto result = std::move(*it);
            this->children.erase(it);
            return result;
        }
    }
    return nullptr;
}


void Object3D::traverse(Object3D::TraverseCallback callback)
{
    callback(this);
    for (const auto &child : this->children)
        child->traverse(callback);
}


vector<Object3D *> Object3D::getChildren() {
    vector<Object3D *> result;
    for (const auto &child : this->children)
        result.push_back(child.get());
    return result;
}


Object3D::~Object3D()
{
    for (const auto& child : this->children) {
        child->parent = nullptr;
    }
}


glm::vec3 Object3D::getWorldPosition() {
    if (this->parent == nullptr) {
        return this->position;
    } else {
        this->updateParents(nullptr);
        auto p = this->parent->matrixWorld * glm::vec4(position, 1.);
        return glm::vec3(p) * (1.f / p.w);
    }
}


void Object3D::applyLookAt(const glm::vec3 &eye, const glm::vec3 &target, const glm::vec3 &up)
{
    this->setQuaternion(glm::toQuat(Math::getLookAtMatrix(eye, target, up)));
}

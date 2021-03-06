/**
 * @file
 * @author Max Godefroy
 * @date 21/11/2020
 */

#ifndef INC_KRYNE_ENGINE_OBJECT3D_H
#define INC_KRYNE_ENGINE_OBJECT3D_H


// Circular dependencies forward declaration
class Entity;
class Scene;


#include <memory>
#include <vector>
#include <queue>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <kryne-engine/Math/Transform.hpp>
#include "Scene.h"


using namespace std;

/**
 * An object in 3D space
 */
class Transform {


friend class Scene;


// -- Base --

public:

    /**
     * Initializes the transforms and saves its attached entity.
     */
    explicit Transform(Entity *entity);

    /**
     * Object3D destructor.
     * All its children's #parent value will be set to `nullptr`, and it will be removed from its parent (if it has
     * one).
     */
    virtual ~Transform();

    /**
     * Returns the entity this transform is attached to
     * @return A raw pointer to the attached entity.
     */
    [[nodiscard]] Entity *getEntity() const { return attachedEntity; }

    /**
     * Attaches the transform (and its attached entity) to a scene.
     * @param scene     The scene to attach to. Can be null.
     * @warning If the transform has a parent, will remove itself from its children before setting the scene.
     */
    void setScene(Scene *scene);

private:

    /// The entity this transform is attached to.
    Entity *attachedEntity;

    /// The scene this transform is attached to.
    Scene *attachedScene = nullptr;


// -- Hierarchy --

public:

    /**
     * The traversal callback function type.
     */
    typedef void (*TraverseCallback)(Transform *object);

    /**
     * Adds a child to the object. If the new child already has a parent, call #remove() first.
     * @param child The new child to add.
     */
    void add(Transform *child);

    /**
     * Removes the provided object from this object's children, if it belonged to it.
     * @param childToRemove The raw pointer to the child to remove.
     * @return `true` is the provided transform was removed, `false` otherwise
     * @note    The child won't be detached from the scene by default. Use Transform::setScene() on the child with
     *          `nullptr` as parameter to detach it from the scene completely.
     */
    bool remove(Transform *childToRemove);

    /**
     * Removes this object from its parent, if it has one.
     */
    void removeFromParent() { if (this->parent) this->parent->remove(this); }

    /**
     * Retrieve the object's parent
     * @return A raw pointer to the object's parent. May be null.
     */
    [[nodiscard]] Transform *getParent() const { return parent; }

    /**
     * @brief Sets the transform parent in an unsafe way.
     *
     * @details
     * Simply sets the #parent property without any of the other complex operations. <br>
     * The current transform will have a parent pointing to another transform, but the inverse is false. This transform
     * won't be registered as a child, and won't be associated to any scene. <br>
     * Use this operation if you really now what you're doing.
     *
     * For an example, see the transform handling of light cameras in ShadowMapProcess and LightingRegistrySystem.
     *
     * @warning
     * Since this transform is not registered as a child, #parent may point to an invalidated address. As such, it needs
     * to be updated regularly , and reset when appropriate.
     *
     * @param newParent The pointer to the parent transform.
     */
    inline void unsafeSetParent(Transform *newParent)
    {
        this->removeFromParent();
        this->parent = newParent;
    };

    /**
     *
     * @param callback
     */
    void traverse(TraverseCallback callback);

    /**
     * Retrieve the object's children
     * @return A list of raw pointers to the children
     */
    [[nodiscard]] const vector<Transform *> &getChildren() const { return children; }

protected:

    /// The parent of the object
    Transform *parent = nullptr;

    /// The object children
    vector<Transform *> children {};


// -- Transform --

/**
 * Replace the needs update value check according to the defined behaviour
 */
#ifndef NO_CHANGE_CHECK
#define compareNeedsUpdate(a,b) !glm::all(glm::equal(a,b))
#else
#define compareNeedsUpdate(a,b) true
#endif


public:

    /**
     * Updates the object transform if needed.
     * @param force             Set to true to force the update
     * @param updateChildren    Set to true to update children transforms as well.
     */
    virtual void updateTransform(bool force, bool updateChildren);

    /**
     * Update the object itself along its parents.
     * @param caller  The child that called the object. Can be null
     * @return The new matrixWorld value of this object
     */
    virtual const glm::mat4 &updateParents(const Transform *caller);

    /**
     * Retrieve the current object position.
     */
    [[nodiscard]] const glm::vec3 &getPosition() const { return position; }

    /**
     * Change the object position
     * @param pos   The new position value
     */
    virtual void setPosition(const glm::vec3 &pos)
    {
        matrixWorldNeedsUpdate |= compareNeedsUpdate(pos, Transform::position);
        Transform::position = glm::vec3(pos);
    }

    /**
     * Retrieve the current object quaternion.
     */
    [[nodiscard]] const glm::quat &getQuaternion() const { return quaternion; }

    /**
     * Change the object quaternion.
     * @param quat  The new quaternion value.
     */
    void setQuaternion(const glm::quat &quat)
    {
        matrixWorldNeedsUpdate |= compareNeedsUpdate(quat, Transform::quaternion);
        Transform::quaternion = glm::quat(quat);
        Transform::euler = glm::eulerAngles(quat);
    }

    /**
     * Retrieve the current object euler angles
     */
    [[nodiscard]] const vec3 &getEuler() const { return euler; }

    /**
     * Change the object rotation.
     * @param newEuler The new euler angles value.
     */
    void setEuler(const vec3 &newEuler)
    {
        matrixWorldNeedsUpdate |= compareNeedsUpdate(newEuler, Transform::euler);
        Transform::euler = newEuler;
        Transform::quaternion = glm::quat(newEuler);
    }

    /**
     * Retrieve the current object scale.
     */
    [[nodiscard]] const glm::vec3 &getScale() const { return scale; }

    /**
     * Changes the object scale.
     * @param s The new scale information.
     */
    void setScale(const glm::vec3 &s)
    {
        matrixWorldNeedsUpdate |= compareNeedsUpdate(s, Transform::scale);
        Transform::scale = glm::vec3(s);
    }

    /**
     * Marks the object as needing a matrix world update.
     */
    void setWorldMatrixNeedsUpdate() { this->matrixWorldNeedsUpdate = true; }

    /**
     * Retrieve the world matrix for this object
     */
    const mat4 &getWorldMatrix() const { return this->matrixWorld; }

    /**
     * Retrieve the normal matrix for this object
     */
    [[nodiscard]] const mat3 &getNormalMatrix() const { return this->normalMatrix; }

protected:

    /**
     * Computes the local transform matrix.
     */
    void calculateLocalTransform();

protected:

    /// The local transform matrix.
    glm::mat4 localTransform {};

    /// The object world matrix.
    glm::mat4 matrixWorld {};

    /// The world normal matrix of the object.
    glm::mat3 normalMatrix {};

    /// Set to true to update the world matrix at the next #updateTransform() call.
    bool matrixWorldNeedsUpdate = true;

    /// The local position of the object.
    glm::vec3 position {};

    /// The local rotation quaternion.
    glm::quat quaternion {};

    /// The local rotation euler angles.
    glm::vec3 euler {};

    /// The local object scale.
    glm::vec3 scale {};


public:

    /**
     * Retrieves the world position of the object
     * @return A `vec3` of the object's world position.
     */
    glm::vec3 getWorldPosition();

    /**
     * Changes the object's rotation (through #quaternion) for it to look at the target position, using the current
     * transform position as the 'eye' position.
     * @param target    The target position in world coordinates.
     * @param up        The up direction in world coordinates.
     * @param swap      Set to `true` to swap the target and eye positions. Useful for camera lookAt operations.
     */
    void lookAt(const vec3 &target = vec3(0), const vec3 &up = vec3(0, 1, 0), bool swap = false);

};



// Circular dependencies includes
#include <kryne-engine/Core/Entity.h>


#endif //INC_KRYNE_ENGINE_OBJECT3D_H

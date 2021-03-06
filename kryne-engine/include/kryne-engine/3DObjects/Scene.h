/**
 * @file
 * @author Max Godefroy
 * @date 20/11/2020
 */

#ifndef INC_KRYNE_ENGINE_SCENE_H
#define INC_KRYNE_ENGINE_SCENE_H

class Entity;
class Transform;
class EnvironmentMap;

#include <unordered_set>
#include <memory>


using namespace std;


/**
 * An object for regrouping entities into a single scene
 */
class Scene {


public:

    /**
     * Retrieves the set of entities in the scene.
     * @return An unordered set of of pointer to the entities instances attached to this scene
     */
    [[nodiscard]] const unordered_set<Entity *> &getEntities() const { return sceneEntities; }

    /**
     * Attaches an entity to this scene
     * @param transform     The entity's transform
     */
    void add(Transform *transform);

    /**
     * Detaches an entity from this scene
     * @param transform     The entity's transform
     */
    void remove(Transform *transform);

    /**
     * Retrieves the top-level entities of the scene (meaning the super-parent entities)
     * @return An unordered set of those entities.
     */
    const unordered_set<Entity *> &getTopLevelEntities();

    /// @brief Retrieves the skybox envmap
    [[nodiscard]] const shared_ptr<EnvironmentMap> &getSkyboxEnvMap() const { return skyboxEnvMap; }

    /// @brief Changes the skybox envmap
    void setSkyboxEnvMap(const shared_ptr<EnvironmentMap> &value) { Scene::skyboxEnvMap = value; }


protected:

    /// A flat set of entities
    unordered_set<Entity *> sceneEntities {};

    /// TODO: Update it automatically
    unordered_set<Entity *> topLevelEntities {};

    /// The skybox environment map for the scene
    shared_ptr<EnvironmentMap> skyboxEnvMap {};


};


#include <kryne-engine/Core/Entity.h>
#include <kryne-engine/Textures/EnvironmentMap.hpp>


#endif //INC_KRYNE_ENGINE_SCENE_H

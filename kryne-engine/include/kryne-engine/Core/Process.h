/**
 * @file
 * @author Max Godefroy
 * @date 22/12/2020.
 */

#ifndef KRYNE_ENGINE_PROCESS_H
#define KRYNE_ENGINE_PROCESS_H


#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <stack>

#include <kryne-engine/enums/SystemTypes.h>
#include "kryne-engine/Dispatch/Dispatcher.h"
#include <kryne-engine/Utils/UniquePtrVector.hpp>
#include <kryne-engine/Utils/FrameTime.hpp>


class Entity;
class Scene;
class System;
class GraphicContext;
class RenderMesh;
class UIRenderer;
class LoopRenderer;
class PlayerInput;


using namespace std;



// ===========
// Core process logic
// ===========

class Process {


    friend struct ProcessCommon;


public:

    explicit Process(GraphicContext *context);

    /**
     * Instantiates a new scene.
     */
    Scene *makeScene();

    /**
     * Runs a single game loop
     */
    void runLoop();

    /**
     * Returns the graphic context used for this process.
     */
    [[nodiscard]] GraphicContext *getGraphicContext() const { return this->context; }

    /**
     * Set the value of #currentScene
     * @param scene
     */
    void setCurrentScene(Scene *scene) { this->currentScene = scene; }

    [[nodiscard]] Scene *getCurrentScene() const { return currentScene; }

    /**
     * Retrieves the UIRenderer list.
     */
    Utils::UniquePtrVector<UIRenderer> &getUIRenderers() { return this->uiRenderers; }

    /**
     * @brief Retrieves the time data for the last frame.
     */
    [[nodiscard]] const Utils::FrameTime::Data &getLastFrameTimeData() const { return lastFrameTimeData; }

protected:

    /**
     * @brief Processes a provided entity, executing all pertinent systems before rendering.
     *
     * @param entity    The entity to process.
     * @param renderer  The renderer used for displaying the entity render meshes.
     */
    void processEntity(Entity *entity, LoopRenderer *renderer) const;

    /**
     * @brief Processes the post-rendering systems for all entities.
     */
    void handlePostRender() const;

    /**
     * @brief Processes all logic systems for priority components
     *
     * @param entities  The priority entities that need pre-processing.
     */
    void runPriorityPreProcesses(const unordered_set<Entity *> &entities) const;

protected:

    /// The graphical context for this process
    GraphicContext *context;

    /// The current scene used in the loop
    Scene *currentScene = nullptr;

    /// Tool for measuring the frame times.
    Utils::FrameTime frameTimer;

    /// The timings of the last frame.
    Utils::FrameTime::Data lastFrameTimeData;

    /// The scenes for this process
    unordered_set<unique_ptr<Scene>> scenes;

    /// The list of UI renderers
    Utils::UniquePtrVector<UIRenderer> uiRenderers;


// ===========
// Entities-related interface
// ===========

public:

    /**
     * Initializes an entity and attaches it to this process.
     * @tparam T        The entity class
     * @tparam Args     The class constructor argument types collection.
     * @param args      The arguments for the class constructor
     * @return A pointer to the newly created entity. The pointer is linked to a shared pointer owned only by the
     *         process. Weak reference to the object can be retrieved using #getWeakReference
     */
    template<typename T, typename... Args>
    inline T *makeEntity(Args&&... args)
    {
        static_assert(is_convertible_v<T*, Entity*>, "Class must inherit from Entity");

        const auto entity = make_shared<T>(this, forward<Args>(args)...);
        this->processEntities.emplace(pair(entity.get(), entity));
        return entity.get();
    }

    /**
     * Retrieves a weak reference of an entity attached to the process.
     * @param system    The attached entity.
     * @return  A weak reference to the provided entity. Will link to nullptr if the provided entity is not attached to
     *          the process.
     */
    weak_ptr<Entity> getWeakReference(Entity *entity);

    /**
     * Detaches an entity from the process, meaning it should be deleted (if there is no other shared reference active
     * at the moment).
     * @param entity    The entity to detach.
     * @return `true` if the provided entity was attached. `false` otherwise.
     */
    bool detachEntity(Entity *entity);

protected:

    /// The set of entities attached to this process, mapping the pointer to its corresponding shared pointer.
    unordered_map<Entity *, shared_ptr<Entity>> processEntities;


// ===========
// Systems-related interface
// ===========

public:

    /**
     * Initializes a system and attaches it to this process.
     * @tparam T        The system class
     * @tparam Args     The class constructor argument types collection.
     * @param args      The arguments for the class constructor
     * @return A pointer to the newly created system. The pointer is linked to a shared pointer owned only by the
     *         process. Weak reference to the object can be retrieved using #getWeakReference
     */
    template<typename T, typename... Args>
    T *makeSystem(Args&&... args)
    {
        static_assert(std::is_convertible_v<T*, System*>, "Class must inherit from System");

        const auto system = make_shared<T>(this, forward<Args>(args)...);
        this->processSystems.emplace(system.get(), system);

        this->systemsByType[system->getType()].push_back(system.get());

        return system.get();
    }

    /**
     * Retrieves a weak reference of a system attached to the process.
     * @param system    The attached system.
     * @return  A weak reference to the provided system. Will link to nullptr if the provided system is not attached to
     *          the process.
     */
    weak_ptr<System> getWeakReference(System *system);

    /**
     * Detaches a system from the process, meaning it should be deleted (if there is no other shared reference active
     * at the moment).
     * @param system    The system to detach.
     * @return `true` if the provided system was attached. `false` otherwise.
     */
    bool detachSystem(System *system);

protected:

    /// The set of systems attached to this process, mapping the pointer to its corresponding shared pointer.
    unordered_map<System *, shared_ptr<System>> processSystems;

    /// A map of processes by type
    vector<System *> systemsByType[SystemTypes::COUNT] {};


// ===========
// Helper functions
// ===========


public:

    /**
     * Helper function for retrieving the current player input handler.
     * @return The PlayerInput for the current graphic context
     */
    PlayerInput *getPlayerInput();

};



#include "Entity.h"
#include "System.h"
#include "GraphicContext/GraphicContext.h"
#include <kryne-engine/Rendering/RenderMesh.h>
#include <kryne-engine/UI/UIRenderer.hpp>


#endif //KRYNE_ENGINE_PROCESS_H

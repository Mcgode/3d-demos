/**
 * @file
 * @author Max Godefroy
 * @date 06/02/2021.
 */

#pragma once


#include <vector>
#include <unordered_map>
#include <memory>

#include <kryne-engine/enums/RenderMode.h>
#include <kryne-engine/Math/Frustum.hpp>


class RenderMesh;
class Camera;
class Scene;
class Entity;
class PostProcessPass;


using namespace std;


/**
 * A renderer that will be run in the game loop.
 */
class LoopRenderer {

public:

    virtual void prepareFrame();

    virtual void handleMesh(RenderMesh *renderMesh) = 0;

    virtual std::vector<Entity *> parseScene(Scene *scene) = 0;

    virtual void setCamera(Camera *camera) { this->mainCamera = camera; }

    [[nodiscard]] RenderMode getRenderingMode() const { return this->renderMode; }

    virtual void setRenderingMode(RenderMode mode) { this->renderMode = mode; }

protected:

    RenderMode renderMode = ForwardRendering;

    Camera *mainCamera {};


// ================
// Post processing
// ================

public:

    /**
     * @brief Inserts a post process pass at the last valid position in the list.
     *
     * @param pass A unique pointer owning the pass to insert.
     */
    void addPass(unique_ptr<PostProcessPass> pass);

    /**
     * @brief Inserts a post process pass after another one.
     *
     * @details
     * The process will fail if it either can't find the other pass, or if the position is invalid in regards to
     * pass priority (defined by PostProcessPass::priority).
     *
     * @param pass The pass to insert.
     * @param name The name of the pass to insert after.
     * @return true if insertion was successful, false otherwise.
     */
    bool addPassAfter(unique_ptr<PostProcessPass> pass, const string &name);

    /**
     * @brief Inserts a post process pass before another one.
     *
     * @details
     * The process will fail if it either can't find the other pass, or if the position is invalid in regards to
     * pass priority (defined by PostProcessPass::priority).
     *
     * @param pass The pass to insert.
     * @param name The name of the pass to insert before.
     * @return true if insertion was successful, false otherwise.
     */
    bool addPassBefore(unique_ptr<PostProcessPass> pass, const string &name);

    /**
     * @brief Tries to remove a post process pass.
     *
     * @param name The name of the post process pass to remove.
     * @return A unique pointer owning the pass. Can be null if there was no pass with the provided name.
     */
    unique_ptr<PostProcessPass> removePass(const string &name);

protected:

    /**
     * @brief Inserts a post process pass unique pointer to #postProcessPasses.
     *
     * @param it The iterator pointing to the post process pass insert position.
     * @param pass The pass to insert into the list
     */
    void insertPostProcessPass(vector<unique_ptr<PostProcessPass>>::iterator it, unique_ptr<PostProcessPass> &pass);

    /**
     * @brief Removes a post process pass from #postProcessPasses.
     *
     * @param it The reverse iterator pointing to the post process pass to remove.
     * @return A unique pointer owning the removed post process pass.
     */
    unique_ptr<PostProcessPass> removePostProcessPass(vector<unique_ptr<PostProcessPass>>::reverse_iterator it);

protected:

    /// The ordered list of post process passes.
    vector<unique_ptr<PostProcessPass>> postProcessPasses {};


// ==================
// Frustum culling
// ==================

public:

    void computeFrustumCulling(RenderMesh *mesh);

protected:

    struct FrustumCullingData {

        Math::Frustum frustum;

        std::unordered_map<RenderMesh *, bool> culledMeshes {};

        explicit FrustumCullingData(Camera *camera);

    };

    /// The frustum culling state of meshes.
    std::unordered_map<Camera *, FrustumCullingData> frustumCulled;

};


#include "PostProcessPass.h"
#include <kryne-engine/Core/Entity.h>
#include <kryne-engine/Camera/Camera.h>
#include <kryne-engine/3DObjects/Scene.h>
#include <kryne-engine/Rendering/RenderMesh.h>
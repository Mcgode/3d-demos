//
// Created by max on 22/04/19.
//

#ifndef INC_3D_DEMOS_SCENE_H
#define INC_3D_DEMOS_SCENE_H


#include <kryne-engine/Core/Window.h>
#include <kryne-engine/Rendering/MainRenderer.h>
#include <kryne-engine/Rendering/ShadowMapping/ShadowMapHandler.h>

/**
 * A class for handling a basic 3D scene
 */
class Scene {

public:

    /**
     * A class for handling a basic 3D scene
     * @param window        The window for the current process.
     * @param camera        The camera used for this scene
     */
    explicit Scene(Window *window, Camera *camera);

    /**
     * Changes the current camera
     * @param newCamera The new camera you want to use in the scene
     */
    Camera *updateCamera(Camera *newCamera);

    /**
     * Adds a new drawable root node to the scene
     * @param node The node to add to the scene
     */
    void addDrawable(HierarchicalNode *node);


    /**
     * The method for launching the rendering loop
     * @param params Additional parameters to pass down during the rendering
     */
    void renderLoop(AdditionalParameters *params);

    /**
     * Sets the directional light of the scene
     * @param light The directional light for the scene
     */
    void setDirectionalLight(DirectionalLight *light);

    /**
     * Adds a point light to scene
     * @param light The point light that you want to be added to the scene
     */
    void addPointLight(PointLight *light);

    Camera *getCamera() const;

    void setSkybox(Skybox *skybox, SkyboxDrawOrder drawOrder);

    [[nodiscard]] ShadowMapHandler *getShadowMapHandler() const;

    ~Scene();

private:

    /// The window where the rendering display will happen
    Window *window;

    /// All the drawable root nodes to render in the scene
    std::vector<HierarchicalNode *> rootNodes;

    /// The directional light for the scene
    DirectionalLight *directionalLight{};

    /// The point lights of the scene
    std::vector<PointLight *> pointLights;

    MainRenderer *mainRenderer{};

    ShadowMapHandler *shadowMapHandler;

};


#endif //INC_3D_DEMOS_SCENE_H
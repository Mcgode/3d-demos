//
// Created by Max Godefroy on 20/11/2020.
//

#include "kryne-engine/Rendering/Renderer.h"

Renderer::Renderer(uint16_t width, uint16_t height) :
        associatedWindow(make_unique<Window>(width, height)),
        playerInput(PlayerInput::tryMakeInput(associatedWindow->getGlfwWindow())),
        renderingState(FrontSide, true)
{}


void Renderer::render(Scene *scene, Camera *camera)
{
    scene->traverse(&updateObjects);
    camera->update(false);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->renderObject(scene, camera);
}


void Renderer::updateObjects(Object3D *object)
{
    object->update(false);
}


void Renderer::renderObject(Object3D *object, Camera *camera)
{
    if (!object->isVisible())
        return;

    auto mesh = dynamic_cast<Mesh *>(object);
    if (mesh) {
        const auto& material = mesh->getMaterial();

        material->prepareShader(mesh->getGeometry().get());

        // Only update external rendering state once, before drawing any object.
        // Since each object can have a different required state in this regard, it needs to be checked every single time.
        // No need to reset to a base state, since it will be updated dynamically, to fit the required state.

        if (this->renderingState.getSide() != material->getSide())
            renderingState.setSide(material->getSide());

        if (renderingState.isDepthTestEnabled() != material->isDepthTest())
            renderingState.setDepthTest(material->isDepthTest());

        if (renderingState.isDepthWriteEnabled() != material->isWriteDepth())
            renderingState.setDepthWrite(material->isWriteDepth());


        // Renderer-level uniforms
        material->setUniform("projectionMatrix", camera->getProjectionMatrix());
        material->setUniform("viewMatrix", camera->getViewMatrix());

        // Run mesh updates
        mesh->onBeforeRender(camera);

        // Upload uniforms
        material->getShader()->updateUniforms();

        // Finally draw the object
        mesh->getGeometry()->draw(material->getPrimitiveType());

        // Reset shader use, just in case
        material->resetUse();
    }

    for (auto child: object->getChildren())
        renderObject(child, camera);
}

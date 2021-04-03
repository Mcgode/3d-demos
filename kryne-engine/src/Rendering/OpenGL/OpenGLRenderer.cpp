/**
 * @file
 * @author Max Godefroy
 * @date 06/02/2021.
 */

#include "kryne-engine/Rendering/OpenGL/OpenGLRenderer.h"


OpenGLRenderer::OpenGLRenderer(RenderingState *renderingState, const ivec2 &size) :
        LoopRenderer(make_unique<OpenGLScreenFramebuffer>(size.x, size.y),
                     make_unique<OpenGLFramebuffer>(size.x, size.y),
                     make_unique<OpenGLFramebuffer>(size.x, size.y),
                     size),
        renderingState(renderingState)
{
    this->writeFramebuffer->addColorAttachment();
    this->writeFramebuffer->setUpDepthLayer();

    this->readFramebuffer->addColorAttachment();
    this->readFramebuffer->setUpDepthLayer();

    this->fullscreenPlane = make_unique<PlaneBufferGeometry>(2.f, 2.f);
}


void OpenGLRenderer::handleMesh(RenderMesh *renderMesh)
{
    const auto camera = this->mainCamera;

    if (camera == nullptr) return;

    if (renderMesh->isFrustumCull())
    {
        const auto it = this->frustumCulled.find(camera);
        if (it == this->frustumCulled.end())
            return;

        const auto culledIt = it->second.culledMeshes.find(renderMesh);
        if (culledIt != it->second.culledMeshes.end() && culledIt->second)
            return;
    }

    const auto& material = renderMesh->getMaterial();
    const auto& geometry = renderMesh->getGeometry();
    const auto transform = renderMesh->getEntity()->getTransform();

    material->prepareShader(geometry.get());

    // Only update external rendering state once, before drawing any object.
    // Since each object can have a different required state in this regard, it needs to be checked every single time.
    // No need to reset to a base state, since it will be updated dynamically, to fit the required state.

    if (renderingState->getSide() != material->getSide())
        renderingState->setSide(material->getSide());

    if (renderingState->isDepthTestEnabled() != material->isDepthTest())
        renderingState->setDepthTest(material->isDepthTest());

    if (renderingState->isDepthWriteEnabled() != material->isWriteDepth())
        renderingState->setDepthWrite(material->isWriteDepth());

    // Renderer-level uniforms
    material->setUniform("projectionMatrix", camera->getProjectionMatrix());
    material->setUniform("viewMatrix", camera->getViewMatrix());
    material->setUniform("cameraPosition", camera->getTransform()->getWorldPosition());
    material->setUniform("modelMatrix", transform->getWorldMatrix());
    material->setUniform("normalMatrix", transform->getNormalMatrix());

    // Run mesh updates
    renderMesh->onBeforeRender(camera);

    // Upload uniforms
    material->getShader()->updateUniforms();

    // Finally draw the object
    geometry->draw(material->getPrimitiveType());

    // Reset shader use, just in case
    material->resetUse();
}


void OpenGLRenderer::prepareFrame()
{
    assertIsMainThread();

    LoopRenderer::prepareFrame();

    if (this->framePostProcessPasses.empty())
        this->screenFramebuffer->setAsRenderTarget();
    else
        this->writeFramebuffer->setAsRenderTarget();

    glClearColor(0.f, 0.f, 0.f, 0.f);
    renderingState->setDepthWrite(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}


vector<Entity *> OpenGLRenderer::parseScene(Scene *scene)
{
    vector<Entity *> result;

    result.push_back(this->mainCamera);

    return result;
}


void OpenGLRenderer::renderToScreen()
{
    assertIsMainThread();

    for (size_t i = 0; i < this->framePostProcessPasses.size(); i++)
    {
        this->swapBuffers();
        auto &pass = this->framePostProcessPasses[i];
        pass->processPass(this,
                          this->readFramebuffer.get(),
                          (i + 1 == this->framePostProcessPasses.size()) ? this->screenFramebuffer.get() : this->writeFramebuffer.get());
    }
}


void OpenGLRenderer::textureRender(Material *material)
{
    glClearColor(0.f, 0.f, 0.f, 0.f);
    renderingState->setDepthWrite(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    material->prepareShader(this->fullscreenPlane.get());

    // Only update external rendering state once, before drawing any object.
    // Since each object can have a different required state in this regard, it needs to be checked every single time.
    // No need to reset to a base state, since it will be updated dynamically, to fit the required state.

    renderingState->setSide(FrontSide);
    renderingState->setDepthTest(false);
    renderingState->setDepthWrite(false);

    // Upload uniforms
    material->getShader()->updateUniforms();

    // Finally draw the object
    this->fullscreenPlane->draw(material->getPrimitiveType());

    // Reset shader use, just in case
    material->resetUse();
}

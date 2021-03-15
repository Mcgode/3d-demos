/**
 * @file
 * @author Max Godefroy
 * @date 14/03/2021.
 */

#include "kryne-engine/Rendering/LoopRenderer.h"


LoopRenderer::LoopRenderer(unique_ptr<Framebuffer> screenFramebuffer,
                           unique_ptr<Framebuffer> readFramebuffer,
                           unique_ptr<Framebuffer> writeFramebuffer) :
   screenFramebuffer(move(screenFramebuffer)),
   readFramebuffer(move(readFramebuffer)),
   writeFramebuffer(move(writeFramebuffer))
{}


LoopRenderer::FrustumCullingData::FrustumCullingData(Camera *camera) :
    frustum(camera->getProjectionMatrix() * camera->getViewMatrix())
{}


void LoopRenderer::prepareFrame()
{
    this->frustumCulled.clear();

    FrustumCullingData mainCamFCD(mainCamera);
    this->frustumCulled.emplace(mainCamera, mainCamFCD);

    this->framePostProcessPasses.clear();
    for (const auto &pass : this->postProcessPasses)
    {
        if (pass->isEnabled())
            this->framePostProcessPasses.push_back(pass.get());
    }
}


void LoopRenderer::computeFrustumCulling(RenderMesh *mesh)
{
    if (!mesh->isFrustumCull())
        return;

    for (auto &pair : this->frustumCulled)
    {
        pair.second.culledMeshes.emplace(mesh, !pair.second.frustum.sphereIntersects(mesh->getBoundingSphere()));
    }
}


void LoopRenderer::insertPostProcessPass(vector<unique_ptr<PostProcessPass>>::iterator it, unique_ptr<PostProcessPass> &pass)
{
    while (it != this->postProcessPasses.end())
    {
        swap(pass, *it);
        ++it;
    }

    this->postProcessPasses.emplace_back(pass.release());
}


unique_ptr<PostProcessPass> LoopRenderer::removePostProcessPass(vector<unique_ptr<PostProcessPass>>::reverse_iterator it)
{
    auto it2 = this->postProcessPasses.rbegin();
    unique_ptr<PostProcessPass> ptr(nullptr);

    while (it2 != it)
    {
        swap(ptr, *it2);
        it2++;
    }

    swap(ptr, *it);
    this->postProcessPasses.pop_back();
    return ptr;
}


void LoopRenderer::addPass(unique_ptr<PostProcessPass> pass)
{
    auto it = this->postProcessPasses.begin();

    while (this->postProcessPasses.end() != it)
    {
        if ((*it)->getPriority() >= pass->getPriority())
            break;
    }

    this->insertPostProcessPass(it, pass);
}


bool LoopRenderer::addPassAfter(unique_ptr<PostProcessPass> pass, const string &name)
{
    auto it = this->postProcessPasses.begin();

    for (; it != this->postProcessPasses.end(); it++)
    {
        if ((*it)->getName() == name)
            break;
    }

    if (it == this->postProcessPasses.end())
        return false;

    if ((*it)->getPriority() > pass->getPriority())
        return false;

    it++;
    this->insertPostProcessPass(it, pass);
    return true;
}


bool LoopRenderer::addPassBefore(unique_ptr<PostProcessPass> pass, const string &name)
{
    auto it = this->postProcessPasses.begin();
    auto before = it;

    for (; it != this->postProcessPasses.end(); it++)
    {
        if ((*it)->getName() == name)
            break;
        before = it;
    }

    if (it == this->postProcessPasses.end())
        return false;

    if (it != this->postProcessPasses.begin() && (*before)->getPriority() > pass->getPriority())
        return false;

    this->insertPostProcessPass(it, pass);
    return true;
}


unique_ptr<PostProcessPass> LoopRenderer::removePass(const string &name)
{
    auto it = this->postProcessPasses.rbegin();
    for (; it != this->postProcessPasses.rend(); ++it)
    {
        if ((*it)->getName() == name)
            break;
    }

    if (it == this->postProcessPasses.rend())
        return unique_ptr<PostProcessPass>();

    return this->removePostProcessPass(it);
}

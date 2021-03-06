//
// Created by max on 26/07/2019.
//

#include <kryne-engine/Core/GeometricShapes.h>
#include <kryne-engine/Rendering/RenderScene.h>
#include "Floor.h"

Floor::Floor(const std::string &textureName, RenderScene *scene, OldDirectionalLight *light)
{
    this->shader = new Shader("Quad/ShadowMap");

    std::vector<glm::vec3> vertices, normals, tangents;
    std::vector<glm::vec2> textureCoordinates;

    this->handler = scene->getShadowMapHandler();
    this->light = light;

    generateQuadShape(1.0, &vertices, &normals, &textureCoordinates, &tangents);
    auto va = new VertexArray(&vertices);
    va->add_array(&normals);
    va->add_array(&textureCoordinates);
    va->add_array(&tangents);
    this->vertexArray = va;

    this->diffuseTexture = Texture2D::getTexture(textureName, "diff");
    this->normalMapTexture = Texture2D::getTexture(textureName, "norm");

    supportsLighting = true;
    maximumSupportedPointLights = 4;

    shadowCasting = HARD_SHADOW;

}

void Floor::draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model, AdditionalParameters *params) {
    shader->setFloat("material.shininess", 64.0f);
    shader->setVec3("material.ambient", glm::vec3(0.07));
    shader->setVec3("material.diffuse", glm::vec3(1.0));
    shader->setVec3("material.specular", glm::vec3(1.6));



    auto lsms = handler->getLightSpaceMatrices(light);
    auto sms = handler->getDirectionalShadowMaps(light);
    auto radii = light->getShadowCastRadii();
    for (int i = 0; i < sms.size(); i++) {
        Texture2D::textureSet(sms[i], shader, "directionalShadowMaps[" + std::to_string(i) + "].shadowMap");
        shader->setMat4("directionalShadowMaps[" + std::to_string(i) + "].lightSpaceMatrix", lsms[i]);
        shader->setFloat("directionalShadowMaps[" + std::to_string(i) + "].shadowBias", 0.01);
        shader->setFloat("directionalShadowMaps[" + std::to_string(i) + "].radius", radii[i]);
    }
    this->shader->setInt("amountOfShadowMaps", sms.size());

    this->diffuseTexture->setTexture(shader, "material.diffuseMap");
    this->normalMapTexture->setTexture(shader, "material.normalMap");
    BaseObject::draw(projection, view, model, params);
}


Floor::~Floor()
{
    delete this->shader;
    delete this->vertexArray;
    delete this->diffuseTexture;
    delete this->normalMapTexture;
}

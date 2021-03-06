//
// Created by max on 26/07/2019.
//

#include "kryne-engine/RenderTargets/FinalPostProcess.h"

FinalPostProcess::FinalPostProcess(Shader *shader)
{
    this->postProcessShader = shader;

    vector<glm::vec3> vertices;
    vector<glm::vec2> textureCoordinates;
    generateQuadShape(2.0, &vertices, nullptr, &textureCoordinates);
    this->va = new VertexArray(&vertices);
    this->va->add_array(&textureCoordinates);
}


void FinalPostProcess::setIntUniform(int value, const string &uniformName)
{
    this->postProcessShader->setUniform(uniformName, value);
}


void FinalPostProcess::setFloatUniform(float value, const string &uniformName)
{
    this->postProcessShader->setUniform(uniformName, value);
}


void FinalPostProcess::setVec2Uniform(glm::vec2 vector, const string &uniformName)
{
    this->postProcessShader->setUniform(uniformName, vector);
}


void FinalPostProcess::setVec3Uniform(glm::vec3 vector, const string &uniformName)
{
    this->postProcessShader->setUniform(uniformName, vector);
}


void FinalPostProcess::setVec4Uniform(glm::vec4 vector, const string &uniformName)
{
    this->postProcessShader->setUniform(uniformName, vector);
}


void FinalPostProcess::setTextureUniform(Texture2D *textureUniform, const string& uniformName)
{
    auto position = this->textures.find(uniformName);
    if (position != this->textures.end()) {
        if (nextIndex >= 32) {
            cerr << "Maximum amount of individual textures used for this shader" << endl;
            exit(EXIT_FAILURE);
        }
        this->textures.insert(pair<string, uint8_t>(uniformName, this->nextIndex));
//        textureUniform->setTexture(this->postProcessShader, uniformName);
        throw "Fix needed";
    } else {
//        textureUniform->setTexture(this->postProcessShader, uniformName);
        throw "Fix needed";
    }

}


void FinalPostProcess::draw()
{
    this->postProcessShader->use();
    this->va->execute(GL_TRIANGLES);
    Shader::resetUse();
}


FinalPostProcess::~FinalPostProcess()
{
    delete(this->postProcessShader);
}

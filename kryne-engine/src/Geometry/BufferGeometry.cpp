//
// Created by Max Godefroy on 21/11/2020.
//

#include "kryne-engine/Geometry/BufferGeometry.h"

#include <utility>


void BufferGeometry::setAttribute(const string &name, unique_ptr<BufferAttribute> attribute)
{
    GLuint location = this->nextLocation;

    auto l = this->attributes.find(name);
    if (l != this->attributes.end())
    {
        bool willNeedLocationRecompute = false;

        if (l->second.attribute->getItemSize() != attribute->getItemSize())
        {
            willNeedLocationRecompute = true;
        }
        else
        {
            location = l->second.location;
        }

        l->second = AttributeData(move(attribute), location, true);

        if (willNeedLocationRecompute)
            this->recomputeLocations();
    }
    else
    {
        this->nextLocation += (attribute->getItemSize() + 3) / 4;
        this->attributes.emplace(name, AttributeData(move(attribute), location, true));
    }

    if (name == "position")
        this->computeBoundingVolumes();

    this->updateNeeded = true;
    this->updateLength();
    this->updateLayoutCode();
}


void BufferGeometry::removeAttribute(const string &name)
{
    if (this->attributes.erase(name))
    {
        if (name == "position")
            this->computeBoundingVolumes();

        this->updateLength();
        this->recomputeLocations();
        this->updateNeeded = true;
    }
}

void BufferGeometry::update()
{
    for (auto location : this->locationsToDisable)
        glDisableVertexAttribArray( location );
    this->locationsToDisable.clear();

    if (this->vao == 0)
        glGenVertexArrays(1, &this->vao);

    for (const auto &attribPair : this->attributes)
    {
        if (attribPair.second.needsLink)
            attribPair.second.attribute->bindToVAO(this->vao, attribPair.second.location);
    }

    if (this->indicesChanged)
    {
        if (this->ebo == 0)
            glGenBuffers(1, &this->ebo);

        glBindVertexArray(this->vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indexes.size() * sizeof(uint32_t), this->indexes.data(), GL_STATIC_DRAW); //< TODO : handle non-static draw case
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        this->indicesChanged = false;
    }

    this->updateNeeded = false;
}


void BufferGeometry::updateLength()
{
    if (this->indexes.empty())
    {
        auto it = this->attributes.begin();
        this->length = it->second.attribute->getLength();

        for (; it != this->attributes.end(); ++it)
        {
            const auto l = it->second.attribute->getLength();
            if (l < this->length)
                this->length = l;
        }
    }
}


void BufferGeometry::recomputeLocations()
{
    GLuint currentLocation = 0;

    for ( auto &attributePair : this->attributes )
    {
        auto &p = attributePair.second;
        this->locationsToDisable.push_back(p.location);
        p.location = currentLocation;
        currentLocation += (p.attribute->getItemSize() + 3) / 4;
        p.needsLink = true;
    }

    this->nextLocation = currentLocation;
    this->updateLayoutCode();
    this->updateNeeded = true;
}


void BufferGeometry::draw(GLenum geometry)
{
    assertIsMainThread();

    for (const auto &p : this->attributes)
    {
        if (p.second.attribute->needsUpdate())
            p.second.attribute->updateData();
    }

    if (this->updateNeeded)
        this->update();

    glBindVertexArray(this->vao);
    if (this->ebo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
        glDrawElements(geometry, length, GL_UNSIGNED_INT, nullptr);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } else
        glDrawArrays(geometry, 0, length);
    glBindVertexArray(0);
}


void BufferGeometry::setIndices(vector<uint32_t> newIndexes)
{
    this->indexes = std::move(newIndexes);
    this->indicesChanged = true;
    this->updateNeeded = true;
    this->length = this->indexes.size();
}


BufferGeometry::~BufferGeometry()
{
    glDeleteVertexArrays(1, &this->vao);

    if (this->ebo)
        glDeleteBuffers(1, &this->ebo);
}


using namespace glm;
vec3 BufferGeometry::computeTangent(vec3 p0, vec3 p1, vec3 p2, vec2 uv0, vec2 uv1, vec2 uv2) {
    vec3 q0 = p1 - p0;
    vec3 q1 = p2 - p0;

    vec2 deltaUV0 = uv1 - uv0;
    vec2 deltaUV1 = uv2 - uv0;

    float f = 1.f / (deltaUV0.x * deltaUV1.y - deltaUV0.y * deltaUV1.x);

    vec3 T = vec3(
            f * (deltaUV1.y * q0.x - deltaUV0.y * q1.x),
            f * (deltaUV1.y * q0.y - deltaUV0.y * q1.y),
            f * (deltaUV1.y * q0.z - deltaUV0.y * q1.z)
    );

    return normalize(T);
}


bool BufferGeometry::computeTangents()
{
    BufferAttribute *positionsAttribute = nullptr,
                    *uvsAttribute = nullptr;

    for (const auto &attribute : this->attributes) {
        if (attribute.first == "position")
            positionsAttribute = attribute.second.attribute.get();

        if (attribute.first == "uv")
            uvsAttribute = attribute.second.attribute.get();

        if (positionsAttribute && uvsAttribute)
            break;
    }

    if (positionsAttribute == nullptr) {
        cout << "BufferGeometry::computeTangents() - No position attribute found, aborting" << endl;
        return false;
    }

    if (uvsAttribute == nullptr) {
        cout << "BufferGeometry::computeTangents() - No uv attribute found, aborting" << endl;
        return false;
    }

    const auto *positions = (const glm::vec3 *) (positionsAttribute->getData());
    const auto *uvs = (const glm::vec2 *) (uvsAttribute->getData());

    vector<glm::vec3> tangents;
    tangents.assign(std::min(positionsAttribute->getLength(), uvsAttribute->getLength()), glm::vec3(0));

    if (!this->indexes.empty()) {
        for (uint32_t i = 0; i < this->length; i += 3) {
            uint32_t j1 = this->indexes[i],
                     j2 = this->indexes[i+1],
                     j3 = this->indexes[i+2];

            glm::vec3 t = BufferGeometry::computeTangent(
                    positions[j1], positions[j2], positions[j3],
                    uvs[j1], uvs[j2], uvs[j3]);

            tangents[j1] += t;
            tangents[j2] += t;
            tangents[j3] += t;
        }

        for (auto &tangent : tangents)
            tangent = glm::normalize(tangent);
    } else {
        for (uint32_t i = 0; i < this->length; i += 3) {
            glm::vec3 t = BufferGeometry::computeTangent(
                    positions[i], positions[i+1], positions[i+2],
                    uvs[i], uvs[i+2], uvs[i+3]);

            tangents[i] = t;
            tangents[i+1] = t;
            tangents[i+2] = t;
        }
    }

    vector<float> tangentFloats;
    auto floats = (const float *)(tangents.data());
    tangentFloats.assign(floats, floats + tangents.size() * 3);
    this->setAttribute(
            "tangent",
            make_unique<BufferAttribute>(tangentFloats, 3));

    return true;
}


void BufferGeometry::updateLayoutCode()
{
    string fullCode;

    for (const auto &attribute : this->attributes) {

        string code = "layout (location = " + to_string(attribute.second.location) + ") in ";

        string type = attribute.second.attribute->inferTypeString();
        if (type.empty()) {
            cout << "Unable to infer type for attribute '"
                 << attribute.first << "' with item size "
                 << attribute.second.attribute->getItemSize() << endl;
            continue;
        }

        code += type + " " + attribute.first + ";\n";

        fullCode += code;
    }

    this->layoutCode = make_pair(fullCode, hash<string>{}(fullCode));
}


void BufferGeometry::computeBoundingVolumes()
{
    this->boundingSphere = Math::Sphere();
    this->boundingBox = Math::AxisAlignedBox();

    const auto it = this->attributes.find("position");
    if (it != this->attributes.end() && it->second.attribute->getItemSize() == 3)
    {
        const auto array = it->second.attribute->getData();
        for (size_t i = 0; i < it->second.attribute->getLength(); i += 3)
        {
            this->boundingBox.expandByPoint(vec3(array[i],
                                                   array[i + 1],
                                                   array[i + 2]));
        }
        this->boundingSphere.fromBox(this->boundingBox);
    }
}


vector<string> BufferGeometry::getInfo() const
{
    vector<string> v;

    auto posIt = this->attributes.find("position");
    if (posIt != this->attributes.end())
    {
       auto &attribute = posIt->second.attribute;
       string s = to_string(attribute->getLength()) + " vertices, ";
       if (this->indexes.empty())
           s += to_string(attribute->getLength() / 3) + " triangles";
       else
           s += to_string(this->indexes.size() / 3) + " triangles";
       v.push_back(s);
    }

    v.emplace_back("Attributes:");
    for (const auto &pair : this->attributes)
        v.push_back(" - " + pair.first + " (" + pair.second.attribute->inferTypeString() + ")");

    return v;
}

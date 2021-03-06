/**
 * @file
 * @author Max Godefroy
 * @date 14/03/2021.
 */

#pragma once


#include <kryne-engine/Textures/Texture.h>


class Framebuffer {


public:

    Framebuffer(uint16_t width, uint16_t height) : width(width), height(height) {}

    /**
     * @brief Changes the size of this framebuffer.
     *
     * @param size The new size for this framebuffer.
     */
    void setSize(const ivec2 &size)
    {
        this->width = size.x;
        this->height = size.y;
        this->sizeUpdated();
    }

    /// @brief Retrieves the framebuffer size
    [[nodiscard]] ivec2 getSize() const { return ivec2(this->width, this->height); }

    virtual ~Framebuffer() = default;

protected:

    /**
     * @brief Size update behaviour.
     */
    virtual void sizeUpdated() = 0;

protected:

    uint16_t width;

    uint16_t height;


protected:

    friend class OpenGLRenderer;

    /**
     * @brief Sets the framebuffer as the current render target
     */
    virtual void setAsRenderTarget() = 0;


public:

    virtual void addColorAttachment() = 0;

    virtual void setUpDepthLayer() = 0;

    virtual shared_ptr<Texture> retrieveColor(uint8_t index) = 0;

    virtual shared_ptr<Texture> retrieveDepth() = 0;

};
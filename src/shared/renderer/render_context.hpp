#pragma once

#include <cinttypes>

#include "renderer/renderable.hpp"

namespace Renderer
{
    class IRenderContext
    {
    public:
        virtual ~IRenderContext();
        virtual bool Init() = 0;
        virtual void ResizeScreen(uint32_t const width, uint32_t const height) = 0;
        virtual void PreRender() = 0;
        virtual void RenderFrame() = 0;
        virtual void SubmitRenderable(IRenderable const * renderable) = 0;
    };
}

#pragma once

#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Renderer
{
    class IRenderable
    {
    public:
        virtual ~IRenderable();

        virtual std::vector<glm::vec3> const & GetVerts() const = 0;
        virtual std::vector<uint32_t> const & GetIndices() const = 0;
        virtual std::vector<glm::vec4> const & GetVertColours() const = 0;

        virtual glm::mat4 const & GetTransformMatrix() const = 0;
    };
}

#pragma once

#include "renderer/renderable.hpp"

namespace Renderer
{
    class CTriangle2d : public IRenderable
    {
    public:
        CTriangle2d();

        virtual std::vector<glm::vec3> const & GetVerts() const override;
        virtual std::vector<uint32_t> const & GetIndices() const override;
        virtual std::vector<glm::vec4> const & GetVertColours() const override;

        virtual glm::mat4 const & GetTransformMatrix() const override;

    private:
        std::vector<glm::vec3> m_verts;
        std::vector<uint32_t> m_indices;
        std::vector<glm::vec4> m_cols;
        glm::mat4 m_transform_mat;
    };
}

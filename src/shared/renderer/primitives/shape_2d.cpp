#include "shape_2d.hpp"

#include <utility>

Renderer::CTriangle2d::CTriangle2d()
: m_verts()
, m_indices()
, m_cols()
, m_transform_mat(1.0f)
{   
    m_verts.emplace_back(glm::vec3(-0.5f, -0.5f, 0.0f));
    m_verts.emplace_back(glm::vec3( 0.5f, -0.5f, 0.0f));
    m_verts.emplace_back(glm::vec3( 0.0f,  0.5f, 0.0f));

    m_indices.emplace_back(0);
    m_indices.emplace_back(1);
    m_indices.emplace_back(2);

    m_cols.emplace_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_cols.emplace_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_cols.emplace_back(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
}

std::vector<glm::vec3> const & Renderer::CTriangle2d::GetVerts() const
{
    return m_verts;
}

std::vector<uint32_t> const & Renderer::CTriangle2d::GetIndices() const
{
    return m_indices;
}

std::vector<glm::vec4> const & Renderer::CTriangle2d::GetVertColours() const
{
    return m_cols;
}

glm::mat4 const & Renderer::CTriangle2d::GetTransformMatrix() const
{
    return m_transform_mat;
}

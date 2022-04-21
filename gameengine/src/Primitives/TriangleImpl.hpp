#pragma once

#include "gameengine/Primitives/ITriangle.hpp"
#include "gameengine/Shader.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/IUtility.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/Math/Primitives/Triangle3D.hpp"

NAMESPACE_BEGIN( LOGLW )

class TriangleImpl final:
    public ITriangle,
    public IUtilityUser
{
public:
    TriangleData m_values;

    TriangleImpl();

    ~TriangleImpl();

protected:
    void setValues( const TriangleData& values ) override;
    void addShader( const CUL::FS::Path& filePath, IShaderFactory* sf ) override;
    const std::vector<float> getVertices() const override;
    void setColor( const TriangleColors& colors ) override;
    void setColor( const ColorS& color ) override;

private:
    void render() override;

    CUL::MATH::Primitives::Triangle3D m_triangle;
    std::array<ColorS, 3> m_colors;

// Deleted:
    TriangleImpl( const TriangleImpl& args ) = delete;
    TriangleImpl( TriangleImpl&& args ) = delete;
    TriangleImpl& operator=( const TriangleImpl& rhv ) = delete;
    TriangleImpl& operator=( TriangleImpl&& rhv ) = delete;
};

NAMESPACE_END( LOGLW )
#pragma once

#include <gameengine/IObject.hpp>
#include <CUL/GenericUtils/NonCopyable.hpp>
#include <CUL/STL_IMPORTS/STD_vector.hpp>
#include <CUL/STL_IMPORTS/STD_cstdint.hpp>

namespace LOGLW
{
class CSphere final: public IObject
{
public:
    GAME_ENGINE_API CSphere( IObject* parent );
    GAME_ENGINE_API void init( std::int32_t inStackCount, std::int32_t inSectorCount, float inRadius );
    GAME_ENGINE_API ~CSphere();

    CUL_NONCOPYABLE( CSphere )
protected:
private:
    void render() override;
    void fillIndices();
    void fillVerticesNormalsTex();

    float m_radius{ 0.f };
    std::int32_t m_stackCount{ 0 };
    std::int32_t m_sectorCount{ 0 };

    std::vector<std::int32_t> m_indices;
    std::vector<std::int32_t> m_lineIndices;

    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<float> m_texCoords;
};
}  // namespace LOGLW
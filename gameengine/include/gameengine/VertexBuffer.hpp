#pragma once

#include "gameengine/IRenderable.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IndexBuffer.hpp"
#include "CUL/GenericUtils/DumbPtr.hpp"

#include "CUL/STL_IMPORTS/STD_cstdint.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_atomic.hpp"

/*
A Vertex Buffer Object (VBO) is the common term for
a normal Buffer Object when it is used as a
source for vertex array data.
It is no different from any other buffer object,
and a buffer object used for Transform Feedback
or asynchronous pixel transfers can be used as
source values for vertex arrays.
*/

/*
A Vertex Buffer Object (VBO) is a memory buffer
in the high speed memory of your video card designed
to hold information about vertices.
In our example we have two VBOs, one that
describes the coordinates of our vertices and
another that describes the color
associated with each vertex.
VBOs can also store information such as normals,
texcoords, indicies, etc.
*/

// Vertex Buffer Objects (VBOs) store actual data.

// Just data?

NAMESPACE_BEGIN( LOGLW )

using FloatData = std::vector<float>;
using BuffIDType = uint8_t;
template<typename Type>
using Ptr = CUL::GUTILS::DumbPtr<Type>;

struct GAME_ENGINE_API VertexBufferData
{
    class VertexArray* vao = nullptr;
    std::vector<float> vertices;
    std::vector<unsigned> indices;
    bool containsColorData = false;
    LOGLW::PrimitiveType primitiveType = LOGLW::PrimitiveType::NONE;
    bool containsTextureCoords = false;
};

class GAME_ENGINE_API VertexBuffer final : public IUtilityUser,
                                                public IRenderable
{
public:
    VertexBuffer( VertexBufferData& VertexData, IGameEngine* engine );
    void render() override;
    unsigned getId()const;
    int getSize() const;
    void bind();
    ~VertexBuffer();

protected:
private:
    void loadData();
    void release();

    unsigned m_vboId = 0;
    VertexBufferData m_vertexData;
    Ptr<IndexBuffer> m_indexBuffer;
    std::atomic<bool> m_load = true;

    VertexBuffer( const VertexBuffer& value ) = delete;
    VertexBuffer( VertexBuffer&& value ) = delete;
    VertexBuffer& operator=( const VertexBuffer& value ) = delete;
    VertexBuffer& operator=( VertexBuffer&& value ) = delete;
};

NAMESPACE_END( LOGLW )
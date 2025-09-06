#pragma once

#include "gameengine/IRenderable.hpp"
#include "gameengine/IUtilityUser.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/IndexBuffer.hpp"
#include "gameengine/AttributeMeta.hpp"

#include "CUL/GenericUtils/DumbPtr.hpp"
#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/Task/TaskAccumulator.hpp"

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
texcoords, indices, etc.
*/

// Vertex Buffer Objects (VBOs) store actual data.

// Just data?

NAMESPACE_BEGIN( LOGLW )

using FloatData = std::vector<float>;
using BuffIDType = std::uint32_t;
template<typename Type>
using Ptr = CUL::GUTILS::DumbPtr<Type>;

class GAME_ENGINE_API VertexBuffer final:
    public IUtilityUser,
    public CUL::IName
{
public:
    VertexBuffer( const VertexData& vertexData );
    void setVertexData( const VertexData& vertexData );
    void updateVertexData( const VertexData& vertexData );
    void updateVertexData( bool isRenderThread );
    void render();
    unsigned getId() const;
    int getSize() const;
    void bind();
    const VertexData& getData() const;
    VertexData& getData();
    ~VertexBuffer();

    VertexBuffer( const VertexBuffer& value ) = delete;
    VertexBuffer( VertexBuffer&& value ) = delete;
    VertexBuffer& operator=( const VertexBuffer& value ) = delete;
    VertexBuffer& operator=( VertexBuffer&& value ) = delete;

protected:
    void onNameChange( const CUL::String& newName ) override;

private:
    void init();
    void createVboBuffer();
    void loadData();
    void release();

    CUL::CTaskAccumulator m_bufferTasks;
    VertexData m_vertexData;
    std::unique_ptr<IndexBuffer> m_indexBuffer;
    std::atomic<bool> m_load = true;
};

NAMESPACE_END( LOGLW )

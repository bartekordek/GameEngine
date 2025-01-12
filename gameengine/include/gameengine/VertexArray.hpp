#pragma once

#include "gameengine/IRenderable.hpp"
#include "gameengine/VertexBuffer.hpp"
#include "gameengine/AttributeMeta.hpp"

#include "CUL/IName.hpp"
#include "CUL/IRegisteredObject.hpp"
#include "CUL/IName.hpp"

#include "CUL/STL_IMPORTS/STD_cstdint.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"
#include "CUL/STL_IMPORTS/STD_mutex.hpp"
#include "CUL/STL_IMPORTS/STD_queue.hpp"

/*
A Vertex Array Object (VAO) is an OpenGL Object
that stores all of the state needed to supply
vertex data (with one minor exception noted below).
It stores the format of the vertex data as well as
the Buffer Objects (see below) providing the vertex data arrays.
Note that VAOs do not copy,
freeze or store the contents of the referenced buffers - if you
change any of the data in the buffers referenced by an existing VAO,
those changes will be seen by users of the VAO.

A Vertex Array Object (VAO) is an object which contains
one or more Vertex Buffer Objects and is designed to
store the information for a complete rendered object.
In our example this is a diamond consisting of four
vertices as well as a color for each vertex.
*/

// Vertex Array Objects (VAOs) are conceptually nothing but thin state wrappers.
// VAO - attribs?

NAMESPACE_BEGIN( LOGLW )

class IGameEngine;
class Program;
class ShaderProgram;

using BuffIDType = std::uint32_t;

class GAME_ENGINE_API VertexArray final:
    public IUtilityUser,
    public IRenderable,
    public CUL::IName,
    public CUL::IRegisterdObject
{
public:
    explicit VertexArray( IGameEngine& engine );

    VertexArray( const VertexArray& value ) = delete;
    VertexArray( VertexArray&& value ) = delete;
    VertexArray& operator=( const VertexArray& value ) = delete;
    VertexArray& operator=( VertexArray&& value ) = delete;

    BuffIDType getId() const;
    void addVertexBuffer( VertexData& data );
    void createShader( const CUL::FS::Path& path );
    ShaderProgram* getProgram();
    void setProgram( ShaderProgram* inProgram );
    void render() override;
    VertexBuffer* getVertexBuffer( std::size_t inIndex );
    void updateVertexData( std::size_t inIndex );

    void bind();
    void unbind();
    void release();

    void setName( const CUL::String& name ) override;

    ~VertexArray();
protected:
private:
    enum class TaskType : short
    {
        NONE = 0,
        CREATE_VAO,
        ADD_VBO,
        CREATE_PROGRAM,
        ADD_SHADER,
        RENDER
    };

    bool taskIsAlreadyPlaced( TaskType tt ) const;

    void runTasks();
    void registerTask( TaskType taskType );

    std::mutex m_vbosMtx;


    void createVAO();
    void createVBOs( VertexData& data );

    std::uint32_t m_vaoId = 0;

    std::mutex m_tasksMtx;
    std::deque<TaskType> m_preRenderTasks;

    ShaderProgram* m_shaderProgram = nullptr;
    std::vector<std::unique_ptr<ShaderProgram>> m_shaders;
    std::mutex m_shadersMtx;
    std::queue<CUL::FS::Path> m_shadersPaths;

    std::vector<std::unique_ptr<VertexBuffer>> m_vbos;

    std::vector<Ptr<IndexBuffer>> m_indexBuffers;

    std::vector < std::vector<unsigned>> m_indicesToPrepare;
};

NAMESPACE_END( LOGLW )

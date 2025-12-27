#include "gameengine/VertexArray.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "RunOnRenderThread.hpp"
#include "CUL/Proifling/Profiler.hpp"
#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Threading/ThreadUtil.hpp"
#include "CUL/STL_IMPORTS/STD_vector.hpp"

using namespace LOGLW;

std::vector<VertexArray*> g_vertexList;
std::mutex g_vertexListMtx;

void addVertex( VertexArray* inVertex )
{
    std::lock_guard<std::mutex> locker( g_vertexListMtx );
    g_vertexList.push_back( inVertex );
}

void removeVertex( VertexArray* inVertex )
{
    std::lock_guard<std::mutex> locker( g_vertexListMtx );
    const auto it = std::find_if( g_vertexList.begin(), g_vertexList.end(),
                                  [inVertex]( VertexArray* current )
                                  {
                                      return inVertex == current;
                                  } );

    if( it != g_vertexList.end() )
    {
        g_vertexList.erase( it );
    }
}

VertexArray::VertexArray():
    IRenderable( false )
{
    m_vertexData = std::make_unique<VertexData>();

    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            createVAO();
        } );
    addVertex( this );
}

void VertexArray::onNameChange( const String& newName )
{
    CUL::IName::onNameChange( newName );

    RunOnRenderThread::getInstance().RunWaitForResult(
        [this, &newName]()
        {
            getDevice()->setObjectName( EObjectType::VERTEX_ARRAY, m_vaoId, newName );
            std::uint8_t index = 0u;
            for( auto& vbo : m_vbos )
            {
                const char* nameStr = *getName();

                constexpr std::size_t bufferSize{ 1024u };
                char buffer[bufferSize];
                snprintf( buffer, bufferSize, "%s/vbo%d", nameStr, index );

                vbo->setName( buffer );
                ++index;
            }
        } );
}

BuffIDType VertexArray::getId() const
{
    return m_vaoId;
}

ShaderProgram* VertexArray::getProgram()
{
    return m_shaderProgram;
}

void VertexArray::setProgram( ShaderProgram* inProgram )
{
    m_shaderProgram = inProgram;
}

void VertexArray::createShader( const CUL::FS::Path& path )
{
    CUL::Assert::simple( path.exists(), "File does not exist: " + path.getPath() );

    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        if( !m_shaderProgram )
        {
            registerTask( TaskType::CREATE_PROGRAM );
        }
        m_shadersPaths.push( path );
        registerTask( TaskType::ADD_SHADER );
        runTasks();
    }
    else
    {
        std::lock_guard<std::mutex> guard( m_shadersMtx );
        if( !m_shaderProgram )
        {
            registerTask( TaskType::CREATE_PROGRAM );
        }
        m_shadersPaths.push( path );
        registerTask( TaskType::ADD_SHADER );
    }
}

void VertexArray::addVertexBuffer( const VertexData& data )
{
    createVBOs( data );
}

void VertexArray::updateVertexBuffer( const VertexData& data )
{
    m_vertexData->Attributes = data.Attributes;
    m_vertexData->Data = data.Data;
    m_vertexData->Indices = data.Indices;
    m_vertexData->primitiveType = data.primitiveType;
    m_vertexData->VAO = m_vaoId;

    if( m_vbos.empty() )
    {
        createVBOs( *m_vertexData );
    }
    else
    {
        m_vbos[0]->setVertexData( *m_vertexData );
    }
}

void VertexArray::render()
{
    ProfilerScope( "VertexArray::render" );

    runTasks();
    const size_t vbosCount = (size_t)m_vbos.size();

    bind();

    for( size_t i = 0; i < vbosCount; ++i )
    {
        m_vbos[i]->bind();
    }

    CUL::Assert::check( m_shaderProgram != nullptr, "There is no shader matching this vbo." );

    if( m_shaderProgram->getIsLinked() == false )
    {
        return;
    }

    m_shaderProgram->enable();

    for( size_t i = 0; i < vbosCount; ++i )
    {
        m_vbos[i]->render();
    }

    if( m_unbindBuffersAfterDraw )
    {
        if( m_shaderProgram )
        {
            m_shaderProgram->disable();
        }
        unbind();
    }
}

VertexBuffer* VertexArray::getVertexBuffer( std::size_t inIndex )
{
    return m_vbos[inIndex].get();
}

void VertexArray::updateVertexData( std::size_t inIndex )
{
    m_vbos[inIndex]->updateVertexData( false );
}

bool VertexArray::taskIsAlreadyPlaced( TaskType tt ) const
{
    return std::find( m_preRenderTasks.begin(), m_preRenderTasks.end(), tt ) != m_preRenderTasks.end();
}

void VertexArray::runTasks()
{
    std::lock_guard<std::mutex> tasksGuard( m_tasksMtx );
    while( !m_preRenderTasks.empty() )
    {
        auto task = m_preRenderTasks.front();
        if( task == TaskType::CREATE_VAO )
        {
            createVAO();
        }
        else if( task == TaskType::CREATE_PROGRAM )
        {
            if( !m_shaderProgram )
            {
                m_shaderProgram = getEngine().createProgram();
            }
        }
        else if( task == TaskType::ADD_SHADER )
        {
            if( !m_shaderProgram )
            {
                if( !taskIsAlreadyPlaced( TaskType::CREATE_PROGRAM ) )
                {
                    m_preRenderTasks.push_back( TaskType::CREATE_PROGRAM );
                }
                m_preRenderTasks.push_back( TaskType::ADD_SHADER );
            }
            else
            {
                std::lock_guard<std::mutex> shadersGuard( m_shadersMtx );
                while( !m_shadersPaths.empty() )
                {
                    auto shaderPath = m_shadersPaths.front();

                    auto shaderFile = CUL::CULInterface::getInstance()->getFF()->createFileFromPath( shaderPath );
                    shaderFile->load( true, true );
                    // auto shader = new Shader( *getEngine(), shaderFile );
                    throw std::logic_error( "Method not implemented" );
                    // m_shaderProgram->attachShader( shader );

                    m_shadersPaths.pop();
                }
                // m_shaderProgram->link();
                throw std::logic_error( "Method not implemented" );
            }
        }

        m_preRenderTasks.pop_front();
    }
}

void VertexArray::registerTask( TaskType taskType )
{
    std::lock_guard<std::mutex> guard( m_tasksMtx );
    if( taskType == TaskType::CREATE_PROGRAM )
    {
        if( taskIsAlreadyPlaced( taskType ) )
        {
            return;
        }
    }
    m_preRenderTasks.push_back( taskType );
}

void VertexArray::createVBOs( const VertexData& data )
{
    auto vbo = new VertexBuffer( data );
    m_vbos.emplace_back( vbo );
}

void VertexArray::createVAO()
{
    CUL::Assert::check( m_vaoId == 0u, "VAO ALREADY CREATED!" );
    m_vaoId = IUtilityUser::getDevice()->generateBuffer( BufferTypes::VERTEX_ARRAY );
}

void VertexArray::bind()
{
    getDevice()->bindBuffer( LOGLW::BufferTypes::VERTEX_ARRAY, m_vaoId );
    m_bufferTasks.executeAll();
}

void VertexArray::unbind()
{
    getDevice()->bindBuffer( LOGLW::BufferTypes::VERTEX_ARRAY, 0 );
}

void VertexArray::setName( const CUL::String& name )
{
    CUL::IName::setName( name );
    std::uint16_t id{ 0u };
    for( std::unique_ptr<VertexBuffer>& vbo : m_vbos )
    {
        constexpr std::size_t bufferSize{ 512u };
        char buffer[bufferSize];
        snprintf( buffer, bufferSize, "%s/vbo%d", *getName(), id++ );
        vbo->setName( buffer );
    }
}

VertexArray::~VertexArray()
{
    if( CUL::CULInterface::getInstance()->getThreadUtils().getIsCurrentThreadNameEqualTo( "RenderThread" ) )
    {
        release();
    }
    else
    {
        getEngine().addPreRenderTask(
            [this]()
            {
                release();
            } );
    }

    removeVertex( this );
}

void VertexArray::release()
{
    getDevice()->deleteBuffer( LOGLW::BufferTypes::VERTEX_ARRAY, m_vaoId );
}

#include "gameengine/VertexArray.hpp"
#include "gameengine/IGameEngine.hpp"
#include "gameengine/IRenderDevice.hpp"
#include "gameengine/Shaders/ShaderProgram.hpp"
#include "RunOnRenderThread.hpp"
#include "CUL/IMPORT_tracy.hpp"
#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Threading/ThreadUtil.hpp"

using namespace LOGLW;

VertexArray::VertexArray( IGameEngine& engine ) : IRenderable( &engine )
{
    RunOnRenderThread::getInstance().Run(
        [this]()
        {
            createVAO();
        } );

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::VERTEX_ARRAY, m_vaoId, newName );
                std::uint8_t index = 0u;
                for( auto& vbo : m_vbos )
                {
                    vbo->setName( getName() + "::vertex_buffer" + CUL::String(index) );
                    ++index;
                }
            } );
    };
    setName( "vertex_array_" + CUL::String( getId() ) );
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

    IName::AfterNameChangeCallback = [this]( const CUL::String& newName )
    {
        RunOnRenderThread::getInstance().Run(
            [this, newName]()
            {
                getDevice()->setObjectName( EObjectType::BUFFER, m_vaoId, newName );
            } );
    };
}

void VertexArray::addVertexBuffer( VertexData& data )
{
    createVBOs( data );
}

void VertexArray::render()
{
    ZoneScoped;
    runTasks();

    bind();
    if( m_shaderProgram )
    {
        m_shaderProgram->enable();
    }

    const size_t vbosCount = (size_t)m_vbos.size();
    for( size_t i = 0; i < vbosCount; ++i )
    {
        m_vbos[i]->render();
    }
    if( m_shaderProgram )
    {
        m_shaderProgram->disable();
    }
    unbind();
}

VertexBuffer* VertexArray::getVertexBuffer( std::size_t inIndex )
{
    return m_vbos[inIndex].get();
}

void VertexArray::updateVertexData( std::size_t inIndex )
{
    m_vbos[inIndex]->updateVertexData();
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
                m_shaderProgram = getEngine()->createProgram();
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
                    //auto shader = new Shader( *getEngine(), shaderFile );
                    throw std::logic_error( "Method not implemented" );
                    //m_shaderProgram->attachShader( shader );

                    m_shadersPaths.pop();
                }
                //m_shaderProgram->link();
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

void VertexArray::createVBOs( VertexData& data )
{
    bind();
    data.VAO = getId();
    auto vbo = new VertexBuffer( data, getEngine() );
    vbo->setDisableRenderOnMyOwn( true );
    m_vbos.emplace_back( vbo );
}

void VertexArray::createVAO()
{
    m_vaoId = IUtilityUser::getDevice()->generateBuffer( BufferTypes::VERTEX_ARRAY );
}

void VertexArray::bind()
{
    getDevice()->bindBuffer( LOGLW::BufferTypes::VERTEX_ARRAY, m_vaoId );
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
        vbo->setName( getName() + "::vbo::" + CUL::String( id++ ) );
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
        getEngine()->addPreRenderTask(
            [this]()
            {
            release();
        } );
    }
}

void VertexArray::release()
{
    getEngine()->removeObjectToRender( this );
    getDevice()->deleteBuffer( LOGLW::BufferTypes::VERTEX_ARRAY, m_vaoId );
}

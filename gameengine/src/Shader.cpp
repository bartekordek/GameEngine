#include "gameengine/Shader.hpp"

using namespace LOGLW;

Shader::Shader( CUL::FS::IFile* file ):
    m_shaderCode( file )
{
    create();
}

unsigned int Shader::getId() const
{
    return m_id;
}

void Shader::useShader() const
{

}

void Shader::reload()
{
    m_shaderCode->reload();
    release();
    create();
}

void Shader::create()
{
    m_id = getUtility()->createShader( *m_shaderCode );
}

const CUL::FS::Path& Shader::getPath() const
{
    return m_shaderCode->getPath();
}

Shader::~Shader()
{
    release();
}

void Shader::release()
{
    if( m_id )
    {
        getUtility()->removeShader( m_id );
        m_id = 0;
    }
}
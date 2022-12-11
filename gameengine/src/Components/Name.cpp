#include "gameengine/Components/Name.hpp"
#include "gameengine/IObject.hpp"

using namespace LOGLW;

Name::Name( IObject& owner ) : m_owner( owner )
{
    //IObject* parent = m_owner.getParent();
    //if( parent )
    //{
    //    Name* parentName = parent->getnameCmp();
    //    if( parentName )
    //    {
    //        m_rootPath = parentName->getFullName() + String( "::" ) + m_rootPath;
    //    }
    //}

    //setName("");
}

const String& Name::getName() const
{
    return m_name;
}

const String& Name::getFullName() const
{
    return m_fullName;
}

void Name::setName( const String& name )
{
    if( name.empty() )
    {
        m_name = typeid( m_owner ).name();
    }
    else
    {
        m_name = typeid( m_owner ).name();
        m_name = m_name + "::" + name;
    }

    m_name.replace( "class ", "" );
    if( m_rootPath.empty() )
    {
        m_fullName = m_name;
    }
    else
    {
        m_fullName = m_rootPath + "::" + m_name;
    }
}
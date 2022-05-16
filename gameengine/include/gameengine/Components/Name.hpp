#pragma once

#include "gameengine/Components/IComponent.hpp"

#include "CUL/String.hpp"

NAMESPACE_BEGIN( LOGLW )

class IObject;
using String = CUL::String;

class GAME_ENGINE_API Name final: public IComponent
{
public:
    Name( IObject& owner );

    const String& getName() const;
    const String& getFullName() const;

    void setName( const String& name );

protected:
private:
    IObject& m_owner;
    String m_name;
    String m_rootPath;
    String m_fullName;
};

NAMESPACE_END( LOGLW )
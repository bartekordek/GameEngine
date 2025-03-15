#include "gameengine/Shaders/ShaderUnit.hpp"
#include "CUL/Filesystem/IFile.hpp"

namespace LOGLW
{

ShaderUnit::ShaderUnit( ShaderUnit&& arg ):
    File( std::move( arg.File ) ),
    Type( arg.Type ),
    ID( arg.ID ),
    State(arg.State)
{
    arg.Type = CShaderTypes::ShaderType::Unkown;
    arg.ID = 0u;
    arg.State = EShaderUnitState::Unkown;
}

ShaderUnit& ShaderUnit::operator=( ShaderUnit&& arg )
{
    if( this != &arg )
    {
        File = std::move( arg.File );
        Type = arg.Type;
        ID = arg.ID;
        State = arg.State;

        arg.Type = CShaderTypes::ShaderType::Unkown;
        arg.ID = 0u;
        arg.State = EShaderUnitState::Unkown;
    }
    return *this;
}

}

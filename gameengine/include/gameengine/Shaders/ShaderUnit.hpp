#pragma once

#include "gameengine/Shaders/ShaderTypes.hpp"

#include "CUL/STL_IMPORTS/STD_memory.hpp"

NAMESPACE_BEGIN( CUL )
NAMESPACE_BEGIN( FS )
class IFile;
NAMESPACE_END( FS )
NAMESPACE_END( CUL )

NAMESPACE_BEGIN( LOGLW )

enum class EShaderUnitState : std::int8_t
{
    Unkown = 1,
    Loaded,
    Unloaded,
    Error
};

#if defined( _MSC_VER )
#pragma warning( push, 0 )
#pragma warning( disable : 4625 )
#pragma warning( disable : 4626 )
#endif  // defined( _MSC_VER )

struct GAME_ENGINE_API ShaderUnit final
{
    std::unique_ptr<CUL::FS::IFile> File;
    CShaderTypes::ShaderType Type{ CShaderTypes::ShaderType::Unkown };
    std::uint32_t ID = 0u;
    EShaderUnitState State{ EShaderUnitState::Unkown };

    ShaderUnit() = default;
    ShaderUnit( const ShaderUnit& arg ) = delete;
    ShaderUnit( ShaderUnit&& arg );
    ShaderUnit& operator=( const ShaderUnit& arg ) = delete;
    ShaderUnit& operator=( ShaderUnit&& arg );

    ~ShaderUnit() = default;
};

#if defined( _MSC_VER )
#pragma warning( pop )
#endif  // #if defined( _MSC_VER )

NAMESPACE_END( LOGLW )

#include "ShaderEditor.hpp"

#include "CUL/GenericUtils/ConsoleUtilities.hpp"

int main( int argc, char** argv )
{
    CUL::GUTILS::ConsoleUtilities cu;
    cu.setArgs( argc, argv );

    LOGLW::WinSize size;
    size.W = 1920;
    size.H = 1080;

    LOGLW::WinPos pos;
    pos.X = 256;
    pos.Y = 32;

    auto valW = cu.getFlagValue( "-w" ).string();
    if( !valW.empty() )
    {
        size.W = std::stoi( valW );
    }

    auto valH = cu.getFlagValue( "-h" ).string();
    if( !valH.empty() )
    {
        size.H = std::stoi( valH );
    }

    auto valX = cu.getFlagValue( "-x" ).string();
    if( !valX.empty() )
    {
        pos.X = std::stoi( valX );
    }

    auto valY = cu.getFlagValue( "-y" ).string();
    if( !valY.empty() )
    {
        pos.Y = std::stoi( valY );
    }

    ShaderEditor editor( size, pos );
    editor.run();

    return 0;
}

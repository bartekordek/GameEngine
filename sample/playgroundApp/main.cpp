#include "Playground.hpp"
#include "CUL/GenericUtils/ConsoleUtilities.hpp"

int main( int argc, char** argv )
{
    CUL::GUTILS::ConsoleUtilities cu;
    cu.setArgs( argc, argv );

    LOGLW::WinData wd;
    wd.Pos = { .X = 256, .Y = 256 };
    wd.WindowRes = { .W = 1920u, .H = 1080u };

    auto valW = cu.getFlagValue( "-w" ).string();
    if( !valW.empty() )
    {
        wd.WindowRes.W = std::stoi( valW );
    }

    auto valH = cu.getFlagValue( "-h" ).string();
    if( !valH.empty() )
    {
        wd.WindowRes.H = std::stoi( valH );
    }

    auto valX = cu.getFlagValue( "-x" ).string();
    if( !valX.empty() )
    {
        wd.Pos.X = std::stoi( valX );
    }

    auto valY = cu.getFlagValue( "-y" ).string();
    if( !valY.empty() )
    {
        wd.Pos.Y = std::stoi( valY );
    }

    wd.CurrentRes = wd.WindowRes;

    Playground playground( wd );
    playground.run();

    return 0;
}
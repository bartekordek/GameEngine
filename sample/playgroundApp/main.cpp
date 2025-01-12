#include "Playground.hpp"
#include "CUL/GenericUtils/ConsoleUtilities.hpp"

int main( int argc, char** argv )
{
    CUL::GUTILS::ConsoleUtilities cu;
    cu.setArgs( argc, argv );

    std::int16_t width = 1920;
    std::int16_t height = 1080;
    std::int16_t x{ 256 };
    std::int16_t y{ 256 };

    auto valW = cu.getFlagValue( "-w" ).string();
    if( !valW.empty() )
    {
        width = std::stoi( valW );
    }

    auto valH = cu.getFlagValue( "-h" ).string();
    if( !valH.empty() )
    {
        height = std::stoi( valH );
    }

    auto valX = cu.getFlagValue( "-x" ).string();
    if( !valX.empty() )
    {
        x = std::stoi( valX );
    }

    auto valY = cu.getFlagValue( "-y" ).string();
    if( !valY.empty() )
    {
        y = std::stoi( valY );
    }

    Playground playground( width, height, x, y );
    playground.run();

    return 0;
}
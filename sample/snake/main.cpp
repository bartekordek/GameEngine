#include "Game.hpp"

#include "CUL/GenericUtils/ConsoleUtilities.hpp"

int main( int argc, char** argv )
{
    CUL::GUTILS::ConsoleUtilities cu;
    cu.setArgs( argc, argv );

    std::uint16_t width = 1280u;
    std::uint16_t height = 800u;

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

    std::int32_t size = 10;
    LOGLW::WinPos winPos{.X = 64u, .Y = 64u};
    LOGLW::WinSize winSize{ .W = width, .H = height };
    Game snakeGame( size, size, winPos, winSize );
    snakeGame.run();

    return 0;
}
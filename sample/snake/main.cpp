#include "Game.hpp"

#include "CUL/GenericUtils/ConsoleUtilities.hpp"

int main( int argc, char** argv )
{
    CUL::GUTILS::ConsoleUtilities cu;
    cu.setArgs( argc, argv );

    auto width = 1280;
    auto height = 800;

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

    int size = 10;
    Game snakeGame( size, size, { 64, 64 }, { width, height } );
    snakeGame.run();

    return 0;
}
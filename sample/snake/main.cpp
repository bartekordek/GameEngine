#include "CUL/GenericUtils/ConsoleUtilities.hpp"
#include "Game.hpp"

int main( int argc, char** argv )
{
    auto& cu = CUL::GUTILS::ConsoleUtilities::getInstance();
    cu.setArgs( argc, argv );

    auto width = cu.getDoesFlagExist( "-w" ) ? cu.getFlagValue( "-w" )
                                             : CUL::String( CUL_STR( "1280" ) );
    auto height = cu.getDoesFlagExist( "-h" ) ? cu.getFlagValue( "-h" )
                                              : CUL::String( CUL_STR( "800" ) );

    std::int32_t size = 10;
    LOGLW::WinPos winPos{ .X = 64u, .Y = 64u };
    LOGLW::WinSize winSize{ .W = static_cast<std::uint16_t>( width.toInt() ),
                            .H = static_cast<std::uint16_t>( height.toInt() ) };
    Game snakeGame( size, size, winPos, winSize );
    snakeGame.run();

    return 0;
}
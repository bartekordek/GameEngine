#include "Playground.hpp"
#include "CUL/GenericUtils/ConsoleUtilities.hpp"

int main( int argc, char** argv )
{
    auto& cu = CUL::GUTILS::ConsoleUtilities::getInstance();
    cu.setArgs( argc, argv );

    auto width = cu.getFlagValue( "-w", CUL_STR( "1280" ) );
    auto height = cu.getFlagValue( "-h", CUL_STR( "800" ) );

    auto x = cu.getFlagValue( "-X", CUL_STR( "128" ) );
    auto y = cu.getFlagValue( "-y", CUL_STR( "128" ) );

    LOGLW::WinData wd;
    wd.WindowRes.W = static_cast<std::uint16_t>( width.toInt() );
    wd.WindowRes.H = static_cast<std::uint16_t>( height.toInt() );
    wd.Pos.X = x.toInt();
    wd.Pos.Y = y.toInt();

    wd.CurrentRes = wd.WindowRes;

    Playground playground( wd );
    playground.run();

    return 0;
}
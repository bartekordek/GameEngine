#include "Playground.hpp"
#include "CUL/GenericUtils/ConsoleUtilities.hpp"

int main( int argc, char** argv )
{
    auto& cu = CUL::GUTILS::ConsoleUtilities::getInstance();
    cu.setArgs( argc, argv );

    auto width =
        cu.getFlagValue( "-w" ) ? cu.getFlagValue( "-w" ).value() : CUL_STR( "1280" );
    auto height =
        cu.getFlagValue( "-h" ) ? cu.getFlagValue( "-h" ).value() : CUL_STR( "800" );

    auto x = cu.getFlagValue( "-x" ) ? cu.getFlagValue( "-x" ).value() : CUL_STR( "256" );
    auto y = cu.getFlagValue( "-y" ) ? cu.getFlagValue( "-y" ).value() : CUL_STR( "256" );

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
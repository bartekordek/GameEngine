#include "CUL/CULInterface.hpp"
#include "CUL/GenericUtils/CommandlineParser.hpp"
#include "CUL/Filesystem/FSApi.hpp"
#include "CUL/Filesystem/FileFactory.hpp"
#include "CUL/Filesystem/IFile.hpp"
#include "CUL/JSON/JSONFile.hpp"
#include "CUL/Filesystem/IArchive.hpp"
#include "CUL/Filesystem/Archiver.hpp"

namespace CUL
{
class Cooker
{
public:
    Cooker()
    {
    }

    std::int32_t run()
    {
        auto& cmdParser = CUL::CommandlineParser::getInstance();
        const auto jsonPath = cmdParser.getFlagValue( CUL_STR( "-jsonPath" ) );
        if( jsonPath.empty() )
        {
            // Handle the case where the JSON path is not provided
            return 1;
        }

        auto cul = CUL::CULInterface::createInstance();
        auto fs = cul->getFS();
        if( !fs->fileExist( jsonPath ) )
        {
            // Handle the case where the JSON file does not exist
            return 1;
        }

        auto ff = cul->getFF();
        CUL::FS::JSONFilePtr jsonFile = ff->createJSONFileRawPtr( jsonPath );
        JSON::INode* children =  jsonFile->getChild( "files" );

        auto cul = CUL::CULInterface::createInstance();
        CUL::SFArchiveMetadata md;
        for( const auto& child : children->getArray() )
        {
            CUL::SFile file;
            file.Path = child->getString();
            file.PathSize = file.Path.
            std::unique_ptr<CUL::FS::IFile> fileLoaded(
                cul->getFF()->createRegularFileRawPtr( file.Path ) );
            fileLoaded->load( true, true );
            const auto contentSize = fileLoaded->getSizeBytes();
            file.Content.resize( contentSize.toUint64() );
            file.Content = 
        }

        return 0;
    }

    ~Cooker()
    {
    }

    CUL_NONCOPYABLE( Cooker );

protected:
private:
};
}  // namespace CUL

int main( int argc, char* argv[] )
{
    auto& cmdParser = CUL::CommandlineParser::getInstance();
    cmdParser.passVariables( argc, argv );
    
    CUL::Cooker cooker;
    return cooker.run();
}
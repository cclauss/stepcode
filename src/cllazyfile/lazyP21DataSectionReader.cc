#include "lazyP21DataSectionReader.h"
#include "lazyInstMgr.h"
#include <assert.h>

lazyP21DataSectionReader::lazyP21DataSectionReader( lazyFileReader * parent, std::ifstream & file, std::streampos start ):
                                                    lazyDataSectionReader( parent, file, start ) {
    findSectionStart();
    findSectionEnd();
    if( _sectionEnd < 0 ) {
        _error = true;
        return;
    }
    _file.seekg( _sectionStart );
    namedLazyInstance nl;
    while( nl = nextInstance(), ( ( nl.loc.begin > 0 ) && ( nl.name != 0 ) ) ) {
//         _headerInstances.insert( instancesLoaded_pair( nl.loc.instance, getRealInstance( &nl.loc ) ) );
        parent->getInstMgr()->addLazyInstance( nl );
    }
    _file.seekg( _sectionEnd );
}
// part of readdata1
const namedLazyInstance lazyP21DataSectionReader::nextInstance() {
    namedLazyInstance i;

    //TODO detect comments

    i.loc.instance = readInstanceNumber();
    if( !_file.good() ) {
        i.loc.begin = -1;
        return i;
    }
    _file >> std::ws;
    char c = _file.get();
    if( c != '=' ) {
        char s[25];
        _file.width(24);
        _file >> s;
        std::cerr << "expected equals: " << c << s << std::endl;
    }
//     assert( c == '=' );
    _file >> std::ws;

    i.loc.begin = _file.tellg();
    i.loc.section = _sectionID;
//     i.loc.file = _fileID;
    _file >> std::ws;
    i.name = getDelimitedKeyword(";( /\\");

    if( ( !_file.good() ) || ( seekInstanceEnd() >= _sectionEnd ) ) {
        //invalid instance, so clear everything
        i.loc.begin = -1;
        delete i.name;
        i.name = 0;
    }
    return i;
}
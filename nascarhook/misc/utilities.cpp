#include <misc/utilities.h>
#include <Windows.h>
#include <algorithm>
#include <sstream>
#include <vector>

namespace cheat {
namespace utilities
{
    using patterns_t     = std::vector< uintptr_t >;
    using pattern_byte_t = std::pair< uint8_t, bool >;
}}

namespace cheat {
namespace utilities
{
    bool get_pe_headers( void* module, PIMAGE_DOS_HEADER &dosHeader, PIMAGE_NT_HEADERS &ntHeaders )
    {
        IMAGE_DOS_HEADER* dos = { };
        IMAGE_NT_HEADERS* nt  = { };

        dos = reinterpret_cast< IMAGE_DOS_HEADER* >( module );
        
        if ( !dos || dos->e_magic != IMAGE_DOS_SIGNATURE )
            return false;

        nt = reinterpret_cast< IMAGE_NT_HEADERS* >( uintptr_t( dos ) + dos->e_lfanew );

        if ( !nt || nt->Signature != IMAGE_NT_SIGNATURE )
            return false;

        dosHeader = dos;
        ntHeaders = nt;

        return true;
    }
}}

uintptr_t cheat::utilities::find_pattern( void* module, const std::string& signature )
{
    uint8_t*                      scan_start = { };
    uint8_t*                      scan_end = { };
    std::vector< pattern_byte_t > pattern = { };
    std::stringstream			  stream{ signature };
    size_t                        length = { };
    uintptr_t                     start = { };
    IMAGE_DOS_HEADER*             dos = { };
    IMAGE_NT_HEADERS*             nt  = { };
    std::string				      w = { };

    if ( !get_pe_headers( module, dos, nt ) )
        return { };

    start  = nt->OptionalHeader.ImageBase;
    length = nt->OptionalHeader.SizeOfCode;

    if ( !start || !length || signature.empty( ) )
        return { };
    
    while ( stream >> w )
    {
        if ( w[ 0 ] == '?' )
            pattern.push_back( { 0, true } );
    
        else if ( std::isxdigit( w[ 0 ] ) && std::isxdigit( w[ 1 ] ) )
            pattern.push_back( { ( uint8_t )std::strtoul( w.data( ), 0, 16 ), false } );
    }
    
    scan_start = ( uint8_t* )start;
    scan_end   = scan_start + length;
    
    auto result = std::search
    (
        scan_start, scan_end, pattern.begin( ), pattern.end( ),
        
        [ ]( const uint8_t b, const pattern_byte_t& p ) {
            return b == p.first || p.second;
        }
    );
    
    if ( result == scan_end )
        return { };
    
    return uintptr_t( result );
}
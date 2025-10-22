#include <misc/utilities.h>
#include <setup/offsets.h>
#include <setup/modules.h>

namespace cheat {
namespace offsets
{
    bool initialized = { };

    IDirect3DDevice9* shader_device = { };
}}

bool cheat::offsets::initialize( )
{
    if ( initialized )
        return false;

    if ( !( shader_device = **reinterpret_cast< IDirect3DDevice9*** >( utilities::find_pattern( modules::shaderapi_dll, "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) + 0x1 ) ) ) return false;

    initialized = true;
    return		  true;
}

void cheat::offsets::shutdown( )
{
    initialized = false;
}
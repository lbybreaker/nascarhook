#include <setup/modules.h>
#include <Windows.h>
#include <thread>
#include <chrono>

namespace cheat {
namespace modules
{
    bool initialized = { };

    void* tier0_dll     = { };
    void* client_dll    = { };
    void* engine_dll    = { };
    void* vstdlib_dll   = { };
    void* shaderapi_dll = { };
}}

bool cheat::modules::initialize( )
{
    using namespace std::chrono_literals;

    if ( initialized )
        return false;

    while ( !GetModuleHandleA( "serverbrowser.dll" ) )
        std::this_thread::sleep_for( 100ms );

    if ( !( tier0_dll     = GetModuleHandleA( "tier0.dll"        ) ) ) return false;
    if ( !( client_dll    = GetModuleHandleA( "client.dll"       ) ) ) return false;
    if ( !( engine_dll    = GetModuleHandleA( "engine.dll"       ) ) ) return false;
    if ( !( vstdlib_dll   = GetModuleHandleA( "vstdlib.dll"      ) ) ) return false;
    if ( !( shaderapi_dll = GetModuleHandleA( "shaderapidx9.dll" ) ) ) return false;

    initialized = true;
    return		  true;
}

void cheat::modules::shutdown( )
{
    vstdlib_dll = { };
    engine_dll  = { };
    client_dll  = { };
    tier0_dll   = { };
    initialized = { };
}
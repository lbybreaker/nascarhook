#include <setup/interfaces.h>
#include <setup/modules.h>
#include <setup/offsets.h>
#include <setup/netvars.h>
#include <setup/cheat.h>
#include <setup/hooks.h>
#include <stdexcept>
#include <Windows.h>

namespace cheat
{
    bool initialized = { };
}

bool cheat::initialize( )
{
    if ( initialized )
        return false;

    try
    {
        if ( !modules::initialize( ) )
            throw std::runtime_error( "Failed to get modules." );

        if ( !interfaces::initialize( ) )
            throw std::runtime_error( "Failed to get interfaces." );

        if ( !offsets::initialize( ) )
            throw std::runtime_error( "Failed to initialize offsets." );

        if ( !netvars::initialize( ) )
            throw std::runtime_error( "Failed to initialize netvars." );

        if ( !hooks::initialize( ) )
            throw std::runtime_error( "Failed to initialize hooks." );
    }

    catch ( const std::runtime_error& e )
    {
        MessageBoxA( nullptr, e.what( ), "nascarhook", MB_ICONERROR );
        return false;
    }

    initialized = true;
    return		  true;
}

void cheat::shutdown( )
{
    hooks::shutdown( );
    netvars::shutdown( );
    offsets::shutdown( );
    interfaces::shutdown( );
    modules::shutdown( );

    initialized = false;
}
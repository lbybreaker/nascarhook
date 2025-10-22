#include <misc/detours.h>

namespace cheat {
namespace memory
{
    bool c_detour_hook::add( uint64_t target, void* function )
    {
        auto& hook = hooks.emplace_back( );
    
        hook.target        = target;
        hook.hook_function = ( uint64_t )function;
        hook.hook          = std::make_unique< PLH::x86Detour >( hook.target, hook.hook_function, &hook.original );
    
        if ( !hook.hook->hook( ) )
            return false;
    
        return true;
    }
    
    void c_detour_hook::remove( void* function )
    {
        auto i = std::find_if
        (
            hooks.begin( ), hooks.end( ),
            
            [ & ]( const hook_t& hook ) {
                return ( void* )hook.hook_function == function;
            }
        );
    
        if ( !i->hook || !i->hook->isHooked( ) )
            return;
    
        i->hook->unHook( );
    }
    
    void c_detour_hook::remove_all( )
    {
        for ( const auto& i : hooks )
        {
            if ( !i.hook || !i.hook->isHooked( ) )
                continue;
    
            i.hook->unHook( );
        }
    }
}}
#ifndef DETOURS_H
#define DETOURS_H
#pragma once

#include <polyhook2/Detour/x86Detour.hpp>

namespace cheat {
namespace memory
{
	class c_detour_hook
	{
	public:
		bool add( uint64_t target, void* function );
	
		void remove( void* function );
	
		void remove_all( );
	
		template< typename t = void* >
		t get_old_method( t function )
		{
			auto i = std::find_if
			(
				hooks.begin( ), hooks.end( ),
				
				[ & ]( const hook_t& hook ) {
					return ( void* )hook.hook_function == function;
				}
			);
	
			if ( i == hooks.end( ) || !i->original )
				return t{ };
	
			return PLH::FnCast( i->original, function );
		}
	
	private:
		struct hook_t
		{
			std::unique_ptr< PLH::IHook > hook;
			uint64_t					  original;
			uint64_t					  target;
			uint64_t					  hook_function;
		};
	
		std::vector< hook_t > hooks;
	};
}}

#endif // DETOURS_H
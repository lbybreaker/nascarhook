#ifndef UTILITIES_H
#define UTILITIES_H
#pragma once

#include <stdint.h>
#include <string>

namespace cheat {
namespace utilities
{
    uintptr_t find_pattern( void* module, const std::string& signature );

    template< typename o = void*, typename i = void* >
    __forceinline o force_cast( i in )
    {
        union
        {
            i in;
            o out;
        }

        u = { in };

        return u.out;
    };

    template< typename t = void* >
    __forceinline t get_method( void* base, size_t index )
    {
        return ( t )( *( uintptr_t** )base )[ index ];
    }
}}

#endif // UTILITIES_H
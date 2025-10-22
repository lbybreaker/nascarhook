#include <setup/netvars.h>

namespace cheat {
namespace netvars
{
    bool initialized = { };
}}

bool cheat::netvars::initialize( )
{
    if ( initialized )
        return false;

    initialized = true;
    return		  true;
}

void cheat::netvars::shutdown( )
{
    initialized = false;
}
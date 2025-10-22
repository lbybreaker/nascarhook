#include <setup/interfaces.h>

namespace cheat {
namespace interfaces
{
    bool initialized = { };
}}

bool cheat::interfaces::initialize( )
{
    if ( initialized )
        return false;

    initialized = true;
    return		  true;
}

void cheat::interfaces::shutdown( )
{
    initialized = false;
}
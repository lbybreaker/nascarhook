#ifndef OFFSETS_H
#define OFFSETS_H
#pragma once

struct IDirect3DDevice9;

namespace cheat {
namespace sdk
{
    // pre-def structs/classes here
}}

namespace cheat {
namespace offsets
{
    bool initialize( );
    void shutdown( );
}}

namespace cheat {
namespace offsets
{
    extern IDirect3DDevice9* shader_device;
}}

#endif // OFFSETS_H
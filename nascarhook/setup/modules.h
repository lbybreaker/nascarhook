#ifndef MODULES_H
#define MODULES_H
#pragma once

namespace cheat {
namespace modules
{
	bool initialize( );
	void shutdown( );
}}

namespace cheat {
namespace modules
{
	extern void* tier0_dll;
	extern void* client_dll;
	extern void* engine_dll;
	extern void* vstdlib_dll;
	extern void* shaderapi_dll;
}}

#endif // MODULES_H
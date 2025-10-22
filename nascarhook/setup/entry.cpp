#include <setup/cheat.h>
#include <Windows.h>

DWORD WINAPI main_thread( LPVOID param )
{
	if ( !cheat::initialize( ) )
		FreeLibraryAndExitThread( HMODULE( param ), FALSE );

	return TRUE;
}

BOOL WINAPI DllMain( HMODULE instance, DWORD reason, LPVOID reserved )
{
	if ( reason == DLL_PROCESS_ATTACH )
	{
		HANDLE thread = { };

		thread = CreateThread( nullptr, NULL, &main_thread, instance, NULL, nullptr );

		if ( !thread )
			FreeLibrary( instance );
		else
			CloseHandle( thread );

		return TRUE;
	}

	if ( reason == DLL_PROCESS_DETACH )
	{
		return TRUE;
	}

	return TRUE;
}
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>
#include <misc/utilities.h>
#include <setup/offsets.h>
#include <misc/detours.h>
#include <setup/hooks.h>
#include <misc/render.h>
#include <d3dx9.h>

namespace cheat {
namespace hooks
{
    bool                  initialized = { };
    memory::c_detour_hook detours     = { };
}}

namespace cheat {
namespace hooks
{
    HRESULT __stdcall reset    ( IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS* present_parameters );
    HRESULT __stdcall end_scene( IDirect3DDevice9* device );
    LRESULT __stdcall wnd_proc( HWND window, UINT message, WPARAM wparam, LPARAM lparam );

    void render_menu( );

    decltype( &reset     ) o_reset     = { };
    decltype( &end_scene ) o_end_scene = { };
    WNDPROC                o_wnd_proc  = { };
}}

bool cheat::hooks::initialize( )
{
    D3DDEVICE_CREATION_PARAMETERS creation_parameters = { };

    if ( initialized )
        return false;

    if ( !render::initialize( ) )
        return false;

    if ( offsets::shader_device->GetCreationParameters( &creation_parameters ) > 0 )
        return false;

    #pragma warning( push )
    #pragma warning( disable : 6387 )
    if ( !( o_wnd_proc = ( WNDPROC )SetWindowLongA( creation_parameters.hFocusWindow, GWL_WNDPROC, utilities::force_cast< LONG_PTR >( &wnd_proc ) ) ) ) return false;
    #pragma warning( pop )

    if ( !detours.add( utilities::get_method< uint64_t >( offsets::shader_device, 16 ), utilities::force_cast( &reset     ) ) ) return false;
    if ( !detours.add( utilities::get_method< uint64_t >( offsets::shader_device, 42 ), utilities::force_cast( &end_scene ) ) ) return false;

    if ( !( o_reset     = detours.get_old_method< decltype( o_reset     ) >( &reset     ) ) ) return false;
    if ( !( o_end_scene = detours.get_old_method< decltype( o_end_scene ) >( &end_scene ) ) ) return false;

    initialized = true;
    return		  true;
}

void cheat::hooks::shutdown( )
{
    initialized = false;
}

HRESULT __stdcall cheat::hooks::reset( IDirect3DDevice9* device, const D3DPRESENT_PARAMETERS* present_parameters )
{
    HRESULT result = { };

    ImGui_ImplDX9_InvalidateDeviceObjects( );

    result = o_reset( device, present_parameters );

    if ( result <= 0 )
        ImGui_ImplDX9_CreateDeviceObjects( );

    return result;
}

void cheat::hooks::render_menu( )
{
    // watermark
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.f );
        ImGui::PushStyleColor( ImGuiCol_WindowBg, IM_COL32( 15, 75, 100, 255 ) );
        ImGui::SetNextWindowSize( ImVec2( 225.f, 70.f ) );
        ImGui::SetNextWindowPos( ImVec2( 7.f, 7.f ), ImGuiCond_Once );
        ImGui::Begin( "nascarhook_watermark", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );
        {
            ImVec2 window_pos = ImGui::GetWindowPos( );
            ImVec2 window_size = ImGui::GetWindowSize( );

            render::rectangle( e_draw_list_background, window_pos.x - 4.f, window_pos.y - 4.f, window_size.x + 8.f, window_size.y + 8.f, IM_COL32( 40,  180, 255, 255 ) ); // most outer border
            render::rectangle( e_draw_list_background, window_pos.x - 3.f, window_pos.y - 3.f, window_size.x + 6.f, window_size.y + 6.f, IM_COL32( 195, 195, 50,  255 ) ); // inner border
            render::rectangle( e_draw_list_background, window_pos.x - 2.f, window_pos.y - 2.f, window_size.x + 4.f, window_size.y + 4.f, IM_COL32( 195, 195, 50,  255 ) ); // inner border
            render::rectangle( e_draw_list_background, window_pos.x - 1.f, window_pos.y - 1.f, window_size.x + 2.f, window_size.y + 2.f, IM_COL32( 130, 130, 0,   255 ) ); // most inner border
            render::image( e_draw_list_window, e_texture_nascarhook_icon, window_pos.x + 4.f, window_pos.y + 3.f, 64.f, 64.f );

            render::formatted_text( e_draw_list_window, e_font_default, window_pos.x + 72.f, window_pos.y + 3.f, IM_COL32( 255, 255, 255, 255 ), e_font_flags_none, "nascarhook | fps: {:.0f}", ImGui::GetIO( ).Framerate / 2.f );
        }
        ImGui::End( );
        ImGui::PopStyleColor( );
        ImGui::PopStyleVar( );
        ImGui::PopStyleVar( );
    }

    // main menu code
    {
        static bool open = { };
        static float opacity = { };
        constexpr float freq = 1.0f / 0.2f;
        float step = freq * ImGui::GetIO( ).DeltaTime;

        if ( ImGui::IsKeyPressed( ImGuiKey_Insert ) )
            open = !open;

        open ? opacity += step : opacity -= step;

        opacity = std::clamp( opacity, 0.f, 1.0f );

        if ( !opacity )
            return;

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.f );
        ImGui::PushStyleVar( ImGuiStyleVar_Alpha, opacity );
        ImGui::PushStyleColor( ImGuiCol_WindowBg, IM_COL32( 15, 75, 100, 255 ) );
        ImGui::PushStyleColor( ImGuiCol_ResizeGrip, IM_COL32( 0, 0, 0, 0 ) );
        ImGui::SetNextWindowSize( ImVec2( 735.f, 470.f ), ImGuiCond_Once );
        ImGui::Begin( "nascarhook", nullptr, ImGuiWindowFlags_NoTitleBar );
        {
            ImVec2 window_pos = ImGui::GetWindowPos( );
            ImVec2 window_size = ImGui::GetWindowSize( );
            
            // border
            render::rectangle( e_draw_list_foreground, window_pos.x - 4.f, window_pos.y - 4.f, window_size.x + 8.f, window_size.y + 8.f, IM_COL32( 40,  180, 255, opacity * 255 ) ); // most outer border
            render::rectangle( e_draw_list_foreground, window_pos.x - 3.f, window_pos.y - 3.f, window_size.x + 6.f, window_size.y + 6.f, IM_COL32( 195, 195, 50, opacity * 255 ) ); // inner border
            render::rectangle( e_draw_list_foreground, window_pos.x - 2.f, window_pos.y - 2.f, window_size.x + 4.f, window_size.y + 4.f, IM_COL32( 195, 195, 50, opacity * 255 ) ); // inner border
            render::rectangle( e_draw_list_foreground, window_pos.x - 1.f, window_pos.y - 1.f, window_size.x + 2.f, window_size.y + 2.f, IM_COL32( 130, 130, 0, opacity * 255 ) ); // most inner border
            render::image( e_draw_list_window, e_texture_nascarhook_icon, window_pos.x, window_pos.y, window_size.x, window_size.y, IM_COL32( 255, 255, 255, opacity * 25 ) );
            
            ImVec2 avail = ImGui::GetContentRegionAvail( );
            ImVec2 child_size = ImVec2( avail.x, 25 );

            ImGui::BeginChild( "nascarhook_tab_selector", child_size );
            {
                ImVec2 child_pos = ImGui::GetWindowPos( );

                render::rectangle( e_draw_list_foreground, child_pos.x - 4.f, child_pos.y - 4.f, child_size.x + 8.f, child_size.y + 8.f, IM_COL32( 40,  180, 255, opacity * 255 ) ); // most outer border
                render::rectangle( e_draw_list_foreground, child_pos.x - 3.f, child_pos.y - 3.f, child_size.x + 6.f, child_size.y + 6.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 2.f, child_pos.y - 2.f, child_size.x + 4.f, child_size.y + 4.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 1.f, child_pos.y - 1.f, child_size.x + 2.f, child_size.y + 2.f, IM_COL32( 130, 130, 0,   opacity * 255 ) ); // most inner border
                render::filled_rectangle( e_draw_list_window, child_pos.x, child_pos.y, child_size.x, child_size.y, IM_COL32( 15, 65, 100, opacity * 125 ) );
            
            }
            ImGui::EndChild( );
            
            avail = ImGui::GetContentRegionAvail( );
            child_size = ImVec2( avail.x / 2, avail.y - 8.f );

            ImGui::SetCursorPosY( 45.f );
            ImGui::BeginChild( "nascarhook_content_1", child_size );
            {
                bool enabled;

                ImVec2 child_pos = ImGui::GetWindowPos( );

                render::rectangle( e_draw_list_foreground, child_pos.x - 4.f, child_pos.y - 4.f, child_size.x + 8.f, child_size.y + 8.f, IM_COL32( 40,  180, 255, opacity * 255 ) ); // most outer border
                render::rectangle( e_draw_list_foreground, child_pos.x - 3.f, child_pos.y - 3.f, child_size.x + 6.f, child_size.y + 6.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 2.f, child_pos.y - 2.f, child_size.x + 4.f, child_size.y + 4.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 1.f, child_pos.y - 1.f, child_size.x + 2.f, child_size.y + 2.f, IM_COL32( 130, 130, 0,   opacity * 255 ) ); // most inner border
                render::filled_rectangle( e_draw_list_window, child_pos.x, child_pos.y, child_size.x, child_size.y, IM_COL32( 15, 65, 100, opacity * 125 ) );

                ImGui::Text( "Aimbot" );
                ImGui::Checkbox( "Enable", &enabled );
            }
            ImGui::EndChild( );

            ImGui::SameLine( );

            ImGui::SetCursorPosX( child_size.x + 20.f );
            ImGui::SetCursorPosY( 45.f );

            avail = ImGui::GetContentRegionAvail( );
            child_size = ImVec2( avail.x, ( avail.y - 8 ) / 2 );

            ImGui::BeginChild( "nascarhook_content_2", child_size );
            {
                ImVec2 child_pos = ImGui::GetWindowPos( );

                render::rectangle( e_draw_list_foreground, child_pos.x - 4.f, child_pos.y - 4.f, child_size.x + 8.f, child_size.y + 8.f, IM_COL32( 40,  180, 255, opacity * 255 ) ); // most outer border
                render::rectangle( e_draw_list_foreground, child_pos.x - 3.f, child_pos.y - 3.f, child_size.x + 6.f, child_size.y + 6.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 2.f, child_pos.y - 2.f, child_size.x + 4.f, child_size.y + 4.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 1.f, child_pos.y - 1.f, child_size.x + 2.f, child_size.y + 2.f, IM_COL32( 130, 130, 0,   opacity * 255 ) ); // most inner border
                render::filled_rectangle( e_draw_list_window, child_pos.x, child_pos.y, child_size.x, child_size.y, IM_COL32( 15, 65, 100, opacity * 125 ) );
            }
            ImGui::EndChild( );

            ImGui::SetCursorPosX( child_size.x + 32.f );
            ImGui::SetCursorPosY( child_size.y + 58.f );

            avail = ImGui::GetContentRegionAvail( );
            child_size = ImVec2( avail.x, avail.y );

            ImGui::BeginChild( "nascarhook_content_3", child_size );
            {
                ImVec2 child_pos = ImGui::GetWindowPos( );

                render::rectangle( e_draw_list_foreground, child_pos.x - 4.f, child_pos.y - 4.f, child_size.x + 8.f, child_size.y + 8.f, IM_COL32( 40,  180, 255, opacity * 255 ) ); // most outer border
                render::rectangle( e_draw_list_foreground, child_pos.x - 3.f, child_pos.y - 3.f, child_size.x + 6.f, child_size.y + 6.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 2.f, child_pos.y - 2.f, child_size.x + 4.f, child_size.y + 4.f, IM_COL32( 195, 195, 50,  opacity * 255 ) ); // inner border
                render::rectangle( e_draw_list_foreground, child_pos.x - 1.f, child_pos.y - 1.f, child_size.x + 2.f, child_size.y + 2.f, IM_COL32( 130, 130, 0,   opacity * 255 ) ); // most inner border
                render::filled_rectangle( e_draw_list_window, child_pos.x, child_pos.y, child_size.x, child_size.y, IM_COL32( 15, 65, 100, opacity * 125 ) );
            }
            ImGui::EndChild( );
        }
        ImGui::End( );
        ImGui::PopStyleColor( );
        ImGui::PopStyleColor( );
        ImGui::PopStyleVar( );
        ImGui::PopStyleVar( );
        ImGui::PopStyleVar( );
    }
}

HRESULT __stdcall cheat::hooks::end_scene( IDirect3DDevice9* device )
{
    render::start( );

    render_menu( );

    render::end( );

    return o_end_scene( device );
}

LRESULT __stdcall cheat::hooks::wnd_proc( HWND window, UINT message, WPARAM wparam, LPARAM lparam )
{
    IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

    if ( ImGui_ImplWin32_WndProcHandler( window, message, wparam, lparam ) )
        return true;

    return CallWindowProcA( o_wnd_proc, window, message, wparam, lparam );
}
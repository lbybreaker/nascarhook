#include <imgui/imgui_freetype.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_internal.h>
#include <misc/nascarhook_icon.h>
#include <setup/offsets.h>
#include <misc/render.h>
#include <d3dx9.h>

namespace cheat {
namespace render
{
    bool initialized = { };

    HWND              window = { };
    IDirect3DDevice9* device = { };

    ImFont*            fonts   [ e_font_max    ] = { };
    IDirect3DTexture9* textures[ e_texture_max ] = { };

    DWORD                        srgb_write         = { };
    DWORD                        color_write        = { };
    IDirect3DStateBlock9*        state_block        = { };
    IDirect3DVertexShader9*      vertex_shader      = { };
    IDirect3DVertexDeclaration9* vertex_declaration = { };
}}

namespace cheat {
namespace render
{
    ImDrawList* get_draw_list( draw_list_t draw_list_type )
    {
        return draw_list_type == e_draw_list_background ? ImGui::GetBackgroundDrawList( ) :
               draw_list_type == e_draw_list_foreground ? ImGui::GetForegroundDrawList( ) :
               draw_list_type == e_draw_list_window     ? ImGui::GetWindowDrawList( ) : nullptr;
    }

    ImVec2 get_text_size( ImFont* font, const char* text )
    {
        float  font_size = font->FontSize;
        ImVec2 text_size = { };

        if ( !text )
            return ImVec2( 0.f, font_size );

        text_size = font->CalcTextSizeA( font_size, FLT_MAX, -1.f, text );

        text_size.x = IM_FLOOR( text_size.x + 0.9f ); // 0.99999f

        return text_size;
    }
}}

bool cheat::render::initialize( )
{
    D3DDEVICE_CREATION_PARAMETERS creation_parameters = { };

    if ( initialized )
        return false;

    device = offsets::shader_device;

    if ( !device )
        return false;

    if ( device->GetCreationParameters( &creation_parameters ) > 0 )
        return false;

    if ( !ImGui::CreateContext( ) )
        return false;

    ImGui::GetIO( ).IniFilename = nullptr;
    ImGui::GetIO( ).ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    if ( !ImGui_ImplDX9_Init( device ) )
        return false;

    if ( !ImGui_ImplWin32_Init( creation_parameters.hFocusWindow ) )
        return false;

    fonts[ e_font_default    ] = ImGui::GetIO( ).Fonts->AddFontDefault( );
    fonts[ e_font_comic_sans ] = ImGui::GetIO( ).Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\comic.ttf", 48.f );

    if ( D3DXCreateTextureFromFileInMemory( device, texture_data::nascarhook_logo, sizeof( texture_data::nascarhook_logo ), &textures[ e_texture_nascarhook_icon ] ) != D3D_OK )
        return false;

    initialized = true;
    return		  true;
}

void cheat::render::shutdown( )
{
    initialized = false;
}

void cheat::render::start( )
{
    if ( device->GetVertexDeclaration( &vertex_declaration ) > 0 )
        return;

    if ( device->GetVertexShader( &vertex_shader ) > 0 )
        return;

    if ( device->CreateStateBlock( D3DSBT_PIXELSTATE, &state_block ) > 0 )
        return;

    if ( state_block->Capture( ) > 0 )
        return;

    if ( device->GetRenderState( D3DRS_COLORWRITEENABLE, &color_write ) > 0 )
        return;

    if ( device->GetRenderState( D3DRS_SRGBWRITEENABLE, &srgb_write ) > 0 )
        return;

    if ( device->SetRenderState( D3DRS_COLORWRITEENABLE, 0xFFFFFFFF ) > 0 )
        return;

    if ( device->SetRenderState( D3DRS_SRGBWRITEENABLE, false ) > 0 )
        return;

    ImGui_ImplDX9_NewFrame( );
    ImGui_ImplWin32_NewFrame( );
    ImGui::NewFrame( );
}

void cheat::render::end( )
{
    if ( device->SetRenderState( D3DRS_COLORWRITEENABLE, color_write ) > 0 )
        return;

    if ( device->SetRenderState( D3DRS_SRGBWRITEENABLE, srgb_write ) > 0 )
        return;

    ImGui::EndFrame( );
    ImGui::Render( );
    ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );

    if ( state_block->Apply( ) > 0 )
         return;

    if ( state_block->Release( ) != 0 )
        return;

    if ( device->SetVertexShader( vertex_shader ) > 0 )
        return;

    if ( device->SetVertexDeclaration( vertex_declaration ) > 0 )
        return;
}

void cheat::render::line( draw_list_t draw_list, float x, float y, float x1, float y1, unsigned int color, float thickness )
{
    get_draw_list( draw_list )->AddLine( ImVec2( x, y ), ImVec2( x1, y1 ), color, thickness );
}

void cheat::render::poly_line( draw_list_t draw_list, void* points, int point_count, unsigned int color, bool closed, float thickness )
{
    get_draw_list( draw_list )->AddPolyline( ( ImVec2* )points, point_count, color, closed, thickness );
}

void cheat::render::rectangle( draw_list_t draw_list, float x, float y, float w, float h, unsigned int color, float rounding, int rounding_flags, float thickness )
{
    get_draw_list( draw_list )->AddRect( ImVec2( x, y ), ImVec2( x + w, y + h ), color, rounding, rounding_flags, thickness );
}

void cheat::render::filled_rectangle( draw_list_t draw_list, float x, float y, float w, float h, unsigned int color, float rounding, int rounding_flags )
{
    get_draw_list( draw_list )->AddRectFilled( ImVec2( x, y ), ImVec2( x + w, y + h ), color, rounding, rounding_flags );
}

void cheat::render::filled_gradient_rectangle( draw_list_t draw_list, float x, float y, float w, float h, unsigned int color_tl, unsigned int color_tr, unsigned int color_bl, unsigned int color_br, float rounding, int rounding_flags )
{
    get_draw_list( draw_list )->AddRectFilledMultiColor( ImVec2( x, y ), ImVec2( x + w, y + h ), color_tl, color_tr, color_br, color_bl );
}

void cheat::render::triangle( draw_list_t draw_list, float x, float y, float x1, float y1, float x2, float y2, unsigned int color, float thickness )
{
    get_draw_list( draw_list )->AddTriangle( ImVec2( x, y ), ImVec2( x1, y1 ), ImVec2( x2, y2 ), color, thickness );
}

void cheat::render::filled_triangle( draw_list_t draw_list, float x, float y, float x1, float y1, float x2, float y2, unsigned int color )
{
    get_draw_list( draw_list )->AddTriangleFilled( ImVec2( x, y ), ImVec2( x1, y1 ), ImVec2( x2, y2 ), color );
}

void cheat::render::filled_gradient_triangle( draw_list_t draw_list, float x, float y, float x1, float y1, float x2, float y2, unsigned int color, unsigned int color1, unsigned int color2 )
{
    // get_draw_list( draw_list )->AddTriangleFilledMulticolor( ImVec2( x, y ), ImVec2( x1, y1 ), ImVec2( x2, y2 ), color, color1, color2 );
}

void cheat::render::circle( draw_list_t draw_list, float x, float y, float radius, unsigned int color, int segments, float thickness )
{
    get_draw_list( draw_list )->AddCircle( ImVec2( x, y ), radius, color, segments, thickness );
}

void cheat::render::filled_circle( draw_list_t draw_list, float x, float y, float radius, unsigned int color, int segments )
{
    get_draw_list( draw_list )->AddCircleFilled( ImVec2( x, y ), radius, color, segments );
}

void cheat::render::text( draw_list_t draw_list, font_t font, float x, float y, unsigned int color, font_flags_t flags, const char* text )
{
    ImGui::PushFont( fonts[ font ] );
        get_draw_list( draw_list )->AddText( ImVec2( x, y ), color, text );
    ImGui::PopFont( );
}

void cheat::render::image( draw_list_t draw_list, texture_t texture, float x, float y, float w, float h, unsigned int color, float uv_min_x, float uv_min_y, float uv_max_x, float uv_max_y )
{
    get_draw_list( draw_list )->AddImage( ImTextureID( textures[ texture ] ), ImVec2( x, y ), ImVec2( x + w, y + h ), ImVec2( uv_min_x, uv_min_y ), ImVec2( uv_max_x, uv_max_y ), color );
}

bool cheat::render::world_to_screen( )
{
    return true;
}
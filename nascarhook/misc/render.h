#ifndef RENDER_H
#define RENDER_H
#pragma once

#include <string>
#include <format>

enum draw_list_t
{
    e_draw_list_background,
    e_draw_list_foreground,
    e_draw_list_window
};

enum font_flags_t
{
    e_font_flags_none,
    e_font_flags_x_centered,
    e_font_flags_y_centered,
    e_font_flags_dropshadow,
    e_font_flags_light_back,
    e_font_flags_outline
};

enum font_t
{
    e_font_default,
    e_font_comic_sans,
    e_font_max
};

enum texture_t
{
    e_texture_nascarhook_icon,
    e_texture_ragebot_icon,
    e_texture_antiaim_icon,
    e_texture_visuals_icon,
    e_texture_misc_icon,
    e_texture_player_icon,
    e_texture_keybinds_icon,
    e_texture_scripts_icon,
    e_texture_max
};

namespace cheat {
namespace render
{
    bool initialize( );
    void shutdown( );

    void start( );
    void end( );

    void line( draw_list_t draw_list, float x, float y, float x1, float y1, unsigned int color, float thickness = 1.f );
    void poly_line( draw_list_t draw_list, void* points, int point_count, unsigned int color, bool closed, float thickness = 1.f );

    void rectangle( draw_list_t draw_list, float x, float y, float w, float h, unsigned int color, float rounding = 0.f, int rounding_flags = 0, float thickness = 1.f );
    void filled_rectangle( draw_list_t draw_list, float x, float y, float w, float h, unsigned int color, float rounding = 0.f, int rounding_flags = 0 );
    void filled_gradient_rectangle( draw_list_t draw_list, float x, float y, float w, float h, unsigned int color_tl, unsigned int color_tr, unsigned int color_bl, unsigned int color_br, float rounding = 0.f, int rounding_flags = 0 );

    void triangle( draw_list_t draw_list, float x, float y, float x1, float y1, float x2, float y2, unsigned int color, float thickness = 1.f );
    void filled_triangle( draw_list_t draw_list, float x, float y, float x1, float y1, float x2, float y2, unsigned int color );
    void filled_gradient_triangle( draw_list_t draw_list, float x, float y, float x1, float y1, float x2, float y2, unsigned int color, unsigned int color1, unsigned int color2 );

    void circle( draw_list_t draw_list, float x, float y, float radius, unsigned int color, int segments, float thickness = 1.f );
    void filled_circle( draw_list_t draw_list, float x, float y, float radius, unsigned int color, int segments );

    void text( draw_list_t draw_list, font_t font, float x, float y, unsigned int color, font_flags_t flags, const char* text );

    template< typename... args_t >
    void formatted_text( draw_list_t draw_list, font_t font, float x, float y, unsigned int color, font_flags_t flags, std::string_view text, args_t... format )
    {
        render::text( draw_list, font, x, y, color, flags, std::vformat( text, std::make_format_args( format... ) ).c_str( ) );
    }
    
    void image( draw_list_t draw_list, texture_t texture, float x, float y, float w, float h, unsigned int color = 0xFFFFFFFF, float uv_min_x = 0.f, float uv_min_y = 0.f, float uv_max_x = 1.f, float uv_max_y = 1.f );

    bool world_to_screen( );
}}

#endif // RENDER_H
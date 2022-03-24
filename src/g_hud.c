#include "gf2d_graphics.h"
#include "g_hud.h"
#include "g_entity.h"
#include "gf2d_draw.h"
#include "simple_logger.h"

HUD hud;

void HUD_init()
{
	TTF_Init();

	hud.renderer = gf2d_graphics_get_renderer();
	hud.font = TTF_OpenFont( "fonts/arial.ttf", 32 );
}

void HUD_draw()
{

   if (entity_get_enemy_count() <= 0)
   {
       HUD_draw_message( "Kill All Enemies", vector2d( 32, 32 ), vector3d( 0, 255, 0 ), 130, 21 );
   }
   else
   {
       HUD_draw_message( "Kill All Enemies", vector2d( 32, 32 ), vector3d( 255, 255, 255 ), 130, 21 );
   }

   if (entity_get_intel_count() <= 0)
   {
       HUD_draw_message( "All Intel Lcated", vector2d( 32, 96 ), vector3d( 0, 255, 0 ), 130, 21 );

   }
   else
   {
       char text[64];
       snprintf( text, sizeof( text ), "Locate %i Intel", entity_get_intel_count() );
       HUD_draw_message( text, vector2d( 32, 96 ), vector3d( 255, 255, 255 ), 130, 21 );
   }
}

void HUD_draw_message( char *text, Vector2D position, Vector3D color, Uint32 w, Uint32 h )
{
    SDL_Color c = { color.x, color.y, color.z };
    SDL_Rect rect = { position.x, position.y, 0, 0 };

    hud.surface = TTF_RenderText_Blended( hud.font, text, c );
    hud.texture = SDL_CreateTextureFromSurface( hud.renderer, hud.surface );

    TTF_SizeText( hud.font, hud.texture, &rect.w, &rect.h );

    rect.w = w;
    rect.h = h;
    SDL_RenderCopy( hud.renderer, hud.texture, NULL, &rect );

    SDL_FreeSurface( hud.surface );
    SDL_DestroyTexture( hud.texture );
}
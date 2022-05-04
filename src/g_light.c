#include "g_light.h"
#include "g_static_entity.h"
#include "g_entity.h"
#include "gf2d_draw.h"
#include "g_collision.h"
#include "g_globals.h"
#include "gf2d_graphics.h"
#include "simple_logger.h"
#include "g_menu.h"
#include "g_hud.h"

void light_calculate( Vector2D pos, Vector2D target );
void light_draw( Vector2D origin );
void light_debug();

static LightData light_data;

void light_init()
{
	light_data.debug = false;
	light_data.mask = SDL_CreateTexture( gf2d_graphics_get_renderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_screen_width, g_screen_height );

	light_data.mask = SDL_CreateTexture( gf2d_graphics_get_renderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_screen_width, g_screen_height );

	SDL_Surface *unlit = IMG_Load( "images/backgrounds/floor.png" );
	light_data.unlit_background = SDL_CreateTextureFromSurface( gf2d_graphics_get_renderer(), unlit );

	SDL_Surface *lit = IMG_Load( "images/backgrounds/floor.png" );
	light_data.lit_background = SDL_CreateTextureFromSurface( gf2d_graphics_get_renderer(), lit );

	//light_data.mask = gf2d_sprite_load_image();

	SDL_FreeSurface( unlit );
	SDL_FreeSurface( lit );

}

void light_update()
{
	if (g_state != G_RUN) return;
	Entity *player = entity_manager_get_player();
	if (!player) return;
	SDL_Renderer *renderer = gf2d_graphics_get_renderer();

	

	//SDL_SetTextureBlendMode( target, SDL_BLENDMODE_BLEND );
	SDL_SetRenderTarget( renderer, light_data.mask );

	float x_space = g_screen_width / 600;  //	Accuracy of lighting
	float y_space = g_screen_height / 300; //

	for (float x = 0; x <= g_screen_width; x+= x_space)
	{
		light_calculate( player->position, vector2d( x, 0 ) );
	}
	light_draw( player->position );

	for (float y = 0; y <= g_screen_height; y+= y_space)
	{
		light_calculate( player->position, vector2d( 0, y ) );
	}
	light_draw( player->position );
	for (float x = g_screen_width; x >= 0; x-= x_space)
	{
		light_calculate( player->position, vector2d( x, g_screen_height ) );
	}
	light_draw( player->position );
	
	float y;
	for (float y = g_screen_height; y >= 0; y -= y_space)
	{
		light_calculate( player->position, vector2d( g_screen_width, y ) );
	}
	light_draw( player->position );



	
	SDL_Texture *second_pass = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, g_screen_width, g_screen_height );
	SDL_SetTextureBlendMode( second_pass, SDL_BLENDMODE_BLEND );
	SDL_SetRenderTarget( renderer, second_pass );
	SDL_SetTextureBlendMode( light_data.mask, SDL_BLENDMODE_MOD );
	SDL_SetTextureBlendMode( light_data.lit_background, SDL_BLENDMODE_NONE );
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0 );
	SDL_RenderClear( renderer );
	SDL_RenderCopy( renderer, light_data.lit_background, NULL, NULL );
	SDL_RenderCopy( renderer, light_data.mask, NULL, NULL );
	SDL_SetRenderTarget( renderer, NULL ); 
	SDL_SetRenderDrawColor( renderer, 0, 255, 0, 0 );
	SDL_RenderClear( renderer );
	SDL_RenderCopy( renderer, second_pass, NULL, NULL );

	SDL_DestroyTexture( second_pass );

	SDL_SetRenderTarget( renderer, light_data.mask );
	SDL_RenderCopy( renderer, light_data.unlit_background, NULL, NULL );
	SDL_SetRenderTarget( renderer, NULL );

	if (light_data.debug)
	{
		light_debug();
	}
}

void light_calculate(Vector2D pos, Vector2D target)
{
	if (light_data.vertex_count >= 1024) return;
	HitObj hit;


	hit = raycast_between(pos, target, 9999, NULL, NULL);
	if (!hit.entity && !hit.static_entity) return;
	if (hit.position.x < 0 || hit.position.y < 0)
	{
		
		light_data.verts[light_data.vertex_count].position.x = target.x;
		light_data.verts[light_data.vertex_count].position.y = target.y;
		light_data.verts[light_data.vertex_count].color.r = 255;
		light_data.verts[light_data.vertex_count].color.g = 255;
		light_data.verts[light_data.vertex_count].color.b = 255;
		light_data.verts[light_data.vertex_count].color.a = 255;//MAX( MIN( 256 - vector2d_magnitude_between( pos, target ), 256 ), 0 );

		light_data.vertex_count++;
		return;
	}
	light_data.verts[light_data.vertex_count].position.x = hit.position.x;
	light_data.verts[light_data.vertex_count].position.y = hit.position.y;
	light_data.verts[light_data.vertex_count].color.r = 255;
	light_data.verts[light_data.vertex_count].color.g = 255;
	light_data.verts[light_data.vertex_count].color.b = 255;
	light_data.verts[light_data.vertex_count].color.a = 255;//MAX( MIN( 256 - vector2d_magnitude_between( pos, hit.position ), 256 ), 0 );


	light_data.vertex_count++;
	return;

}

void light_draw( Vector2D origin)
{
	SDL_SetRenderDrawColor( gf2d_graphics_get_renderer(), 0, 0, 0, 255 );

	for (int i = 0; i < light_data.vertex_count - 1; i++) // Insert origin to complete triangle
	{
		SDL_Vertex tri[3];

		tri[0].position.x = origin.x;
		tri[0].position.y = origin.y;
		tri[0].color.r = 255;
		tri[0].color.g = 255;
		tri[0].color.b = 255;
		tri[0].color.a = 255;

		tri[1] = light_data.verts[i];
		tri[2] = light_data.verts[i + 1];
		SDL_RenderGeometry( gf2d_graphics_get_renderer(), NULL, tri, 3, NULL, 0 );
	}

	SDL_SetRenderDrawColor( gf2d_graphics_get_renderer(), 255, 255, 255, 255 );

	light_data.vertex_count = 0;
}

void light_debug()
{
}

LightData* light_data_get()
{
	return &light_data;
}

void light_data_enable_debug()
{
	light_data.debug = true;
}

void light_data_disable_debug()
{
	light_data.debug = false;
}


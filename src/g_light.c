#include "g_light.h"
#include "g_static_entity.h"
#include "g_entity.h"
#include "gf2d_draw.h"
#include "g_collision.h"
#include "g_globals.h"
#include "gf2d_graphics.h"
#include "simple_logger.h"
#include "g_menu.h"

void light_draw_debug( Vector2D pos, Vector2D target );

static LightData light_data;

void light_init()
{
	light_data.debug = true;
}

void light_update()
{
	light_data.vertex_count = 0;

	Entity *player = entity_manager_get_player();
	if (!player) return;

	if (light_data.debug)
	{
		float x_space = g_screen_width / 5;
		float y_space = g_screen_height / 5;


		for (float x = 0; x < g_screen_width; x+= x_space)
		{
			light_draw_debug( player->position, vector2d( x, 0 ) );
		}
		for (float y = 0; y < g_screen_height; y+= y_space)
		{
			light_draw_debug( player->position, vector2d( 0, y ) );
		}
		for (float x = g_screen_width; x > 0; x-= x_space)
		{
			light_draw_debug( player->position, vector2d( x, g_screen_height ) );
		}
		for (float y = g_screen_height; y > 0; y -= y_space)
		{
			light_draw_debug( player->position, vector2d( g_screen_width, y ) );
		}

		light_draw_debug( player->position, vector2d( 0, g_screen_height ) );
	}


	SDL_SetRenderDrawColor( gf2d_graphics_get_renderer(), 0, 0, 0, 255 );

	int count = 0;

	for (int i = 0; i < light_data.vertex_count - 1; i++)
	{
		SDL_Vertex tri[3];

		tri[0].position.x = player->position.x;
		tri[0].position.y = player->position.y;
		tri[0].color.r = 255;
		tri[0].color.g = 255;
		tri[0].color.b = 255;
		tri[0].color.a = 255;

		tri[1] = light_data.verts[i];
		tri[2] = light_data.verts[i + 1];

		if (!vector2d_distance_between_less_than( 
			vector2d( light_data.verts[i].position.x, light_data.verts[i].position.y ),
			vector2d( light_data.verts[i + 1].position.x, light_data.verts[i + 1].position.y ),
			64 ))
		{
			gf2d_draw_line( player->position, vector2d( light_data.verts[i].position.x, light_data.verts[i].position.y ), vector4d( 255, 0, 0, 255 ) );
			gf2d_draw_line( vector2d( light_data.verts[i].position.x, light_data.verts[i].position.y ), vector2d( light_data.verts[i + 1].position.x, light_data.verts[i + 1].position.y ), vector4d( 255, 0, 0, 255 ) );
			gf2d_draw_line( player->position, vector2d( light_data.verts[i + 1].position.x, light_data.verts[i + 1].position.y ), vector4d( 255, 0, 0, 255 ) );
			//continue;
			tri[1] = light_data.verts[i + 1];
			tri[2] = light_data.verts[i + 2]; // Dumb fix
		}
		else
		{
			continue;
		}
		


		count++;

		SDL_RenderGeometry( gf2d_graphics_get_renderer(), NULL, tri, 3, NULL, 0 );
	}

	//slog( "C, L, CL: %i, %i, %i", count, light_data.vertex_count, count + light_data.vertex_count );

	SDL_SetRenderDrawColor( gf2d_graphics_get_renderer(), 255, 255, 255, 255 );

}

void light_draw_debug(Vector2D pos, Vector2D target)
{
	if (light_data.vertex_count == 1024) return;
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
		light_data.verts[light_data.vertex_count].color.a = 255;

		light_data.vertex_count++;
		//gf2d_draw_line( pos, target, vector4d( 0, 255, 0, 128 ) );
		return;
	}
	light_data.verts[light_data.vertex_count].position.x = hit.position.x;
	light_data.verts[light_data.vertex_count].position.y = hit.position.y;
	light_data.verts[light_data.vertex_count].color.r = 255;
	light_data.verts[light_data.vertex_count].color.g = 255;
	light_data.verts[light_data.vertex_count].color.b = 255;
	light_data.verts[light_data.vertex_count].color.a = 255;

	//gf2d_draw_line( pos, hit.position, vector4d( 0, 255, 0, 100 ) );
	//gf2d_draw_line( hit.position, target, vector4d( 255, 0, 255, 100 ) );
	gf2d_draw_circle( hit.position, 6, vector4d( 0, 0, 255, 255 ) );
	light_data.vertex_count++;
	return;

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
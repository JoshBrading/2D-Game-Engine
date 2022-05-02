#include "g_light.h"
#include "g_static_entity.h"
#include "g_entity.h"
#include "gf2d_draw.h"
#include "g_collision.h"
#include "g_globals.h"
#include "gf2d_graphics.h"
#include "simple_logger.h"
#include "g_menu.h"

void light_calculate( Vector2D pos, Vector2D target );
void light_draw( Vector2D origin );

static LightData light_data;

void light_init()
{
	light_data.debug = false;
}

void light_update()
{

	Entity *player = entity_manager_get_player();
	if (!player) return;

	float x_space = g_screen_width / 200;  //	Accuracy of lighting
	float y_space = g_screen_height / 100; //


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

	//light_data.vertex_count = 0;
	//light_calculate( player->position, vector2d( 0, g_screen_height ) );
	//light_draw( player->position, vector4d( 255, 255, 255, 25 ) );



}

void light_calculate(Vector2D pos, Vector2D target)
{
	if (light_data.vertex_count == 1024) return;
	HitObj hit;

	hit = raycast_between(pos, target, 9999, NULL, NULL);
	if (!hit.entity && !hit.static_entity) return;
	if (hit.position.x < 0 || hit.position.y < 0)
	{
		
		light_data.verts[light_data.vertex_count].position.x = target.x;
		light_data.verts[light_data.vertex_count].position.y = target.y;
		light_data.verts[light_data.vertex_count].color.r = 200;
		light_data.verts[light_data.vertex_count].color.g = 200;
		light_data.verts[light_data.vertex_count].color.b = 128;
		light_data.verts[light_data.vertex_count].color.a = 25;

		light_data.vertex_count++;

		if (light_data.debug)
		{
			gf2d_draw_line( pos, target, vector4d( 0, 255, 0, 50 ) );
			gf2d_draw_circle( target, 6, vector4d( 0, 0, 255, 255 ) );
		}
		return;
	}
	light_data.verts[light_data.vertex_count].position.x = hit.position.x;
	light_data.verts[light_data.vertex_count].position.y = hit.position.y;
	light_data.verts[light_data.vertex_count].color.r = 200;
	light_data.verts[light_data.vertex_count].color.g = 200;
	light_data.verts[light_data.vertex_count].color.b = 128;
	light_data.verts[light_data.vertex_count].color.a = 25;

	if (light_data.debug)
	{
		gf2d_draw_line( pos, hit.position, vector4d( 0, 255, 0, 50 ) );
		gf2d_draw_line( hit.position, target, vector4d( 255, 0, 255, 50 ) );
		gf2d_draw_circle( hit.position, 6, vector4d( 0, 0, 255, 255 ) );
	}
	light_data.vertex_count++;
	return;

}

void light_draw( Vector2D origin)
{
	SDL_SetRenderDrawColor( gf2d_graphics_get_renderer(), 0, 0, 0, 255 );

	for (int i = 0; i < light_data.vertex_count - 1; i++)
	{
		SDL_Vertex tri[3];

		tri[0].position.x = origin.x;
		tri[0].position.y = origin.y;
		tri[0].color.r = 200;
		tri[0].color.g = 200;
		tri[0].color.b = 28;
		tri[0].color.a = 25;

		tri[1] = light_data.verts[i];
		tri[2] = light_data.verts[i + 1];

		//if (!vector2d_distance_between_less_than(
		//	vector2d( light_data.verts[i].position.x, light_data.verts[i].position.y ),
		//	vector2d( light_data.verts[i + 1].position.x, light_data.verts[i + 1].position.y ),
		//	128 ))
		//{
		//	if (light_data.debug)
		//	{
		//		gf2d_draw_line( origin, vector2d( light_data.verts[i].position.x, light_data.verts[i].position.y ), vector4d( 255, 0, 0, 100 ) );
		//		gf2d_draw_line( vector2d( light_data.verts[i].position.x, light_data.verts[i].position.y ), vector2d( light_data.verts[i + 1].position.x, light_data.verts[i + 1].position.y ), vector4d( 255, 0, 0, 100 ) );
		//		gf2d_draw_line( origin, vector2d( light_data.verts[i + 1].position.x, light_data.verts[i + 1].position.y ), vector4d( 255, 0, 0, 100 ) );
		//	}
		//	continue;
		//	//tri[1] = light_data.verts[i + 1];
		//	//tri[2] = light_data.verts[i + 2]; // Dumb fix
		//}
		SDL_RenderGeometry( gf2d_graphics_get_renderer(), NULL, tri, 3, NULL, 0 );
	}

	SDL_SetRenderDrawColor( gf2d_graphics_get_renderer(), 255, 255, 255, 255 );

	light_data.vertex_count = 0;
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
#include "g_light.h"
#include "g_static_entity.h"
#include "g_entity.h"
#include "gf2d_draw.h"
#include "g_collision.h"
#include "g_globals.h"

void light_draw_debug( Vector2D pos, Vector2D target );

static LightData light_data;

void light_init()
{

}

void light_update()
{
	StaticEntityManager *sem = static_entity_manager_get();

	Entity *player = entity_manager_get_player();

	for (int i = 0; i < sem->static_entity_count; i++)
	{
		StaticEntity *sEnt = &sem->static_entity_list[i];

		if (!sEnt) continue;
		if (!sEnt->_inuse) continue;

		Vector2D x1, x2, y1, y2;

		x1 = vector2d(sEnt->bounds.x, sEnt->bounds.y);
		x2 = vector2d( sEnt->bounds.x + sEnt->bounds.w, sEnt->bounds.y );
		y1 = vector2d(sEnt->bounds.x, sEnt->bounds.y + sEnt->bounds.h);
		y2 = vector2d( sEnt->bounds.x + sEnt->bounds.w, sEnt->bounds.y + sEnt->bounds.h );

		gf2d_draw_line( player->position, x1, vector4d( 255, 0, 255, 128 ) );
		gf2d_draw_line( player->position, x2, vector4d( 255, 0, 255, 128 ) );
		gf2d_draw_line( player->position, y1, vector4d( 255, 0, 255, 128 ) );
		gf2d_draw_line( player->position, y2, vector4d( 255, 0, 255, 128 ) );

		light_draw_debug( player->position, x1 );
		light_draw_debug( player->position, x2 );
		light_draw_debug( player->position, y1 );
		light_draw_debug( player->position, y2 );

		
	}

	light_draw_debug( player->position, vector2d( 0, g_screen_width ) );
	light_draw_debug( player->position, vector2d( 0, g_screen_height ) );
	light_draw_debug( player->position, vector2d( g_screen_width, g_screen_height ) );
	light_draw_debug( player->position, vector2d( 0, 0 ) );
}

void light_draw_debug( Vector2D pos, Vector2D target )
{
	HitObj hit;

	hit = raycast_between( pos, target, 9999, NULL, NULL );
	if (hit.entity) return;
	if (!hit.static_entity) return;
	if (hit.position.x < 0 || hit.position.y < 0)
	{
		gf2d_draw_line( pos, target, vector4d( 255, 255, 255, 255 ) );
		return;
	}
	gf2d_draw_line( pos, hit.position, vector4d( 255, 255, 255, 255 ) );

	hit = raycast_between( pos, target, 9999, hit.static_entity->_id, NULL );
	if (hit.entity) return;
	if (!hit.static_entity) return;
	gf2d_draw_line( pos, hit.position, vector4d( 255, 255, 255, 255 ) );

}
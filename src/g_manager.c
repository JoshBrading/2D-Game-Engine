#include "g_manager.h"
#include "g_entity.h"
#include "g_static_entity.h"
#include "g_menu.h"
#include "g_world.h"
#include "g_editor.h";
#include "gf2d_graphics.h"
#include "g_hud.h"
#include "g_globals.h"

void game_draw_timings();

static timing_debug = false;

void game_load_mission( char* world_name )
{
	world_load( world_name );
}

void game_load_editor()
{
	game_clear();
	menu_manager_close_all();
	editor_load();
}

void game_mission_quit()
{
	game_clear();
	game_return_to_main_menu();
}

void game_return_to_main_menu()
{
	menu_manager_close_all();
	menu_open( menu_manager_get_by_tag( "main" ) ); // Is this okay?
}

void game_clear()
{
	entity_manager_clear();
	static_entity_manager_clear();
}

void game_quit()
{

}

void game_state_change( int new_state )
{

}

void game_update()
{
	if (timing_debug)
	{
		game_draw_timings();
	}
}

void game_timing_enable_debug()
{
	timing_debug = true;
}

void game_timing_disable_debug()
{
	timing_debug = false;
}



void game_draw_timings()
{
	char *fps[32];
	char *time[32];
	char *time_fixed[32];

	snprintf( fps, sizeof( fps ), "FPS: %0.0f", gf2d_graphics_get_frames_per_second() );

	snprintf( time, sizeof( time ), "Time: %i", g_update_time );
	snprintf( time_fixed, sizeof( time_fixed ), "Time Fixed: %i", g_update_time_fixed );

	HUD_draw_message( fps, vector2d( 900, 16 ), vector3d( 255, 2555, 255 ), 16, 32 );
	HUD_draw_message( time, vector2d( 900, 48 ), vector3d( 255, 2555, 255 ), 16, 32 );
	HUD_draw_message( time_fixed, vector2d( 900, 64 ), vector3d( 255, 2555, 255 ), 16, 32 );
}
#include "g_manager.h"
#include "g_entity.h"
#include "g_static_entity.h"
#include "g_menu.h"
#include "g_world.h"
#include "g_editor.h";

void game_load_mission( char* world_name )
{
	world_load( "config/asset_list.json" );
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
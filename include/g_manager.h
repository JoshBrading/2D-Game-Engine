#ifndef __G_MANAGER_H__
#define __G_MANAGER_H__

void game_load_mission( char* world_name);

void game_load_editor();

void game_mission_quit();

void game_return_to_main_menu();

void game_clear();

void game_quit();

void game_state_change( int new_state );

void game_update();

void game_timing_enable_debug();

void game_timing_disable_debug();

#endif // !__G_MANAGER_H__

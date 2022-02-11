#include <SDL.h>
#include <stdlib.h>
#include <math.h>
#include "g_entity.h"
#include "g_collision.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"

typedef struct
{
	CollisionCell	*cell_list;
	Uint32			cell_count;
	Vector2D		cell_xy;
}CollisionSystem;

static CollisionSystem collision_system = { 0 };

void collision_system_init( Vector2D cell_xy )
{
	int maxCells = cell_xy.x * cell_xy.y;
	collision_system.cell_list = gfc_allocate_array( sizeof( CollisionCell ), maxCells );
	if (collision_system.cell_list == NULL)
	{
		slog( "CollisionSystemInit: Failed to allocate array of size: %i", maxCells );
		return;
	}
	collision_system.cell_count = maxCells;
	//atexit( entity_manager_close );
	slog( "CollisionSystem: Initialized" );
	collision_system_generate_cells( cell_xy );
}

void collision_system_draw_debug()
{
	for (int i = 0; i < collision_system.cell_count; i++)
	{
		if (collision_system.cell_list[i]._inuse)
		{
			SDL_Rect rectToDraw = { collision_system.cell_list[i].cell_position.x, collision_system.cell_list[i].cell_position.y, collision_system.cell_list[i].bBox.x, collision_system.cell_list[i].bBox.y };
			if ( collision_system.cell_list[i].entity_count > 0 )
			{
				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 0, 0, 255 ) );
			}
			else
			{
				gf2d_draw_rect ( rectToDraw, vector4d ( 200, 200, 200, 255 ) );
			}
		}
	}
}

void collision_system_generate_cells( Vector2D cell_xy )
{
	int height = 0;
	int width = 0;

	SDL_Window *window = gf2d_graphics_get_window();
	SDL_GetWindowSize( window, &width, &height );

	float cell_x = width / cell_xy.x;
	float cell_y = height / cell_xy.y;

	int cell_num = 0;
	for (int row = 0; row < width; row += cell_x)
	{
		for (int col = 0; col < height; col += cell_y)
		{
			collision_system.cell_list[cell_num]._inuse = 1;
			collision_system.cell_list[cell_num].cell_position = vector2d( row, col );
			collision_system.cell_list[cell_num].bBox = vector2d( cell_x, cell_y );
			collision_system.cell_list[cell_num].entity_count = -1;
			cell_num++;
		}
	}
	return;
}

CollisionCell* collision_system_get_nearest_cell_within_range ( Vector2D position, float distance )
{
	CollisionCell *cell;
	cell = NULL;
	for ( int i = 0; i < collision_system.cell_count; i++ )
	{
		if ( collision_system.cell_list[i]._inuse )
		{
			float dist = sqrt ( pow ( collision_system.cell_list[i].cell_position.x + (collision_system.cell_list[i].bBox.x/2) - position.x, 2 ) + pow ( collision_system.cell_list[i].cell_position.y - (collision_system.cell_list[i].bBox.y / 2) - position.y, 2 ) );
			if ( dist < distance )
			{
				cell = &collision_system.cell_list[i];
				distance = dist;

			}
		}

	}
	if ( !cell ) return;
	return cell;
}

void collision_cell_add_entity ( CollisionCell* cell, Entity* ent )
{
	slog ( "Called" );
	if ( !cell ) return;
	if ( !ent ) return;
	cell->entity_list[cell->entity_count + 1] = ent;
	cell->entity_count += 1;
	slog ( "CollisionCellAddEntity: Entity added to cell" );
	return;
}
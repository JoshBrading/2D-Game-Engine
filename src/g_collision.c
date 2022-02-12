#include <SDL.h>
#include <stdlib.h>
#include <math.h>
#include "g_entity.h"
#include "g_collision.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_draw.h"

typedef struct
{
	CollisionCell	*cell_list;
	Uint32			cell_count;
	Vector2D		cell_xy;
}CollisionSystem;

static CollisionSystem collision_system = { 0 };

void collision_system_init( Vector2D cell_xy )
{
	collision_system.cell_xy = cell_xy;
	Uint32 maxCells = cell_xy.x * cell_xy.y;
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
				collision_system_check_neighbor_cells( i );

				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 0, 0, 255 ) );
			}
			//else
			//{
			//	gf2d_draw_rect ( rectToDraw, vector4d ( 200, 200, 200, 255 ) );
			//}
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
			collision_system.cell_list[cell_num].entity_count = 0;
			cell_num++;
		}
	}
	return;
}

void collision_system_check_neighbor_cells( Uint32 cell_index )
{
	Uint32 y_offset = collision_system.cell_xy.y;
	Vector4D color = vector4d( 100, 50, 0, 255 );

	Uint32 pos[3][3] = {
		{cell_index - y_offset - 1, cell_index - 1, cell_index + y_offset - 1},
		{cell_index - y_offset,		cell_index,		cell_index + y_offset},
		{cell_index - y_offset + 1,	cell_index + 1, cell_index + y_offset + 1}
	};

	SDL_Rect rectToDraw;
	for (int j = 0; j < 3; j++)
	{
		for (int k = 0; k < 3; k++)
		{
			rectToDraw.x = collision_system.cell_list[pos[j][k]].cell_position.x;
			rectToDraw.y = collision_system.cell_list[pos[j][k]].cell_position.y;
			rectToDraw.w = collision_system.cell_list[pos[j][k]].bBox.x;
			rectToDraw.h = collision_system.cell_list[pos[j][k]].bBox.y;
			gf2d_draw_rect( rectToDraw, color );

		}
	}

}

CollisionCell *collision_system_get_cell_by_index( Uint32 index )
{
	return 	&collision_system.cell_list[index];
}

CollisionCell* collision_system_get_nearest_cell_within_range ( Vector2D position, float distance )
{
	CollisionCell *cell;
	cell = NULL;
	double x1, x2, y1, y2;
	distance = 128.0f;
	for ( int i = 0; i < collision_system.cell_count; i++ )
	{
		if ( collision_system.cell_list[i]._inuse )
		{
			x1 = (double)collision_system.cell_list[i].cell_position.x + (collision_system.cell_list[i].bBox.x / 2);
			y1 = (double)collision_system.cell_list[i].cell_position.y + (collision_system.cell_list[i].bBox.y / 2);

			x2 = position.x;
			y2 = position.y;

			float dist = sqrt( (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) );
			if ( dist < distance )
			{
				cell = &collision_system.cell_list[i];
				distance = dist;
			}
		}
	}
	if (!cell)return NULL;
	return cell;
}

void collision_cell_add_entity ( CollisionCell* cell, Uint32 ent_ID )
{
	if ( !cell ) return;
	for (int i = 0; i <= cell->entity_count; i++)
	{
		if (cell->entity_index_list[i] == 0)
		{
			cell->entity_index_list[cell->entity_count] = ent_ID;
			cell->entity_count += 1;
			slog( "CollisionCellAddEntity: Entity added to cell" );
			return;
		}
	}
	slog( "CollisionCellAddEntity: No free space in entity_index_list" );
	return;
}
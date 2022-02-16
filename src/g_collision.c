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
				//collision_system_check_neighbor_cells( i );
				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 0, 0, 128 ) );

			}
			//else
			//{
			//	gf2d_draw_rect ( rectToDraw, vector4d ( 200, 200, 200, 255 ) );
			//}
		}
	}
}
void collision_system_clear()
{
	for (int i = 0; i < collision_system.cell_count; i++)
	{
		if (collision_system.cell_list[i]._inuse)
		{
			if (collision_system.cell_list[i].entity_count > 0)
			{
				collision_system.cell_list[i].entity_count = 0;
				collision_system.cell_list[i].entity_index_list[0] = 0;
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
			collision_system.cell_list[cell_num].entity_count = 0;
			collision_system.cell_list[cell_num].id = cell_num;
			collision_system.cell_list[cell_num].max_entities = 128;
			collision_system.cell_list[cell_num].entity_list = gfc_allocate_array( sizeof( Entity ), collision_system.cell_list[cell_num].max_entities );
			cell_num++;
		}
	}
	return;
}

void collision_system_check_neighbor_cells_for_collision( CollisionCell* cell, Entity* entity)
{
	if (!entity) return;
	if (!cell) return;
	Uint32 y_offset = collision_system.cell_xy.y;
	Vector4D color = vector4d( 100, 50, 0, 255 );

	Uint32 cell_index = cell->id;

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
			CollisionCell testCell = collision_system.cell_list[pos[j][k]];
			rectToDraw.x = testCell.cell_position.x;
			rectToDraw.y = testCell.cell_position.y;
			rectToDraw.w = testCell.bBox.x;
			rectToDraw.h = testCell.bBox.y;
			gf2d_draw_rect( rectToDraw, color );

			if (testCell.entity_count > 0)
			{
				for (int i = 0; i < testCell.entity_count; i++)
				{
					if (!testCell.entity_list[i]) continue;
					if (!testCell.entity_list[i]->_inuse) continue;
					if (entity->_id == testCell.entity_list[i]->_id) continue;
					collision_rect_test( testCell.entity_list[i]->bounds, entity->bounds );
				}
			}
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

void collision_cell_add_entity ( CollisionCell* cell, Entity* entity )
{
	if ( !cell ) return;
	if (!entity) return;
	
	for (int i = 0; i < cell->max_entities; i++)
	{
		if (cell->entity_list[i] == NULL)
		{
			cell->entity_index_list[cell->entity_count] = entity->_id;
			//cell->entity_count += 1;
			entity->cell = cell;
			cell->entity_list[i] = entity;
			//slog( "Entity Count: %i", cell->entity_count );
			//slog( "CollisionCellAddEntity: Entity %i added to cell %i", entity->_id, cell->id );
			return;
		}
	}
	slog( "CollisionCellAddEntity: No free space in entity_index_list" );
	return;
}


void collision_cell_remove_entity( CollisionCell *cell, Entity *entity )
{
	if (!cell) return;
	if (!entity)return;
	//slog( "===================== REMOVE ====================" );
	for (int i = 0; i <= cell->max_entities + 1; i++)
	{
		//slog( "CollisionCellRemoveEntity: Entity %i removed from cell %i", entity->_id, cell->id );

		if (!cell->entity_list[i]) continue;
		if (cell->entity_list[i]->_id == entity->_id)
		{
			cell->entity_list[i] = NULL;
			return;
		}
		//slog( "CELL: %i, DOES NOT MATCH: %i", cell->entity_index_list[i], entity->_id );
	}

	slog( "CollisionCellRemoveEntity: Entity not in cell" );
	slog( "CollisionCellRemoveEntity: Entity Count: %i", cell->entity_count );
	//cell->entity_count = 0;
	return;
}

void collision_cell_update(CollisionCell* self)
{
	if (!self) return;
	Uint32 entCount = 0;
	for (int i = 0; i < self->max_entities; i++)
	{
		if (self->entity_list[i]) entCount++;
	}
	self->entity_count = entCount;
	if (self->entity_count > 0)
	{
		for (int i = 0; i < self->entity_count; i++)
		{
			Entity *first = self->entity_list[i];
			if (!first) continue;
			for (int h = 0; h < self->entity_count; h++)
			{
				if (h == i) continue;
				Entity *second = self->entity_list[h];
				if (!second) continue;
				collision_rect_test( first->bounds, second->bounds );
			}
			collision_system_check_neighbor_cells_for_collision( self, first );

		}
	}
}

int collision_rect_test( Rect A, Rect B )
{
	if (A.x < B.x + B.w &&
		 A.x + A.w > B.x &&
		 A.y < B.y + B.h &&
		 A.h + A.y > B.y)
	{
		SDL_Rect rectToDraw;
		if (A.x <= B.x)
		{
			rectToDraw.x = B.x;
			rectToDraw.w = (A.x + A.w) - B.x;
		}
		if (A.x >= B.x)
		{
			rectToDraw.x = A.x;
			rectToDraw.w = (B.x + B.w) - A.x;
		}
		if (A.y <= B.y)
		{
			rectToDraw.y = B.y;
			rectToDraw.h = (A.y + A.h) - B.y;
		}
		if (A.y >= B.y)
		{
			rectToDraw.y = A.y;
			rectToDraw.h = (B.y + B.h) - A.y;
		}
		//rectToDraw.w = A.w + B.w;
		//rectToDraw.h = A.h + B.h;
		gf2d_draw_fill_rect( rectToDraw, vector4d( 255, 0, 0, 100 ) );
		
		return 0;
	}
}

void collision_system_update_all()
{
	int i;
	for (i = 0; i < collision_system.cell_count; i++)
	{
		if (!collision_system.cell_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		collision_cell_update( &collision_system.cell_list[i] );
	}
}
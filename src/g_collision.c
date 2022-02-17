#include <SDL.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "g_entity.h"
#include "g_collision.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_draw.h"
#include "g_globals.h"

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
	collision_system.cell_list = gfc_allocate_array( sizeof( CollisionCell ), maxCells ); // This is probably wrong...
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
			gf2d_draw_rect( rectToDraw, vector4d( 200, 200, 200, 20 ) );

			if ( collision_system.cell_list[i].entity_count > 0 )
			{
				//collision_system_check_neighbor_cells( i );
				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 0, 0, 255 ) );

			}
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
			if (pos[j][k] > collision_system.cell_count) continue;
			CollisionCell testCell = collision_system.cell_list[pos[j][k]];
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

			rectToDraw.x = testCell.cell_position.x;
			rectToDraw.y = testCell.cell_position.y;
			rectToDraw.w = testCell.bBox.x;
			rectToDraw.h = testCell.bBox.y;
			if( g_debug ) gf2d_draw_rect( rectToDraw, color );

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

		if (g_debug) gf2d_draw_fill_rect( rectToDraw, vector4d( 255, 0, 0, 100 ) );
		
		return 0;
	}
}

int collision_line_rect_test( Rect A, Line B )
{
	  // check if the line has hit any of the rectangle's sides
	// uses the Line/Line function below

	Line top, bottom, left, right;

	top.a = vector2d( A.x, A.y );
	top.b = vector2d( A.y, A.x + A.w );

	bottom.a = vector2d( A.x, A.y + A.h );
	bottom.b = vector2d( A.x + A.w, A.y + A.h );

	left.a = vector2d( A.x, A.y );
	left.b = vector2d( A.x + A.h, A.y );

	right.a = vector2d( A.x + A.w, A.y );
	right.b = vector2d( A.x + A.w, A.y + A.h );

	gf2d_draw_line( top.a, top.b, vector4d( 255, 255, 0, 255 ) );
	gf2d_draw_line( bottom.a, bottom.b, vector4d( 255, 255, 0, 255 ) );
	gf2d_draw_line( left.a, left.b, vector4d( 255, 255, 0, 255 ) );
	gf2d_draw_line( right.a, right.b, vector4d( 255, 255, 0, 255 ) );

	gf2d_draw_line( B.a, B.b, vector4d( 255, 255, 0, 255 ) );

	collision_line_line_test( top, B );
	collision_line_line_test( bottom, B );
	collision_line_line_test( left, B );
	collision_line_line_test( right, B );

	return false;
}

int collision_line_line_test( Line A, Line B )
{
	 // calculate the direction of the lines
	float uA = ((B.b.x - B.a.x) * (A.a.y - B.a.y) - (B.b.y - B.a.y) * (A.a.x - B.a.x)) / ((B.b.y - B.a.y) * (A.b.x - A.a.x) - (B.b.x - B.a.x) * (A.b.y - A.a.y));
	float uB = ((A.b.x - A.a.x) * (A.a.y - B.a.y) - (A.b.y - A.a.y) * (A.a.x - B.a.x)) / ((B.b.y - B.a.y) * (A.b.x - A.a.x) - (B.b.x - B.a.x) * (A.b.y - A.a.y));

	// if uA and uB are between 0-1, lines are colliding
	if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1)
	{

	// optionally, draw a circle where the lines meet
		float intersectionX = A.a.x + (uA * (A.b.x - A.a.x));
		float intersectionY = A.a.y + (uA * (A.b.y - A.a.y));

		gf2d_draw_circle( vector2d(intersectionX, intersectionY), 6, vector4d( 255, 0, 0, 255 ) );

		return true;
	}
	return false;
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

void raycast( Vector2D origin, Vector2D direction, float max_distance, Entity *hit_entity, Vector2D *hit_point )
{
	Vector2D a, b;
	float m = direction.x / direction.y;

	slog( "(%f, %f)", direction.x, direction.y );
	if (m == 0)
	{
		a.x = origin.x + max_distance;
		a.y = origin.y;

		b.x = origin.x - max_distance;
		b.y = origin.y;
	}
	else if (m == FLT_MAX)
	{
		a.x = origin.x;
		a.y = origin.y + max_distance;

		b.x = origin.x;
		b.y = origin.y - max_distance;
	}
	else
	{
		float dx = (max_distance / sqrt( 1 + (m * m) ));
		float dy = m * dx;

		a.x = origin.x + dx;
		a.y = origin.y + dy;
		b.x = origin.x - dx;
		b.y = origin.y - dy;
	}


	if (direction.x < 0 && direction.y < 0)
	{
		gf2d_draw_circle( b, 6, vector4d( 255, 0, 255, 255 ) );
		gf2d_draw_line( origin, b, vector4d( 255, 255, 0, 255 ) );
	}
	else if (direction.x > 0 && direction.y < 0)
	{
		gf2d_draw_circle( b, 6, vector4d( 255, 0, 255, 255 ) );
		gf2d_draw_line( origin, b, vector4d( 255, 255, 0, 255 ) );
	}
	else
	{
		gf2d_draw_circle( a, 6, vector4d( 255, 0, 0, 255 ) );
		gf2d_draw_line( origin, a, vector4d( 255, 255, 0, 255 ) );
	}
}
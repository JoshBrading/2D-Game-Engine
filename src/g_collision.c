#include <SDL.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "g_entity.h"
#include "g_static_entity.h"
#include "g_collision.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_draw.h"
#include "g_globals.h"
#include "gfc_vector.h"
#include "g_func.h"

typedef struct
{
	CollisionCell* cell_list;
	Uint32			cell_count;
	Vector2D		cell_xy;
}CollisionSystem;

static CollisionSystem collision_system = { 0 };

void collision_system_init ( Vector2D cell_xy )
{
	collision_system.cell_xy = cell_xy;
	Uint32 maxCells = cell_xy.x * cell_xy.y;
	collision_system.cell_list = gfc_allocate_array ( sizeof ( CollisionCell ), maxCells ); // This is probably wrong...
	if ( collision_system.cell_list == NULL )
	{
		slog ( "CollisionSystemInit: Failed to allocate array of size: %i", maxCells );
		return;
	}
	collision_system.cell_count = maxCells;
	atexit( collision_system_close );
	slog ( "CollisionSystem: Initialized" );
	collision_system_generate_cells ( cell_xy );
}

void collision_system_draw_debug ()
{
	for ( int i = 0; i < collision_system.cell_count; i++ )
	{
		if ( collision_system.cell_list[i]._inuse )
		{

			SDL_Rect rectToDraw = { collision_system.cell_list[i].cell_position.x, collision_system.cell_list[i].cell_position.y, collision_system.cell_list[i].bBox.x, collision_system.cell_list[i].bBox.y };
			gf2d_draw_rect ( rectToDraw, vector4d ( 200, 200, 200, 20 ) );

			if ( collision_system.cell_list[i].entity_count > 0 )
			{
				//collision_system_check_neighbor_cells( i );
				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 0, 0, 255 ) );

			}
		}
	}
}
void collision_system_close()
{
	int i;
	for (i = 0; i < collision_system.cell_count; i++)
	{
		collision_cell_free( &collision_system.cell_list[i] );
	}
	//free( collision_system.cell_list ); // free( cell_list ) crashes...
	slog( "CollisionSystem: Closed" );
}

void collision_cell_free( CollisionCell *self )
{
	if (!self)
	{
		slog( "CollisionCellFree: Cell does not exist" );
		return;
	}
	memset( self, 0, sizeof( self ) );
}

void collision_system_generate_cells ( Vector2D cell_xy )
{
	int height = 0;
	int width = 0;

	SDL_Window* window = gf2d_graphics_get_window ();
	SDL_GetWindowSize ( window, &width, &height );

	float cell_x = width / cell_xy.x;
	float cell_y = height / cell_xy.y;

	int cell_num = 0;
	for ( int row = 0; row < width; row += cell_x )
	{
		for ( int col = 0; col < height; col += cell_y )
		{
			collision_system.cell_list[cell_num]._inuse = 1;
			collision_system.cell_list[cell_num].cell_position = vector2d ( row, col );
			collision_system.cell_list[cell_num].bBox = vector2d ( cell_x, cell_y );
			collision_system.cell_list[cell_num].entity_count = 0;
			collision_system.cell_list[cell_num].id = cell_num;
			collision_system.cell_list[cell_num].max_entities = 128;
			collision_system.cell_list[cell_num].entity_list = gfc_allocate_array ( sizeof ( Entity ), collision_system.cell_list[cell_num].max_entities );
			cell_num++;
		}
	}
	return;
}

void collision_system_check_neighbor_cells_for_collision ( CollisionCell* cell, Entity* entity )
{
	if ( !entity ) return;
	if ( !cell ) return;
	Uint32 y_offset = collision_system.cell_xy.y;
	Vector4D color = vector4d ( 100, 50, 0, 255 );

	Uint32 cell_index = cell->id;

	Uint32 pos[3][3] = {
		{cell_index - y_offset - 1, cell_index - 1, cell_index + y_offset - 1},
		{cell_index - y_offset,		cell_index,		cell_index + y_offset},
		{cell_index - y_offset + 1,	cell_index + 1, cell_index + y_offset + 1}
	};

	SDL_Rect rectToDraw;
	for ( int j = 0; j < 3; j++ )
	{
		for ( int k = 0; k < 3; k++ )
		{
			if ( pos[j][k] > collision_system.cell_count ) continue;
			CollisionCell testCell = collision_system.cell_list[pos[j][k]];
			if ( testCell.entity_count > 0 )
			{
				for ( int i = 0; i < testCell.entity_count; i++ )
				{
					if ( !testCell.entity_list[i] ) continue;
					if ( !testCell.entity_list[i]->_inuse ) continue;
					if ( entity->_id == testCell.entity_list[i]->_id ) continue;
					if ( !testCell.entity_list[i]->collision_enabled) continue;
					if (entity->team == TEAM_FRIEND && testCell.entity_list[i]->team == TEAM_FRIEND) continue; // Friendlies do not collide with friendlies

					CollisionInfo info;
					info.side = COL_NULL;
					info.time = g_time;
					if (collision_rect_test( testCell.entity_list[i]->bounds, entity->bounds, &info ))
					{
						entity_on_collision( entity, info );
						//entity_on_collision( testCell.entity_list[i], info );
					}
					else
					{
						entity->col_info.side = COL_NULL;
					}
				}
			}

			rectToDraw.x = testCell.cell_position.x;
			rectToDraw.y = testCell.cell_position.y;
			rectToDraw.w = testCell.bBox.x;
			rectToDraw.h = testCell.bBox.y;
			if ( g_debug ) gf2d_draw_rect ( rectToDraw, color );

		}
	}

}

CollisionCell* collision_system_get_cell_by_index ( Uint32 index )
{
	return 	&collision_system.cell_list[index];
}

CollisionCell* collision_system_get_nearest_cell_within_range ( Vector2D position, float distance )
{
	CollisionCell* cell;
	cell = NULL;
	double x1, x2, y1, y2;
	distance = pow ( distance, 2 );

	for ( int i = 0; i < collision_system.cell_count; i++ )
	{
		if ( collision_system.cell_list[i]._inuse )
		{
			Vector2D a;
			vector2d_sub ( a, position, collision_system.cell_list[i].cell_position );
			float dist = vector2d_magnitude_squared ( a );

			if ( dist < distance )
			{
				cell = &collision_system.cell_list[i];
				distance = dist;
			}
		}
	}
	if ( !cell )return NULL;
	return cell;
}

void collision_cell_add_entity ( CollisionCell* cell, Entity* entity )
{
	if ( !cell ) return;
	if ( !entity ) return;

	for ( int i = 0; i < cell->max_entities; i++ )
	{
		if ( cell->entity_list[i] == NULL )
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
	slog ( "CollisionCellAddEntity: No free space in entity_index_list" );
	return;
}


void collision_cell_remove_entity ( CollisionCell* cell, Entity* entity )
{
	if ( !cell ) return;
	if ( !entity )return;
	//slog( "===================== REMOVE ====================" );
	for ( int i = 0; i <= cell->max_entities + 1; i++ )
	{
		//slog( "CollisionCellRemoveEntity: Entity %i removed from cell %i", entity->_id, cell->id );

		if ( !cell->entity_list[i] ) continue;
		if ( cell->entity_list[i]->_id == entity->_id )
		{
			cell->entity_list[i] = NULL;
			return;
		}
		//slog( "CELL: %i, DOES NOT MATCH: %i", cell->entity_index_list[i], entity->_id );
	}

	slog ( "CollisionCellRemoveEntity: Entity not in cell" );
	slog ( "CollisionCellRemoveEntity: Entity Count: %i", cell->entity_count );
	//cell->entity_count = 0;
	return;
}

void collision_cell_update ( CollisionCell* self )
{
	if ( !self ) return;
	Uint32 entCount = 0;
	for ( int i = 0; i < self->max_entities; i++ )
	{
		if ( self->entity_list[i] ) entCount++;
	}
	self->entity_count = entCount;
	if ( self->entity_count > 0 )
	{
		for ( int i = 0; i < self->entity_count; i++ )
		{
			Entity* first = self->entity_list[i];
			if ( !first ) continue;
			if (!first->collision_enabled) continue;
			//for ( int h = 0; h < self->entity_count; h++ )
			//{
			//	if ( h == i ) continue;
			//	Entity* second = self->entity_list[h];
			//	if ( !second ) continue;
			//
			//	CollisionInfo info;
			//	info.side = COL_NULL;
			//	if (collision_rect_test( first->bounds, second->bounds, &info ))
			//	{
			//		entity_on_collision(first, info);
			//	//	entity_on_collision(second, info);
			//	}
			//}
			//
			CollisionInfo info;
			info.side = COL_NULL;
			info.time = g_time;

			StaticEntityManager *static_entity_manager = static_entity_manager_get();
			Uint32 ent_id;
			Vector2D rtrn_hit, tmp;
			float dist;
			float old_dist = FLT_MAX;
			Uint8 collision = false;

			int i;
			for (i = 0; i < static_entity_manager->static_entity_count; i++)
			{
				if (!static_entity_manager->static_entity_list[i]._inuse) continue;

				StaticEntity *sEnt = &static_entity_manager->static_entity_list[i];

				if (collision_rect_test( first->bounds, sEnt->bounds, &info ))
				{
					entity_on_collision( first, info );
				}
				else
				{
					first->col_info.side = COL_NULL;
				}

			}
			collision_system_check_neighbor_cells_for_collision ( self, first );
		}
	}
}

int collision_rect_test ( Rect A, Rect B, CollisionInfo *info_out )
{
	if ( A.x < B.x + B.w &&
		A.x + A.w > B.x &&
		A.y < B.y + B.h &&
		A.h + A.y > B.y )
	{
		SDL_Rect collisionRect;
		CollisionInfo leftRight, topBottom;
		if (A.x <= B.x)
		{
			collisionRect.x = B.x;
			collisionRect.w = (A.x + A.w) - B.x;
			leftRight.side = COL_LEFT;
			leftRight.left = true;
			leftRight.right = false;
		}
		else
		{
			collisionRect.x = A.x;
			collisionRect.w = (B.x + B.w) - A.x;
			leftRight.side = COL_RIGHT;
			leftRight.right = true;
			leftRight.left = true;
		}
		if (A.y <= B.y)
		{
			collisionRect.y = B.y;
			collisionRect.h = (A.y + A.h) - B.y;
			topBottom.side = COL_TOP;
			topBottom.top = true;
			topBottom.bottom = false;
		}
		else
		{
			collisionRect.y = A.y;
			collisionRect.h = (B.y + B.h) - A.y;
			topBottom.side = COL_BOTTOM;
			topBottom.bottom = true;
			topBottom.top = true;
		}


		if (collisionRect.w < collisionRect.h)
		{
			info_out->side = leftRight.side;
			info_out->left = leftRight.left;
			info_out->right = leftRight.right;

			info_out->top = false;
			info_out->bottom = false;
		}
		else
		{
			info_out->side = topBottom.side;
			info_out->top = topBottom.top;
			info_out->bottom = topBottom.bottom;

			info_out->left = false;
			info_out->right = false;
		}
		if ( g_debug ) gf2d_draw_fill_rect ( collisionRect, vector4d ( 255, 0, 0, 100 ) );

		return 1;
	}
	return 0;
}

int collision_line_rect_test ( Rect A, Line B, Vector2D* hit_point )
{
	// check if the line has hit any of the rectangle's sides

	Vector2D vec_top, vec_bottom, vec_left, vec_right;
	Line top, bottom, left, right;

	Uint8 col_top, col_bottom, col_left, col_right;

	top.a = vector2d ( A.x, A.y );
	top.b = vector2d ( A.x + A.w, A.y );

	bottom.a = vector2d ( A.x, A.y + A.h );
	bottom.b = vector2d ( A.x + A.w, A.y + A.h );

	left.a = vector2d ( A.x, A.y );
	left.b = vector2d ( A.x, A.y + A.h );

	right.a = vector2d ( A.x + A.w, A.y );
	right.b = vector2d ( A.x + A.w, A.y + A.h );

	//gf2d_draw_line( top.a, top.b, vector4d( 255, 255, 0, 255 ) );
	//gf2d_draw_line( bottom.a, bottom.b, vector4d( 255, 255, 0, 255 ) );
	//gf2d_draw_line( left.a, left.b, vector4d( 255, 255, 0, 255 ) );
	//gf2d_draw_line( right.a, right.b, vector4d( 255, 255, 0, 255 ) );
	//
	//gf2d_draw_line( B.a, B.b, vector4d( 255, 255, 0, 255 ) );

	col_top = collision_line_line_test ( B, top, &vec_top );
	col_bottom = collision_line_line_test ( B, bottom, &vec_bottom );
	col_left = collision_line_line_test ( B, left, &vec_left );
	col_right = collision_line_line_test ( B, right, &vec_right );

	Vector2D a;
	float dist;
	float old_dist = FLT_MAX;



	if ( col_top )
	{
		vector2d_sub ( a, B.a, vec_top );
		dist = vector2d_magnitude_squared ( a );

		if ( dist < old_dist )
		{
			hit_point->x = vec_top.x;
			hit_point->y = vec_top.y;

			old_dist = dist;
		}
	}
	if ( col_bottom )
	{
		vector2d_sub ( a, B.a, vec_bottom );
		dist = vector2d_magnitude_squared ( a );

		if ( dist < old_dist )
		{
			hit_point->x = vec_bottom.x;
			hit_point->y = vec_bottom.y;

			old_dist = dist;
		}
	}
	if ( col_left )
	{
		vector2d_sub ( a, B.a, vec_left );
		dist = vector2d_magnitude_squared ( a );

		if ( dist < old_dist )
		{
			hit_point->x = vec_left.x;
			hit_point->y = vec_left.y;

			old_dist = dist;
		}
	}
	if ( col_right )
	{
		vector2d_sub ( a, B.a, vec_right );
		dist = vector2d_magnitude_squared ( a );

		if ( dist < old_dist )
		{
			hit_point->x = vec_right.x;
			hit_point->y = vec_right.y;

			old_dist = dist;
		}
	}

	if ( col_top || col_bottom || col_left || col_right )
	{
		return true;
	}
	//if( collision_line_line_test( bottom, B, &vec_bottom ) )
	//{
	//	vector2d_sub( a, vector2d( A.x, A.y ), vec_bottom );
	//	dist = vector2d_magnitude_squared( a );
	//
	//	if (dist < old_dist)
	//	{
	//		old_dist = dist;
	//	}
	//}
	//if( collision_line_line_test( left, B, &vec_left ) )
	//{
	//
	//	vector2d_sub( a, vector2d( A.x, A.y ), vec_left );
	//	dist = vector2d_magnitude_squared( a );
	//
	//	if (dist < old_dist)
	//	{
	//		hit_point = &vec_left;
	//		old_dist = dist;
	//	}
	//}
	//if (collision_line_line_test( right, B, &vec_right ))
	//{
	//	vector2d_sub( a, vector2d( A.x, A.y ), vec_right );
	//	dist = vector2d_magnitude_squared( a );
	//
	//	if (dist < old_dist)
	//	{
	//		hit_point = &vec_right;
	//		old_dist = dist;
	//	}
	//}

	return false;
}

int collision_line_line_test ( Line A, Line B, Vector2D* hit_point )
{
	// calculate the direction of the lines
	float uA = ((B.b.x - B.a.x) * (A.a.y - B.a.y) - (B.b.y - B.a.y) * (A.a.x - B.a.x)) / ((B.b.y - B.a.y) * (A.b.x - A.a.x) - (B.b.x - B.a.x) * (A.b.y - A.a.y));
	float uB = ((A.b.x - A.a.x) * (A.a.y - B.a.y) - (A.b.y - A.a.y) * (A.a.x - B.a.x)) / ((B.b.y - B.a.y) * (A.b.x - A.a.x) - (B.b.x - B.a.x) * (A.b.y - A.a.y));

	// if uA and uB are between 0-1, lines are colliding
	if ( uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1 )
	{

		// optionally, draw a circle where the lines meet
		hit_point->x = A.a.x + (uA * (A.b.x - A.a.x));
		hit_point->y = A.a.y + (uA * (A.b.y - A.a.y));

		//slog( "Draw: %f | %f", hit_point->x, hit_point->y );
		//gf2d_draw_circle( vector2d(hit_point->x, hit_point->y), 16, vector4d( 255, 255, 0, 255 ) );

		return true;
	}
	return false;
}

Uint8 collision_point_rect_test( Vector2D p, Rect r )
{
	if (p.x >= r.x &&
		 p.x <= r.x + r.w &&
		 p.y >= r.y &&
		 p.y <= r.y + r.h)
	{
		return true;
	}

	return false;
}

void collision_system_update_all ()
{
	int i;
	for ( i = 0; i < collision_system.cell_count; i++ )
	{
		if ( !collision_system.cell_list[i]._inuse )// not used yet
		{
			continue;// skip this iteration of the loop
		}
		collision_cell_update ( &collision_system.cell_list[i] );
	}
}

// TODO: Clean this function up!
HitObj raycast ( Vector2D origin, Vector2D direction, float max_distance, Uint32 id_mask, Uint32 team_mask )
{
	HitObj hit;
	hit.entity = NULL;

	Vector2D hit_point = vector2d ( 0, 0 );
	Vector2D a, b, temp;
	float m = direction.x / direction.y;
	Line line;
	Rect rect;

	line.a = origin;

	//slog( "(%f, %f)", direction.x, direction.y );
	if ( m == 0 )
	{
		a.x = origin.x + max_distance;
		a.y = origin.y;

		b.x = origin.x - max_distance;
		b.y = origin.y;
	}
	else if ( m == FLT_MAX )
	{
		a.x = origin.x;
		a.y = origin.y + max_distance;

		b.x = origin.x;
		b.y = origin.y - max_distance;
	}
	else
	{
		float dx = (max_distance / sqrt ( 1 + (m * m) ));
		float dy = m * dx;

		a.x = origin.x + dx;
		a.y = origin.y + dy;
		b.x = origin.x - dx;
		b.y = origin.y - dy;
	}


	if ( direction.x < 0 && direction.y < 0 )
	{
		line.b = b;
		//gf2d_draw_circle( b, 6, vector4d( 255, 0, 255, 255 ) );
	//	gf2d_draw_line ( origin, b, vector4d ( 255, 255, 255, 100 ) );
	}
	else if ( direction.x > 0 && direction.y < 0 )
	{
		line.b = b;
		//gf2d_draw_circle( b, 6, vector4d( 255, 0, 255, 255 ) );
	//	gf2d_draw_line ( origin, b, vector4d ( 255, 255, 255, 100 ) );
	}
	else
	{
		line.b = a;
		//gf2d_draw_circle( a, 6, vector4d( 255, 0, 0, 255 ) );
	//	gf2d_draw_line ( origin, a, vector4d ( 255, 255, 255, 100 ) );
	}

	EntityManager* entity_manager = entity_manager_get ();
	StaticEntityManager *static_entity_manager = static_entity_manager_get();

	Uint32 ent_id = -1;
	Uint32 sEnt_id = -1;
	Vector2D rtrn_hit, tmp;
	float dist, ent_dist, sEnt_dist;
	float old_dist = FLT_MAX;
	Uint8 collision = false;

	for ( int i = 0; i < entity_manager->entity_count; i++ )
	{
		if ( !entity_manager->entity_list[i]._inuse ) continue;
		if ( entity_manager->entity_list[i]._id == id_mask ) continue;
		if (!entity_manager->entity_list[i].collision_enabled) continue;
		if (entity_manager->entity_list[i].team == team_mask) continue;

		Entity* ent = &entity_manager->entity_list[i];

		rect.x = ent->bounds.x;
		rect.y = ent->bounds.y;
		rect.w = ent->bounds.w;
		rect.h = ent->bounds.h;

		if ( collision_line_rect_test ( rect, line, &rtrn_hit ) )
		{
			vector2d_sub ( tmp, origin, rtrn_hit );
			dist = vector2d_magnitude_squared ( tmp );

		//	gf2d_draw_circle ( rtrn_hit, 8, vector4d ( 255, 0, 255, 255 ) );
			if ( dist < old_dist )
			{
				hit_point.x = rtrn_hit.x;
				hit_point.y = rtrn_hit.y;

				ent_id = ent->_id;
				collision = true;
				old_dist = dist;
			}
		}
	}

	for (int i = 0; i < static_entity_manager->static_entity_count; i++)
	{
		if (!static_entity_manager->static_entity_list[i]._inuse) continue;

		StaticEntity *sEnt = &static_entity_manager->static_entity_list[i];

		rect.x = sEnt->bounds.x;
		rect.y = sEnt->bounds.y;
		rect.w = sEnt->bounds.w;
		rect.h = sEnt->bounds.h;

		if (collision_line_rect_test( rect, line, &rtrn_hit ))
		{
			vector2d_sub( tmp, origin, rtrn_hit );
			dist = vector2d_magnitude_squared( tmp );

		//	gf2d_draw_circle( rtrn_hit, 8, vector4d( 255, 0, 255, 255 ) );
			if (dist < old_dist)
			{
				hit_point.x = rtrn_hit.x;
				hit_point.y = rtrn_hit.y;

				sEnt_id = sEnt->_id;
				ent_id = -1; // If this is closer, we know the entity is not visible
				collision = true;
				old_dist = dist;
			}
		}
	}
	if ( collision )
	{
		hit.position.x = hit_point.x;
		hit.position.y = hit_point.y;
		if (ent_id != -1) hit.entity = &entity_manager->entity_list[ent_id - 1];
		if ( sEnt_id != -1) hit.static_entity = &static_entity_manager->static_entity_list[sEnt_id - 1];

	}

	return hit;
}

HitObj raycast_between( Vector2D origin, Vector2D target, float max_distance, Uint32 id_mask, Uint32 team_mask )
{
	Vector2D dir = look_at_angle_slope( origin, target );

	return raycast( origin, dir, max_distance, id_mask, team_mask );
}
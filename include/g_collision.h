#ifndef __G_COLLISION__
#define __G_COLLISION__

#include "g_entity.h"

typedef struct CollisionCell_S
{
	Uint8				_inuse;
	Uint32				id;
	Uint32				max_entities;
	Uint32				entity_count;	/**<Number of entities in the collision cell*/
	Vector2D			bBox;
	Vector2D			cell_position;	/**<Cell position*/
	Uint32				entity_index_list[128];	/**<List of all entities in cell*/
	struct Entity_S** entity_list;
}CollisionCell;

typedef struct
{
	Vector2D position;
	struct Entity_S* entity;
}HitObj;

typedef struct
{
	float x;
	float y;
	float w;
	float h;
}Rect;

typedef struct
{
	Vector2D a;
	Vector2D b;
}Line;

typedef struct
{
	Vector2D vert1;
	Vector2D vert2;
	Vector2D vert3;
	Vector2D vert4;
}Polygon;

/**
 * @brief initialize internal collision system
 * @param number of horizontal and vertical cells
 */
void collision_system_init ( Vector2D cell_xy );
void collision_system_generate_cells ( Vector2D cell_xy );
void collision_system_check_neighbor_cells_for_collision ( CollisionCell* cell, struct Entity_S* entity );
CollisionCell* collision_system_get_cell_by_index ( Uint32 index );

void collision_system_draw_debug ();
void collision_system_close ();
void collision_cell_free( CollisionCell *self );

CollisionCell* collision_system_get_nearest_cell_within_range ( Vector2D position, float distance );

void collision_cell_add_entity ( CollisionCell* cell, struct Entity_S* entity );

void collision_cell_remove_entity ( CollisionCell* cell, struct Entity_S* entity );

void collision_system_update_all ();

int collision_rect_test ( Rect A, Rect B );

int collision_line_line_test ( Line A, Line B, Vector2D* hit_point );

int collision_line_rect_test ( Rect A, Line B, Vector2D* hit_point );

HitObj raycast ( Vector2D origin, Vector2D direction, float max_distance, Uint32 id_mask );
#endif
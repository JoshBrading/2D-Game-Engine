#ifndef __G_COLLISION__
#define __G_COLLISION__

#include "g_entity.h"

typedef struct CollisionCell_S
{
	Uint8				_inuse;
	Uint32				entity_count;	/**<Number of entities in the collision cell*/
	Vector2D			bBox;
	Vector2D			cell_position;	/**<Cell position*/
	struct Entity_S		**entity_list;	/**<List of all entities in cell*/
}CollisionCell;

void collision_system_init( Vector2D cell_xy );
void collision_system_generate_cells( Vector2D cell_xy );

void collision_system_draw_debug();
void collision_detect_cell();
#endif
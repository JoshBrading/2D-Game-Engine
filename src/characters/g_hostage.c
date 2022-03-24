#include "g_hostage.h"
#include "g_globals.h"
#include "g_func.h"
#include "gf2d_draw.h"
#include "simple_logger.h"

void hostage_think_fixed( Entity *self );
void hostage_debug( Entity *self );
void hostage_collision( Entity *self, CollisionInfo info );


Entity *hostage_new()
{
	Entity *ent = NULL;

	ent = entity_new();
	ent->thinkFixed = hostage_think_fixed;
	ent->drawDebug = hostage_debug;
	ent->onCollision = hostage_collision;



	ent->scale.x = 0.125;
	ent->scale.y = 0.125;

	ent->offset.x = 16;
	ent->offset.y = 16;

	ent->bounds.w = 32;
	ent->bounds.h = 32;
	ent->bounds.x = ent->position.x - ent->offset.x;
	ent->bounds.y = ent->position.y - ent->offset.y;

	ent->toggles.B = false;
	ent->toggles.A = false;
	ent->move_speed = 3;
	ent->offset.x = 16;
	ent->offset.x = 16;

	ent->position = vector2d( 1130, 375 );

	ent->health = 999;

	return ent;
}

void hostage_think_fixed( Entity *self )
{
	if (self->state == ENT_CHASE)
	{
		Entity *player = entity_manager_get_by_id( 1 );
		Vector2D newPos;
		vector2d_move_towards( &newPos, self->position, player->position, self->move_speed );

		self->position = newPos;
	}
}

void hostage_debug( Entity *self )
{

}

void hostage_collision( Entity *self, CollisionInfo info )
{

}
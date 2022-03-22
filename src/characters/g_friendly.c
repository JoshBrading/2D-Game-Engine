#include "g_friendly.h"
#include "g_globals.h"
#include "g_func.h"
#include "gf2d_draw.h"
#include "simple_logger.h"

void friendly_think( Entity *self );
void friendly_debug( Entity *self );
void friendly_collision( Entity *self, CollisionInfo info );

Entity *friendly_new()
{
	Entity *ent = NULL;

	ent = entity_new();
	ent->team = 0;
	ent->think = friendly_think;
	ent->drawDebug = friendly_debug;
	ent->onCollision = friendly_collision;

	ent->toggles.B = false;
	ent->toggles.A = false;
	ent->move_speed = 1;
	ent->offset.x = 16;
	ent->offset.x = 16;

	ent->health = 10;

	return ent;
}

void friendly_think( Entity *self )
{
	HitObj hit;
	Entity *player;
	Uint8 leftMouseButton = 0;

	SDL_Event e;
	SDL_PollEvent( &e );
	if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		leftMouseButton = 1;
		//slog ( "Mouse clicked" );
	}

	if (e.type == SDL_MOUSEBUTTONUP)
	{
		leftMouseButton = 0;
	}

	player = entity_manager_get_by_id( 1 );

	if (player->path.pos_count == player->path.pos_max)
	{
		self->toggles.A = false;

		self->path.positions[0] = player->path.positions[0];
		self->path.positions[1] = player->path.positions[1];
		self->path.positions[2] = player->path.positions[2];
		self->path.positions[3] = player->path.positions[3];

		self->path.pos_count = player->path.pos_count;
		self->path.pos_max = player->path.pos_max;

		for (int i = 0; i < self->path.pos_max; i++)
		{
			self->path.positions[i].x  += rand() % 128 - 64;
			self->path.positions[i].y  += rand() % 128 - 64;
		}

	}
	else if (player->path.pos_count == player->path.pos_max - 1)
	{
		self->toggles.A = true;
	}

	if (self->path.pos_count != 0 && self->toggles.A )
	{
		entity_follow_path( self, self->path.pos_max - self->path.pos_count, 32 );
	}

	//gf2d_draw_line( self->position, player->position, vector4d( 255, 0, 0, 50 ) );
	//gf2d_draw_circle( self->position, self->view_range, vector4d( 255, 0, 255, 255 ) );

	hit = raycast( self->position, look_at_angle_slope( self->position, player->position ), self->view_range, self->_id );

	self->path.color = vector4d( 255, 0, 0, 255 );


	//if (hit.entity)
	//{
	//	if (hit.entity->_inuse && hit.entity->team == 1)
	//	{
	//		self->state = ENT_ATTACK;
	//	}
	//}
	//else if (self->state != ENT_IDLE)
	//{
	//	self->state = ENT_WANDER;
	//}
	//
	//
	//if (self->state == ENT_WANDER)
	//{
	//	self->target_position = player->position;
	//
	//
	//	if (!vector2d_distance_between_less_than( self->position, self->target_position, 100 ))
	//	{
	//		Vector2D newPos;
	//		vector2d_move_towards( &newPos, self->position, self->target_position, self->move_speed );
	//
	//		CollisionInfo col = self->col_info;
	//
	//		if (col.time + 100 > g_time)
	//		{
	//			if (col.top || col.bottom)
	//			{
	//				newPos.y = self->position.y;
	//			}
	//
	//			if (col.left || col.right)
	//			{
	//				newPos.x = self->position.x;
	//			}
	//		}
	//
	//		self->position = newPos;
	//	}
	//	else
	//	{
	//		int idle_timer = self->idle_time_max - self->idle_time_max;
	//		self->timers.A = g_time + self->idle_time_max;
	//		self->state = ENT_IDLE;
	//		self->target_position = player->position;
	//	}
	//}
	//else if (self->state == ENT_IDLE)
	//{
	//	if (self->timers.A < g_time)
	//	{
	//
	//		self->state = ENT_WANDER;
	//	}
	//}
	//
	//
	//if (self->state != ENT_WANDER) self->toggles.A = false;
	//
	//if (self->col_info.time < g_time) self->toggles.B = false;
}

void f_get_target_position( Entity *self )
{
	Vector2D newTarget;

	newTarget.x = rand() % self->nav_zone.w + self->nav_zone.x;
	newTarget.y = rand() % self->nav_zone.h + self->nav_zone.y;

	self->target_position = newTarget;
}

void friendly_debug( Entity *self )
{
	gf2d_draw_fill_rect( self->nav_zone, vector4d( 255, 255, 0, 15 ) );


	if (self->path.pos_count != 0 && self->toggles.A)
	{
		gf2d_draw_line( self->position, self->path.positions[self->path.pos_max - self->path.pos_count], vector4d( 0, 0, 255, 255 ) );
		gf2d_draw_circle( self->path.positions[0], 32, self->path.color );
		gf2d_draw_circle( self->path.positions[1], 32, self->path.color );
		gf2d_draw_circle( self->path.positions[2], 32, self->path.color );
		gf2d_draw_circle( self->path.positions[3], 32, self->path.color );
	}

}

void friendly_collision( Entity *self, CollisionInfo info )
{
	self->col_info = info;

	// Wiggle around :)
	if (info.side == COL_LEFT)		self->position.x += 0.01;
	if (info.side == COL_RIGHT)		self->position.x -= 0.01;
	if (info.side == COL_TOP)		self->position.y += 0.01;
	if (info.side == COL_BOTTOM)	self->position.y -= 0.01;
}

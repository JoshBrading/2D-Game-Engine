#include "g_friendly.h"
#include "g_globals.h"
#include "g_func.h"
#include "gf2d_draw.h"
#include "simple_logger.h"

void friendly_think_fixed( Entity *self );
void friendly_think( Entity *self );
void friendly_debug( Entity *self );
void friendly_collision( Entity *self, CollisionInfo info );

Entity *friendly_new()
{
	Entity *ent = NULL;

	ent = entity_new();
	ent->team = TEAM_FRIEND;
	ent->thinkFixed = friendly_think_fixed;
	ent->think = friendly_think;
	ent->drawDebug = friendly_debug;
	ent->onCollision = friendly_collision;

	ent->toggles.B = false;
	ent->toggles.A = false;
	ent->move_speed = 4;
	ent->offset.x = 16;
	ent->offset.x = 16;

	ent->health = 100;

	return ent;
}

void friendly_think_fixed( Entity *self )
{
	HitObj hit;
	Entity *player;


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

	hit = raycast( self->position, look_at_angle_slope( self->position, player->position ), self->view_range, self->_id, TEAM_FRIEND );

	self->path.color = vector4d( 255, 0, 0, 255 );

	if (strcmp( self->tag, "demo" ) == 0 && self->state == ENT_ACTION)
	{
		if (!vector2d_distance_between_less_than( self->position, self->target_position, 15 ))
		{
			Vector2D newPos;
			vector2d_move_towards( &newPos, self->position, self->target_position, self->move_speed );

			self->position = newPos;
		}
		else
		{
			self->door_ent->state = DOOR_BLOWN;
			self->door_ent->collision_enabled = false;
		}
	}
	else if (strcmp( self->tag, "shotgun" ) == 0 && self->state == ENT_ACTION)
	{
		if (!vector2d_distance_between_less_than( self->position, self->target_position, 15 ))
		{
			Vector2D newPos;
			vector2d_move_towards( &newPos, self->position, self->target_position, self->move_speed );

			self->position = newPos;
		}
		else
		{
			self->door_ent->state = DOOR_SHOT;
			self->door_ent->collision_enabled = false;
			self->state = ENT_WANDER;
		}
	}
}

void friendly_think( Entity *self )
{

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
		
		gf2d_draw_circle( self->path.positions[0], 6, self->path.color );
		gf2d_draw_circle( self->path.positions[1], 6, self->path.color );
		gf2d_draw_circle( self->path.positions[2], 6, self->path.color );
		gf2d_draw_circle( self->path.positions[3], 6, self->path.color );
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

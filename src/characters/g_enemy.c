#include "g_enemy.h"
#include "g_globals.h"
#include "g_func.h"
#include "gf2d_draw.h"
#include "simple_logger.h"
#include "w_rifle.h"

void enemy_think_fixed( Entity *self );
void debug( Entity *self );
void get_target_position( Entity *self );
void enemy_collision( Entity *self, CollisionInfo info );
void set_visibility( Entity *self, Entity *check );
void enemy_die( Entity *self );

Entity *enemy_new()
{
	Entity *ent = NULL;

	ent = entity_new();

	ent->sprite = gf2d_sprite_load_image( "images/enemy.png" );
	ent->scale.x = 0.125;
	ent->scale.y = 0.125;

	ent->offset.x = 16;
	ent->offset.y = 16;

	ent->bounds.w = 32;
	ent->bounds.h = 32;
	ent->bounds.x = ent->position.x - ent->offset.x;
	ent->bounds.y = ent->position.y - ent->offset.y;

	ent->team = TEAM_ENEMY;
	ent->tag = "enemy";
	ent->thinkFixed = enemy_think_fixed;
	ent->drawDebug = debug;
	ent->onCollision = enemy_collision;
	ent->onDeath = enemy_die;

	ent->toggles.B = false;

	ent->move_speed = 4;
	ent->offset.x = 16;
	ent->offset.x = 16;

	ent->health = 100;

	ent->weapon = rifle_new();
	ent->weapon->owner = ent;
	ent->weapon->ammo = 999;

	return ent;
}

void enemy_think_fixed( Entity* self )
{
	if (!self->_inuse) return;
	HitObj hit;
	Entity *player;

	player = entity_manager_get_by_id( 1 );

	//gf2d_draw_line( self->position, player->position, vector4d(255, 0, 0, 50) );
	//gf2d_draw_circle( self->position, self->view_range, vector4d( 255, 0, 255, 255 ) );

	set_visibility( self, player );
	hit = raycast( self->position, look_at_angle_slope( self->position, player->position ), self->view_range, self->_id, TEAM_ENEMY );


	if (hit.entity)
	{
		if (hit.entity->_inuse && hit.entity->team == TEAM_FRIEND)
		{
			self->state = ENT_ATTACK;
			self->weapon->state = WEP_FIRE;
		}
		else
		{
			self->weapon->state = WEP_IDLE;
		}
	}
	else if (self->state != ENT_IDLE)
	{
		self->state = ENT_WANDER;
	}


	if (self->state == ENT_WANDER)
	{

		if (self->timers.B < g_time) // Force new paths every x seconds
		{
			get_target_position( self );
			self->timers.B = g_time + 5000;
		}

		if (!vector2d_distance_between_less_than( self->position, self->target_position, self->move_speed ))
		{
			Vector2D newPos;
			vector2d_move_towards( &newPos, self->position, self->target_position, self->move_speed );

			CollisionInfo col = self->col_info;

			if (col.time + 100 > g_time)
			{
				if (col.top || col.bottom)
				{
					newPos.y = self->position.y;
				}

				if (col.left || col.right)
				{
					newPos.x = self->position.x;
				}
			}

			self->position = newPos;
		}
		else
		{
			int idle_timer = self->idle_time_max - self->idle_time_max;
			self->timers.A = g_time + self->idle_time_max;
			self->state = ENT_IDLE;
			get_target_position( self );
		}
	}
	else if (self->state == ENT_IDLE)
	{
		if (self->timers.A < g_time)
		{

			self->state = ENT_WANDER;
		}
	}


	if (self->state != ENT_WANDER) self->toggles.A = false;

	//if (self->col_info.time < g_time) self->toggles.B = false;
}

void get_target_position( Entity *self )
{
	Vector2D newTarget;

	newTarget.x = rand() % self->nav_zone.w + self->nav_zone.x;
	newTarget.y = rand() % self->nav_zone.h + self->nav_zone.y;

	self->target_position = newTarget;
}

void debug( Entity *self )
{
	gf2d_draw_fill_rect( self->nav_zone, vector4d( 255, 255, 0, 15 ) );
	gf2d_draw_line( self->position, self->target_position, vector4d( 0, 0, 255, 255 ) );
	gf2d_draw_circle( self->target_position, 8, vector4d( 0, 0, 255, 255 ) );

	gf2d_draw_circle( self->position, self->view_range, vector4d( 255, 0, 255, 255 ) );
}

void enemy_collision( Entity *self, CollisionInfo info )
{
	self->col_info = info;

	// Wiggle around :)
	if (info.side == COL_LEFT)		self->position.x +=	0.01;
	if (info.side == COL_RIGHT)		self->position.x -=	0.01;
	if (info.side == COL_TOP)		self->position.y +=	0.01;
	if (info.side == COL_BOTTOM)	self->position.y -=	0.01;
}

void set_visibility( Entity *self, Entity* check )
{
	HitObj vis_check;
	vis_check = raycast( self->position, look_at_angle_slope( self->position, check->position ), 512, self->_id, TEAM_ENEMY );
	if (vis_check.entity)
	{
		if (vis_check.entity->_inuse && vis_check.entity->team == TEAM_FRIEND && vis_check.entity == check)
		{
			self->visibility = true;
			return;
		}
	}
	self->visibility = false;
}

void enemy_die( Entity *self )
{

}
#include "simple_logger.h"
#include "g_test_bounce_ball.h"
#include "g_func.h"
#include "g_globals.h"

void ball_think_fixed ( Entity* self );
void on_collision ( Entity* self, CollisionInfo collision );
void die( Entity *self );
Entity* ball_new ()
{
	Entity* ent = NULL;
	ent = entity_new ();

	ent->thinkFixed = ball_think_fixed;
	ent->onCollision = on_collision;
	ent->onDeath = die;
	ent->offset.x = 16;
	ent->offset.x = 16;

	ent->speed.x = 1;
	ent->speed.y = 1;
	ent->damage = rand () % 10 + 1;
	ent->health = 10;
	return ent;
}

int ballDir = 2;
int ballDirY = 1;
void ball_think_fixed ( Entity* self )
{

	self->position = lerp_vector_2d( self->position, vector2d(256, 256), 0.1 );
	//( "Test: %f", self->health );
	//int speed = 2; // self->damage;
	//if ( self->position.x > 1100 ) self->speed.x = -1;
	//if ( self->position.x < 32 ) self->speed.x = 1;
	//if ( self->position.y > 600 ) self->speed.y = -1;
	//if ( self->position.y < 32 ) self->speed.y = 1;
	//int directionX = self->speed.x;
	//int directionY = self->speed.y;
	//self->position.x += speed * directionX;
	//self->position.y += speed * directionY;
}

void die( Entity *self )
{
	collision_cell_remove_entity( self->cell, self );
	entity_free( self );
}

void on_collision( Entity *self, CollisionInfo collision )
{

	if (self->col_timer < g_time)
	{
		int speedX = self->speed.x;
		int speedY = self->speed.y;

		if (collision.side == COL_TOP)
		{
			speedY *= -1;
			//slog( "TOP" );
		}
		else if (collision.side == COL_BOTTOM)
		{
			speedY *= -1;
			//slog( "BOTTOM" );
		}
		else if (collision.side == COL_LEFT)
		{
			speedX *= -1;
			//slog( "LEFT" );
		}
		else if (collision.side == COL_RIGHT)
		{
			speedX *= -1;
			//slog( "RIGHT" );
		}

		self->speed.x = speedX;
		self->speed.y = speedY;

		self->col_timer = g_time + 100;
	}

}
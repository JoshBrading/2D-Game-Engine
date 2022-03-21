#include "simple_logger.h"
#include "gf2d_draw.h"
#include "g_globals.h"
#include "g_door.h"

void door_think( Entity *self );
void door_think_fixed( Entity *self );
void door_shot( Entity *self );

Entity *door_new()
{
	Entity *door = NULL;
	door = entity_new();

	door->think = door_think;
	door->thinkFixed = door_think_fixed;
	door->onDeath = door_shot;
}

void door_think( Entity* self )
{
	const Uint8 *keys;
	keys = SDL_GetKeyboardState( NULL ); // get the keyboard state for this frame

	SDL_Event e;
	SDL_PollEvent( &e );

	if (keys[SDL_SCANCODE_E])
	{
		Entity *player = entity_manager_get_by_id( 1 );
		Vector2D vector;
		vector2d_sub( vector, player->position, self->position );
		float dist = vector2d_magnitude_squared( vector );
		if (0 < self->interact_radius)
		{
			if (self->state == DOOR_OPENED)
			{
				self->state = DOOR_CLOSED;
			}
			else if (self->state == DOOR_CLOSED)
			{
				self->state = DOOR_OPENED;
			}
		}
	}
	if (keys[SDL_SCANCODE_F])
	{
		if (self->state == DOOR_CLOSED)
		{
			slog( "BLOWN" );
			self->state = DOOR_BLOWN;

		}
	}

//	gf2d_draw_circle( self->position, self->interact_radius, vector4d( 255, 255, 200, 255 ) );
}

void door_think_fixed( Entity* self )
{
	if (!self) return;

	if (self->door_timer < g_time)
	{
		if (self->state == DOOR_OPENED && self->frame < self->sprite->frames_per_line - 1)
		{
			self->frame += 1;
		}
		else if (self->state == DOOR_CLOSED && self->frame != 0)
		{
			self->frame -= 1;
		}
		else if (self->state == DOOR_SHOT)
		{
			if (self->frame >= self->sprite->frames_per_line * 2 - 1)
			{
				self->state = DOOR_DEAD;
			}
			else if (self->frame >= self->sprite->frames_per_line)
			{
				self->frame += 1;
			}
			else if( self->frame < self->sprite->frames_per_line)
			{
				self->frame = self->sprite->frames_per_line;
			}
		}
		else if (self->state == DOOR_BLOWN)
		{
			if (self->frame == self->sprite->frames_per_line * 3 - 1)
			{
				slog( "L" );
				self->state = DOOR_DEAD;
			}
			else if (self->frame >= self->sprite->frames_per_line * 2)
			{
				self->frame += 1;
			}
			else if (self->frame < self->sprite->frames_per_line * 2)
			{
				self->frame = self->sprite->frames_per_line * 2;
			}

		}
		self->door_timer = g_time + self->speed.x;
	}
	if (self->state == DOOR_DEAD || self->state == DOOR_OPENED)
	{
		slog( "OPEN or DEAD" );
		self->collision_enabled = false;
	}
	else
	{
		self->collision_enabled = true;
	}
}

void door_shot( Entity *self )
{
	if (!self) return;
	if (self->state == DOOR_CLOSED)
	{
		self->state = DOOR_SHOT;
	}
}
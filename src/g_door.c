#include "simple_logger.h"
#include "gf2d_draw.h"
#include "g_globals.h"
#include "g_door.h"
#include "g_hud.h"

void door_think( Entity *self );
void door_think_fixed( Entity *self );
void door_shot( Entity *self );
void door_debug( Entity *self );

Entity *door_new()
{
	Entity *door = NULL;
	door = entity_new();
	door->tag = "door";
	door->team = TEAM_NULL;
	door->think = door_think;
	door->thinkFixed = door_think_fixed;
	//door->onDeath = door_shot;
	door->drawDebug = door_debug;
	door->door_unlocked = false;
}

void door_think( Entity* self )
{
	if (self->state == DOOR_DEAD) return;
	Vector2D interact_zone;
	vector2d_add( interact_zone, self->position, self->interact_offset );
	Entity *player = entity_manager_get_by_id( 1 );

	const Uint8 *keys;
	keys = SDL_GetKeyboardState( NULL ); // get the keyboard state for this frame

	SDL_Event e;
	SDL_PollEvent( &e );

	if (keys[SDL_SCANCODE_E] && vector2d_distance_between_less_than( interact_zone, player->position, self->interact_radius ) && self->state)
	{
		if (self->door_unlocked)
		{
			Vector2D vector;

			if (self->state == DOOR_OPENED)
			{
				self->state = DOOR_CLOSED;
			}
			else if (self->state == DOOR_CLOSED)
			{
				self->state = DOOR_OPENED;
			}
		}
		else
		{
			self->state = DOOR_DIALOG;
		}
	}
	Vector2D m;
	Uint32 mx, my;
	SDL_GetMouseState( &mx, &my );

	m.x = mx;
	m.y = my;
	//if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT)
	//{
	//	if (vector2d_distance_between_less_than( interact_zone, player->position, self->interact_radius ))
	//	{
	//		//if (collision_point_rect_test( m, self->bounds ))
	//		//{
	//			self->state = DOOR_DIALOG;
	//		//}
	//	}
	//}

	if (self->state == DOOR_DIALOG && vector2d_distance_between_less_than( interact_zone, player->position, self->interact_radius ))
	{
		SDL_Rect rect = { player->position.x - 128, player->position.y, 128, 32 };
		gf2d_draw_fill_rect( rect, vector4d( 0, 0, 0, 128 ) );
		HUD_draw_message( "Door Locked", vector2d( player->position.x - 128, player->position.y ), vector3d( 255, 0, 0 ), 128, 32 );

		rect.x = player->position.x;
		rect.w = 168;
		rect.h = 112;

		gf2d_draw_fill_rect( rect, vector4d( 0, 0, 0, 128 ) );
		HUD_draw_message( "BREACH OPTIONS", player->position, vector3d( 255, 255, 255 ), 168, 48 );
		HUD_draw_message( "1: Explosive Charge", vector2d( player->position.x, player->position.y + 40 ), vector3d( 255, 255, 255 ), 128, 32 );
		HUD_draw_message( "2: Shotgun Breach", vector2d( player->position.x, player->position.y + 64 ), vector3d( 255, 255, 255 ), 128, 32 );

		if (keys[SDL_SCANCODE_1])
		{
			Entity *ent;
			ent = entity_get_by_tag( "demo" );
			if (ent)
			{
				ent->state = ENT_ACTION;
				ent->target_position = vector2d( self->position.x, self->position.y + self->bounds.h / 2 );
				ent->door_ent = self;
			}
		}
		else if (keys[SDL_SCANCODE_2])
		{
			Entity *ent;
			ent = entity_get_by_tag( "shotgun" );
			if (ent)
			{
				ent->state = ENT_ACTION;
				ent->target_position = vector2d( self->position.x, self->position.y + self->bounds.h / 2 );
				ent->door_ent = self;
			}
		}
	}
	else if (self->state == DOOR_DIALOG);
	{
		if (!vector2d_distance_between_less_than( interact_zone, player->position, self->interact_radius ))
		{
			self->state = DOOR_CLOSED;

		}
	}
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
	//if (self->state == DOOR_DEAD || self->state == DOOR_OPENED)
	//{
	//	self->collision_enabled = false;
	//}
	//else
	//{
	//	self->collision_enabled = true;
	//}
}

void door_shot( Entity *self )
{
	if (!self) return;
	//if (self->state == DOOR_CLOSED)
	//{
	//	self->state = DOOR_SHOT;
	//}
}

void door_debug( Entity *self )
{
	if (!self) return;
	Vector2D interact_zone;
	vector2d_add( interact_zone, self->position, self->interact_offset );
	gf2d_draw_circle( interact_zone, self->interact_radius, vector4d( 255, 255, 200, 255 ) );
}
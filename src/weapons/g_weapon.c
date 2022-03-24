#include "g_weapon.h"
#include "simple_logger.h"
#include "g_func.h"
#include "g_particle.h"

typedef struct
{
	Weapon* weapon_list;
	Uint32	weapon_count;
}WeaponManager;

static WeaponManager weapon_manager = { 0 };

void weapon_manager_init ( Uint32 maxWeapons )
{
	weapon_manager.weapon_list = gfc_allocate_array ( sizeof ( Weapon ), maxWeapons ); // This is probably wrong...
	if ( weapon_manager.weapon_list == NULL )
	{
		slog ( "WeaponManagerInit: Failed to allocate array of size: %i", maxWeapons );
		return;
	}
	weapon_manager.weapon_count = maxWeapons;
	//atexit( weapon_manager_close );
	slog ( "WeaponManager: Initialized" );
}

void weapon_manager_close()
{
	int i;
	for (i = 0; i < weapon_manager.weapon_count; i++)
	{
		weapon_free( &weapon_manager.weapon_list[i] );
	}
	free( weapon_manager.weapon_list );
	slog( "WeaponManager: Closed" );
}

Weapon *weapon_new()
{
	int i;
	for (i = 0; i < weapon_manager.weapon_count; i++)
	{
		if (!weapon_manager.weapon_list[i]._inuse)
		{
			weapon_manager.weapon_list[i]._inuse = 1;

			return (&weapon_manager.weapon_list[i]);
		}
	}
	slog( "EntityNew: No free space" );
	return NULL;
}

void weapon_free( Weapon *self )
{
	if (!self)
	{
		slog( "WeaponFree: Entity does not exist" );
		return;
	}
	if (self->sprite)
	{
		gf2d_sprite_free( self->sprite );
	}
	memset( self, 0, sizeof( self ) );
}


void weapon_draw( Weapon *self )
{
	if (!self) return;
	if (!self->sprite) return;
	//gf2d_sprite_draw( self->sprite, vector2d( self->position.x - self->offset.x, self->position.y - self->offset.y ), &self->scale, NULL, &self->rotation, NULL, NULL, self->frame );
}

void weapon_manager_draw_all()
{
	for (int i = 0; i < weapon_manager.weapon_count; i++)
	{
		if (weapon_manager.weapon_list[i]._inuse)
		{
			weapon_draw( &weapon_manager.weapon_list[i] );
		}
	}

}

void weapon_think( Weapon *self )
{
	if (!self)return;
	if (self->think)self->think( self );
}

void weapon_think_fixed( Weapon *self )
{
	if (!self)return;
	if (self->thinkFixed)self->thinkFixed( self );
}

void weapon_manager_think_all()
{
	int i;
	for (i = 0; i < weapon_manager.weapon_count; i++)
	{
		if (!weapon_manager.weapon_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		weapon_think( &weapon_manager.weapon_list[i] );
	}
}

void weapon_manager_think_fixed_all()
{
	int i;
	for (i = 0; i < weapon_manager.weapon_count; i++)
	{
		if (!weapon_manager.weapon_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		weapon_think_fixed( &weapon_manager.weapon_list[i] );
	}
}

void weapon_update( Weapon *self )
{
	if (!self)return;

	if (self->update)self->update( self );
}

void weapon_update_fixed( Weapon *self )
{
	if (!self)return;

	if (self->updateFixed)self->updateFixed( self );
}

void weapon_manager_update_all()
{
	int i;
	for (i = 0; i < weapon_manager.weapon_count; i++)
	{
		if (!weapon_manager.weapon_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		weapon_update( &weapon_manager.weapon_list[i] );
	}
}

void weapon_manager_update_fixed_all()
{
	int i;
	for (i = 0; i < weapon_manager.weapon_count; i++)
	{
		if (!weapon_manager.weapon_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		weapon_update_fixed( &weapon_manager.weapon_list[i] );
	}
}


void weapon_shoot( Weapon *self )
{
	self->ammo -= 1;

	HitObj hit;
	Vector2D origin = self->owner->position;
	Vector2D target;

	SDL_Event e;
	SDL_PollEvent( &e );

	int mx, my;
	SDL_GetMouseState( &mx, &my );

	target.x = mx;
	target.y = my;

	hit = raycast( origin, look_at_angle_slope( origin, target ), 1024, self->owner->_id, self->owner->team );

	Particle *particle = particle_new();
	particle->position = hit.position;
	particle->scale.x = 0.1;
	particle->scale.y = 0.1;
	particle->sprite = self->impact;
	particle->life_time = 32;
	particle->timescale = 0.5;
	particle->offset.x = 10;
	particle->offset.y = 10;
	particle->frame = 1;

	if (hit.entity == NULL) return;
	if (hit.entity->_inuse)
	{
		//slog ( "Hit entity with ID: %i", hit.entity->_id );
		//gf2d_draw_line( self->position, hit.position, vector4d( 255, 255, 0, 255 ) );
		//hit.entity->damage ( hit.entity, 1.0f, self );
		if (strcmp( hit.entity->tag, "door" ) == 0)
		{
			//hit.entity->state = DOOR_SHOT;
		}
		else
		{
			set_health( hit.entity, self->damage, self );
		}
	}

}

void weapon_reload( Weapon *self )
{
	if (self->reserve_ammo > 0)
	{
		Uint32 ammo = self->max_ammo - self->ammo;
		self->ammo += ammo;
		self->reserve_ammo -= ammo;

		self->state = WEP_IDLE;
		return;
	}
	
	self->state = WEP_OUT_OF_AMMO;

}

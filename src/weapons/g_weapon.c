#include "g_weapon.h"
#include "simple_logger.h"

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

void weapon_free( Entity *self )
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


void weapon_draw( Entity *self )
{
	if (!self) return;
	if (!self->sprite) return;
	gf2d_sprite_draw( self->sprite, vector2d( self->position.x - self->offset.x, self->position.y - self->offset.y ), &self->scale, NULL, &self->rotation, NULL, NULL, self->frame );
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

void weapon_think( Entity *self )
{
	if (!self)return;
	if (self->think)self->think( self );
}

void weapon_think_fixed( Entity *self )
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

void weapon_update( Entity *self )
{
	if (!self)return;

	if (self->update)self->update( self );
}

void weapon_update_fixed( Entity *self )
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

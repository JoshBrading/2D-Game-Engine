#include <SDL.h.>
#include "gf2d_draw.h"
#include "g_entity.h"
#include "simple_logger.h"
#include "g_collision.h"

#include "g_globals.h"

typedef struct
{

	Entity *entity_list;
	Uint32	entity_count;

}EntityManager;

static EntityManager entity_manager = { 0 };

void entity_manager_close()
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		entity_free( &entity_manager.entity_list[i] );
	}
	free( entity_manager.entity_list );
	slog( "EntityManager: Closed" );
}

void entity_manager_init( Uint32 maxEntities )
{
	entity_manager.entity_list = gfc_allocate_array( sizeof( Entity ), maxEntities );
	if (entity_manager.entity_list == NULL)
	{
		slog( "EntityManagerInit: Failed to allocate array of size: %i", maxEntities );
		return;
	}
	entity_manager.entity_count = maxEntities;
	atexit( entity_manager_close );
	slog( "EntityManager: Initialized" );
}

void entity_manager_draw_all()
{
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (entity_manager.entity_list[i]._inuse)
		{
			entity_draw( &entity_manager.entity_list[i] );
		}
	}

}void entity_manager_draw_debug()
{
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (entity_manager.entity_list[i]._inuse)
		{
			if ( entity_manager.entity_list[i].bounds.x != 0 && entity_manager.entity_list[i].bounds.y != 0 )
			{
				SDL_Rect rectToDraw = { entity_manager.entity_list[i].bounds.x, entity_manager.entity_list[i].bounds.y, entity_manager.entity_list[i].bounds.w, entity_manager.entity_list[i].bounds.h };
				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 255, 0, 255 ) );
			}

		}
	}
}

void entity_manager_clear()
{
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (entity_manager.entity_list[i]._inuse)
		{
			entity_free( &entity_manager.entity_list[i] );
		}
	}
}

Entity *entity_new()
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)
		{
			entity_manager.entity_list[i]._inuse = 1;
			entity_manager.entity_list[i].scale.x = 1;
			entity_manager.entity_list[i].scale.y = 1;
			entity_manager.entity_list[i]._id = i + 1;

			return (&entity_manager.entity_list[i]);
		}
	}
	slog( "EntityNew: No free space" );
	return NULL;
}

void entity_draw( Entity *self )
{
	if (!self) return;
	if (!self->sprite) return;
	gf2d_sprite_draw( self->sprite, self->position, NULL, NULL, NULL, NULL, NULL, self->frame );
}

Entity *entity_manager_get_by_id( Uint32 id )
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		if (entity_manager.entity_list[i]._inuse)// not used yet
		{
			if (entity_manager.entity_list[i]._id == id)
			{
				return &entity_manager.entity_list[i];
			}
		}
	}
	return NULL;
}

void entity_free( Entity *self )
{
	if (!self)
	{
		slog( "EntityFree: Entity does not exist" );
		return;
	}
	if (self->sprite)
	{
		gf2d_sprite_free( self->sprite );
	}
	memset( self, 0, sizeof( self ) );
}

void entity_think( Entity *self )
{
	if (!self)return;
	if (self->think)self->think( self );
}

void entity_think_fixed( Entity *self )
{
	if (!self)return;
	if (self->thinkFixed)self->thinkFixed( self );
}

void entity_manager_think_all()
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		entity_think( &entity_manager.entity_list[i] );
	}
}

void entity_manager_think_fixed_all()
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		entity_think_fixed( &entity_manager.entity_list[i] );
	}
}

void entity_update( Entity *self )
{
	if (!self)return;
	self->bounds.x = self->position.x;
	self->bounds.y = self->position.y;
	CollisionCell *cell = collision_system_get_nearest_cell_within_range( self->position, 32.0f );
	if (&self->cell->id != &cell->id)
	{
		collision_cell_remove_entity( self->cell, self );
		collision_cell_add_entity( cell, self );
	}
	//Uint8 g_debug;
	if (g_debug)
	{
		slog( "Test" );
	}
	if (self->update)self->update( self );
}

void entity_update_fixed( Entity *self )
{
	if (!self)return;

	if (self->updateFixed)self->updateFixed( self );
}

void entity_manager_update_all()
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		entity_update( &entity_manager.entity_list[i] );
	}
}

void entity_manager_update_fixed_all()
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
		entity_update_fixed( &entity_manager.entity_list[i] );
	}
}
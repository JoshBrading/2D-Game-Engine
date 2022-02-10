#include <SDL.h.>
#include "gf2d_draw.h"
#include "g_entity.h"
#include "simple_logger.h"

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
			if ( entity_manager.entity_list[i].bBoxX != NULL && entity_manager.entity_list[i].bBoxY != NULL )
			{
				SDL_Rect rectToDraw = { entity_manager.entity_list[i].position.x, entity_manager.entity_list[i].position.y, entity_manager.entity_list[i].bBoxX, entity_manager.entity_list[i].bBoxY };
				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 0, 0, 255 ) );
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
			return (&entity_manager.entity_list[i]);
		}
	}
	slog( "EntityNew: No free space" );
	return;
}

void entity_draw( Entity *self )
{
	if (!self) return;
	if (!self->sprite) return;
	gf2d_sprite_draw( self->sprite, self->position, NULL, NULL, NULL, NULL, NULL, self->frame );
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


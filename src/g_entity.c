#include <SDL.h.>
#include "gf2d_draw.h"
#include "g_entity.h"
#include "simple_logger.h"
#include "g_collision.h"
#include "g_hud.h"
#include "g_globals.h"

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
	entity_manager.debug = false;
	atexit( entity_manager_close );
	slog( "EntityManager: Initialized" );
}

void entity_manager_draw_all() // Make this better in the future...
{
	//for (int i = 0; i < entity_manager.entity_count; i++)
	//{
	//	if (entity_manager.entity_list[i]._inuse)
	//	{
	//		if(strcmp( entity_manager.entity_list[i].tag, "door") == 0)
	//			entity_draw( &entity_manager.entity_list[i] );
	//	}
	//}

	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (entity_manager.entity_list[i]._inuse)
		{
			//if(strcmp( entity_manager.entity_list[i].tag, "door" ) != 0)
				entity_draw( &entity_manager.entity_list[i] );
		}
	}

	if (entity_manager.debug) entity_manager_draw_debug();
}

void entity_manager_draw_debug()
{
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (entity_manager.entity_list[i]._inuse)
		{
			Entity *entity = &entity_manager.entity_list[i];
			if (entity->bounds.x != 0 && entity->bounds.y != 0 )
			{
				SDL_Rect rectToDraw = { entity->bounds.x, entity->bounds.y, entity->bounds.w, entity->bounds.h };
				gf2d_draw_rect ( rectToDraw, vector4d ( 255, 255, 0, 255 ) );
			}

			char *tag[32];
			char *visibility[32];
			char *health[32];
			char *position[32];

			snprintf( tag, sizeof( tag ), "Tag: %s", entity->tag );
			snprintf( visibility, sizeof( visibility ), "Visible: %i", entity->visibility );
			snprintf( health, sizeof( health ), "Health: %0.2f", entity->health );
			snprintf( position, sizeof( position ), "Pos: (%0.2f, %0.2f)", entity->position.x, entity->position.y );

			Vector2D tag_pos = { entity->bounds.x + entity->bounds.w + 4, entity->bounds.y };
			Vector2D visibility_pos = { entity->bounds.x + entity->bounds.w + 4, entity->bounds.y + 16 };
			Vector2D health_pos = { entity->bounds.x + entity->bounds.w + 4, entity->bounds.y + 32 };
			Vector2D position_pos = { entity->bounds.x + entity->bounds.w + 4, entity->bounds.y + 48 };

			HUD_draw_message( tag, tag_pos, vector3d( 255, 255, 255 ), 0, 0 );
			HUD_draw_message( visibility, visibility_pos, vector3d( 255, 255, 255 ), 0, 0 );
			HUD_draw_message( health, health_pos, vector3d( 255, 255, 255 ), 0, 0 );
			HUD_draw_message( position, position_pos, vector3d( 255, 255, 255 ), 0, 0 );
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
			entity_manager.entity_list[i].offset.x = 0;
			entity_manager.entity_list[i].offset.y = 0;
			entity_manager.entity_list[i]._id = i + 1; // FIXME
			entity_manager.entity_list[i].col_timer = 0;
			entity_manager.entity_list[i].collision_enabled = true;
			entity_manager.entity_list[i].visibility = true;
			entity_manager.entity_list[i].tag = "NULL";
			entity_manager.entity_list[i].team = TEAM_NULL;

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
	if( self->visibility ) 
		gf2d_sprite_draw( self->sprite, vector2d(self->position.x - self->offset.x, self->position.y - self->offset.y), &self->scale, NULL, &self->rotation, NULL, NULL, self->frame );
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
	self->_inuse = false;
	memset( self, 0, sizeof( self ) );
}

void entity_think( Entity *self )
{
	if (!self)return;
	if (self->think)self->think( self );

	if (strcmp( self->tag, "hostage" ) != 0) return;

	SDL_Rect rect = { 28, 28, 178, 100 };
	gf2d_draw_fill_rect( rect, vector4d( 0, 0, 0, 128 ) );

	if (vector2d_distance_between_less_than( self->position, vector2d( 32, 32 ), 64 ))
	{
		HUD_draw_message( "Rescue The Hostage", vector2d( 32, 64 ), vector3d( 0, 255, 0 ), 170, 21 );
	}
	else
	{
		HUD_draw_message( "Rescue The Hostage", vector2d( 32, 64 ), vector3d( 255, 255, 255 ), 170, 21 );
	}
}

void entity_think_fixed( Entity *self )
{
	if (!self)return;
	if (self->thinkFixed)self->thinkFixed( self );

	if (strcmp( self->tag, "hostage" ) != 0) return;

	if (self->state == ENT_CHASE)
	{
		Entity *player = entity_manager_get_by_id( 1 );
		Vector2D newPos;
		if (!vector2d_distance_between_less_than( self->position, player->position, 64 ))
		{
			vector2d_move_towards( &newPos, self->position, player->position, self->move_speed );
			self->position = newPos;

		}

	}
	
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

	if (self->col_info.time < g_time + 100)
	{
		self->col_info.top = false;
		self->col_info.bottom = false;
		self->col_info.left = false;
		self->col_info.right = false;
	}

	vector2d_sub( self->center, self->position, self->offset );
	self->bounds.x = self->position.x - self->offset.x;
	self->bounds.y = self->position.y - self->offset.y;
	CollisionCell *cell = collision_system_get_nearest_cell_within_range( self->center, 32.0f );
	if (&self->cell->id != &cell->id)
	{
		collision_cell_remove_entity( self->cell, self );
		collision_cell_add_entity( cell, self );
	}

	if (self->health <= 0)
	{
		entity_die( self );
	}
	if (self->update)self->update( self );

	if (entity_manager.debug && self->drawDebug) self->drawDebug( self );

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

Entity* entity_manager_get_entity_in_range( Vector2D position, float range )
{
	int i;
	for (i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse)// not used yet
		{
			continue;// skip this iteration of the loop
		}
	//	entity_update( &entity_manager.entity_list[i] );
	}

	return NULL;
}

EntityManager *entity_manager_get()
{
	return &entity_manager;
}

void set_health ( Entity* ent, float damage, Entity* inflictor )
{
	ent->health -= damage;
	//slog ( "Health: %f", ent->health );
}
void damage ( Entity* self, float damage, Entity* inflictor )
{
	self->health -= damage;
	//slog ( "Entity: %i damaged by: %i", self->_id, inflictor->_id );
}

void entity_die ( Entity* self )
{
	if (!self) return;
	if (self->onDeath)self->onDeath( self );

	entity_free( self );
}

void entity_on_collision( Entity* self, CollisionInfo collision )
{
	if (!self) return;
	if (self->onCollision)self->onCollision( self, collision );
}

void entity_follow_path( Entity *self, Uint32 index, float radius )
{
	//slog( "Index: %i", index );

	if (!vector2d_distance_between_less_than( self->position, self->path.positions[index], radius ))
	{
		Vector2D newPos;
		vector2d_move_towards( &newPos, self->position, self->path.positions[index], self->move_speed );

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
		self->path.positions[self->path.pos_max - self->path.pos_count] = vector2d( -1, -1 );
		self->path.pos_count -= 1;
	}


}

Uint32 entity_get_enemy_count()
{
	Uint32 enemy_count = 0;
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse) continue;
		if (entity_manager.entity_list[i].team == TEAM_ENEMY)
		{
			enemy_count += 1;
		}
	}

	return enemy_count;
}

Uint32 entity_get_friendly_count()
{
	Uint32 friendly_count = 0;
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse) continue;
		if (entity_manager.entity_list[i].team == TEAM_FRIEND)
		{
			friendly_count += 1;
		}
	}

	return friendly_count;
}

Uint32 entity_get_intel_count()
{
	Uint32 intel_count = 0;
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse) continue;
		if (strcmp( entity_manager.entity_list[i].tag, "intel") == 0)
		{
			intel_count += 1;
		}
	}

	return intel_count;
}

Entity *entity_get_by_tag( char *tag )
{
	for (int i = 0; i < entity_manager.entity_count; i++)
	{
		if (!entity_manager.entity_list[i]._inuse) continue;
		if (strcmp( entity_manager.entity_list[i].tag, tag) == 0)
		{
			return &entity_manager.entity_list[i];
		}
	}
	return NULL;
}

Entity *entity_manager_get_player()
{
	return entity_get_by_tag( "player" ); // Update to use ID
}

void entity_manager_enable_debug() 
{
	entity_manager.debug = true;
}

void entity_manager_disable_debug() 
{
	entity_manager.debug = false;
}
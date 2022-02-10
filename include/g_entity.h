#ifndef __G_ENTITY_H__
#define __G_ENTITY_H__

#include "gf2d_sprite.h"

typedef struct Entity_S
{
	Uint8		_inuse;		/**<Flag to check if entity is being used*/

	Sprite		*sprite;	/**<a pointer to the sprite that is used by this entity*/
	float		frame;		/**<Current frame of sprite*/

	int			bBoxX;		/**<Bounding box of the sprite +- X*/
	int			bBoxY;		/**<Bounding box of the sprite +- Y*/

	Vector2D	position;	/**<where the entity is in the world*/
	Vector2D	scale;		/**<Vector2D Scale of entity*/

	Uint8		visibility;	/**<Current visibility of entity*/
	float		health;		/**<Current health of entity*/
	//Weapon		weapon; /**<Currently held weapon by the entity*/

	int			state;		/**<Current state of entity, waiting, attacking*/
}Entity;

/**
 * @brief initialize internal entity management system
 * @param max_entities how many concurrent entities to support
 */
void entity_manager_init( Uint32 maxEntities );

/**
 * @brief call draw on all entities
 */
void entity_manager_draw_all();

/**
 * @brief Draw colliders for all entities
 */
void entity_manager_draw_debug();

/**
 * @brief call free on all entities
 */
void entity_manager_clear();

/**
 * @brief allocate an entity, initialize it to zero and return a pointer to it
 * @return NULL on error (see logs) or a pointer to an initialized entity.
 */
Entity *entity_new();

/**
 * @brief frees provided entity
 * @param ent the entity to free
 */
void entity_free( Entity *self );

/**
 * @brief draws an entity to the current render frame
 * @param ent the entity to draw
 */
void entity_draw( Entity *self );

#endif
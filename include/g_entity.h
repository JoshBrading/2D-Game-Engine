#ifndef __G_ENTITY_H__
#define __G_ENTITY_H__

#include "gf2d_sprite.h"
#include "g_collision.h"

typedef struct Entity_S
{
	Uint8					_inuse;		/**<Flag to check if entity is being used*/

	Uint32					_id;		/**<ID of an entity, assume ID cannot be 0*/
	Sprite					*sprite;	/**<a pointer to the sprite that is used by this entity*/
	float					frame;		/**<Current frame of sprite*/

	Uint32					bBoxX;		/**<Bounding box of the sprite +- X*/
	Uint32					bBoxY;		/**<Bounding box of the sprite +- Y*/

	Vector2D				center;
	Vector2D				position;	/**<where the entity is in the world*/
	Vector2D				scale;		/**<Vector2D Scale of entity*/

	Uint8					visibility;	/**<Current visibility of entity*/
	float					health;		/**<Current health of entity*/
	//Weapon					weapon; /**<Currently held weapon by the entity*/

	Uint32					state;		/**<Current state of entity, waiting, attacking*/
	char					*tag;		/**<Tag for naming the entity*/
	Uint8					team;		/**<Team the entity is on*/
	struct CollisionCell_S*	cell;		/**<Current cell position of the entity, used for collision detection*/

	void               (*think)(struct Entity_S *self); /* <pointer to the think function */
	void               (*thinkFixed)(struct Entity_S *self); /* <pointer to the think fixed function */

	void               (*update)(struct Entity_S *self); /* <pointer to the update function */
	void               (*updateFixed)(struct Entity_S *self); /* <pointer to the update fixed function */

	void               (*damage)(struct Entity_S *self, float damage, struct Entity_S *inflictor); /* <pointer to the damage function */
	void               (*onDeath)(struct Entity_S *self); /* <pointer to a funciton to call when the entity dies */

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

/**
 * @brief run the think functions for ALL active entities
 */
void entity_manager_think_all();

/**
 * @brief run the think functions for ALL active entities at a fixed rate
 */
void entity_manager_think_fixed_all();

/**
 * @brief run the update functions for ALL active entities
 */
void entity_manager_update_all();

/**
 * @brief Fixed update runs the update functions for all entities at a fixed rate
 */
void entity_manager_update_fixed_all();
#endif